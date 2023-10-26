//
// Copyright (C) 2000 Institut fuer Telematik, Universitaet Karlsruhe
// Copyright (C) 2004,2011 Andras Varga
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, see <http://www.gnu.org/licenses/>.
//

#include "inet/applications/udpapp/RSUListen.h"

#include "inet/applications/base/ApplicationPacket_m.h"
#include "inet/networklayer/common/L3AddressResolver.h"
#include "inet/common/ModuleAccess.h"
#include "inet/mobility/contract/IMobility.h"
#include "inet/common/lifecycle/NodeOperations.h"
#include "inet/transportlayer/contract/udp/UDPControlInfo_m.h"
#include "inet/mobility/single/HaifaMobilityManager.h"
#include <stdio.h>
#include <iostream>
#include <fstream>

namespace inet {

Define_Module(RSUListen);

simsignal_t RSUListen::sentPkSignal = registerSignal("sentPk");
simsignal_t RSUListen::rcvdPkSignal = registerSignal("rcvdPk");
simsignal_t RSUListen::rcvdPkByVissimSignal = registerSignal("rcvdPkByVissim");
simsignal_t RSUListen::E2Edelay = registerSignal("E2Edelay");


RSUListen::~RSUListen()
{
    cancelAndDelete(selfMsg);
}

void RSUListen::initialize(int stage)
{
    ApplicationBase::initialize(stage);

    if (stage == INITSTAGE_LOCAL) {
        numSent = 0;
        numReceived = 0;
        numReceivedByVissim = 0;
        WATCH(numSent);
        WATCH(numReceived);
        WATCH(numReceivedByVissim);

        localPort = par("localPort");
        destPort = par("destPort");
        //startTime = par("startTime").doubleValue();
       // stopTime = par("stopTime").doubleValue();
        packetName = par("packetName");
        isActive = false;
        wasActivated = false;
        //if (stopTime >= SIMTIME_ZERO && stopTime < startTime)
          //  throw cRuntimeError("Invalid startTime/stopTime parameters");
        selfMsg = new cMessage("sendTimer");
        std::ofstream receivedPKVis;
        receivedPKVis.open("receivedPKByVISSIM.csv", std::ios::out | std::ios::trunc);
        receivedPKVis << "VissimID;SimSecond;latency;numberOfRcvdPkByVISSIM" << endl;
        receivedPKVis.close();
        startTransmission();
    }
}

void RSUListen::finish()
{
    recordScalar("packets sent", numSent);
    recordScalar("packets received", numReceived);
    recordScalar("packet received by VISSIM", numReceivedByVissim);
    ApplicationBase::finish();
}

void RSUListen::setSocketOptions()
{
    int timeToLive = par("timeToLive");
    if (timeToLive != -1)
        socket.setTimeToLive(timeToLive);

    int typeOfService = par("typeOfService");
    if (typeOfService != -1)
        socket.setTypeOfService(typeOfService);

    const char *multicastInterface = par("multicastInterface");
    if (multicastInterface[0]) {
        IInterfaceTable *ift = getModuleFromPar<IInterfaceTable>(par("interfaceTableModule"), this);
        InterfaceEntry *ie = ift->getInterfaceByName(multicastInterface);
        if (!ie)
            throw cRuntimeError("Wrong multicastInterface setting: no interface named \"%s\"", multicastInterface);
        socket.setMulticastOutputInterface(ie->getInterfaceId());
    }

    bool receiveBroadcast = par("receiveBroadcast");
    if (receiveBroadcast)
        socket.setBroadcast(true);

    bool joinLocalMulticastGroups = par("joinLocalMulticastGroups");
    if (joinLocalMulticastGroups) {
        MulticastGroupList mgl = getModuleFromPar<IInterfaceTable>(par("interfaceTableModule"), this)->collectMulticastGroups();
        socket.joinLocalMulticastGroups(mgl);
    }
}

L3Address RSUListen::chooseDestAddr()
{
    int k = intrand(destAddresses.size());
    if (destAddresses[k].isLinkLocal()) {    // KLUDGE for IPv6
        const char *destAddrs = par("destAddresses");
        cStringTokenizer tokenizer(destAddrs);
        const char *token = nullptr;

        for (int i = 0; i <= k; ++i)
            token = tokenizer.nextToken();
        destAddresses[k] = L3AddressResolver().resolve(token);
    }
    return destAddresses[k];
}

void RSUListen::sendPacket()
{
    cModule *host = getContainingNode(this);
    EV_INFO << "hostModuleName: " << host;
    IMobility  *mod = check_and_cast<IMobility *>(host->getSubmodule("mobility"));
    Coord pos = mod->getCurrentPosition();
    HaifaMobility *hMob = check_and_cast<HaifaMobility *>(mod);
    std::ostringstream str;
    std::ostringstream seq;
    std::string se;
    str << packetName << "-" << numSent;
    VeinsRSUPacket *payload = new VeinsRSUPacket(str.str().c_str());
    payload->setByteLength(par("messageLength").intValue());
    payload->setSequenceNumber(numSent);
    payload->setCasOdoslania(simTime());
    payload->setPrikaz(100);
    payload->setPosX(pos.x);
    payload->setPosY(pos.y);
    int idv = hMob->getVisID();
    payload->setVisID(idv);
    seq << getParentModule();
    se = seq.str();
    //EV_INFO << "PackSequence: " << se <<endl;
    payload->setSequence(se.c_str());
    L3Address destAddr = chooseDestAddr();
    emit(sentPkSignal, payload);
    socket.sendTo(payload, destAddr, destPort);
    numSent++;
}

void RSUListen::processStart()
{
    socket.setOutputGate(gate("udpOut"));
    const char *localAddress = par("localAddress");
    socket.bind(*localAddress ? L3AddressResolver().resolve(localAddress) : L3Address(), localPort);
    setSocketOptions();

    const char *destAddrs = par("destAddresses");
    cStringTokenizer tokenizer(destAddrs);
    const char *token;

    while ((token = tokenizer.nextToken()) != nullptr) {
        L3Address result;
        L3AddressResolver().tryResolve(token, result);
        if (result.isUnspecified())
            EV_ERROR << "cannot resolve destination address: " << token << endl;
        else
            destAddresses.push_back(result);
    }

    if (!destAddresses.empty()) {
        selfMsg->setKind(SEND);
        processSend();
    }
}

void RSUListen::processSend()
{
    sendPacket();
    simtime_t d = simTime() + par("sendInterval").doubleValue();
        selfMsg->setKind(SEND);
        scheduleAt(d, selfMsg);
}

void RSUListen::processStop()
{
    socket.close();
}

void RSUListen::handleMessageWhenUp(cMessage *msg)
{
    if (msg->isSelfMessage()) {
        ASSERT(msg == selfMsg);
        switch (selfMsg->getKind()) {
            case START:
                processStart();
                break;

            case SEND:
                processSend();
                break;

            case STOP:
                processStop();
                break;

            default:
                throw cRuntimeError("Invalid kind %d in self message", (int)selfMsg->getKind());
        }
    }
    else if (msg->getKind() == UDP_I_DATA) {
        // process incoming packet
        //processPacket(PK(msg));
        prijmiPaket(check_and_cast<VeinsRSUPacket *>(msg));
    }
    else if (msg->getKind() == UDP_I_ERROR) {
        EV_WARN << "Ignoring UDP error report\n";
        delete msg;
    }
    else {
        throw cRuntimeError("Unrecognized message (%s)%s", msg->getClassName(), msg->getName());
    }
}

void RSUListen::refreshDisplay() const
{
    char buf[100];
    sprintf(buf, "rcvd: %d pks\nsent: %d pks", numReceived, numSent);
    getDisplayString().setTagArg("t", 0, buf);
}

void RSUListen::processPacket(cPacket *pk)
{
    emit(rcvdPkSignal, pk);
    EV_INFO << "Received packet: " << UDPSocket::getReceivedPacketInfo(pk) << endl;
    delete pk;
    numReceived++;
}

void RSUListen::prijmiPaket(VeinsRSUPacket *pk)
{
    cModule *hostRSU = getContainingNode(this);
    EV_INFO << "RSUHostModuleName: " << hostRSU;
    IMobility  *modRSU = check_and_cast<IMobility *>(hostRSU->getSubmodule("mobility"));
    Coord posRSU = modRSU->getCurrentPosition();
    double distance = sqrt(pow((posRSU.x-pk->getPosX()),2)+pow((posRSU.y-pk->getPosY()),2));
    emit(rcvdPkSignal, pk);
    EV_INFO << "Received packet: " << UDPSocket::getReceivedPacketInfo(pk) << endl;
    if (pk->getPrikaz() == 100)
    {
    latency = ((simTime() - pk->getCasOdoslania())*1000);
    emit(E2Edelay, latency);
    //E2Elatency.record(latency);
    EV_INFO << "PackSequence: " << pk->getSequence() << ";" << pk->getCasOdoslania() << ";"<< std::endl;
    EV_INFO  << "E2ELatency: " << latency.dbl() << "\n";
    if(pk->getCasOdoslania() > int(simTime().dbl()))
    {
    addToOutputForVissim(pk->getVisID(), pk->getPosX(), pk->getPosY());
    addToManagerLPI(pk->getVisID(),distance);
    addToManagerLEVandLAS(pk->getVisID(),pk->getApproaching());
    emit(rcvdPkByVissimSignal, pk);
    //std::cout << "Received packet vissimID: " << pk->getVisID() << endl;
    numReceivedByVissim++;
    std::ofstream receivedPKVis;
    receivedPKVis.open("receivedPKByVISSIM.csv", std::ios_base::app);
    receivedPKVis << pk->getVisID() << ";" << int(simTime().dbl()) << ";" << latency.dbl() << ";" << numReceivedByVissim << endl;
    receivedPKVis.close();
    }
    }
    delete pk;
    numReceived++;
    PacketLossEvaluator *ev = check_and_cast<PacketLossEvaluator *>(getModuleByPath("ScenarioTest.evaluator"));
    ev->addRcvdPk();

}

void RSUListen::addToOutputForVissim(int vissimID, double posX, double posY)
{
    HaifaMobilityManager *hManager = check_and_cast<HaifaMobilityManager *>(getModuleByPath("^.^.mobilityManager"));
    hManager->addToVissimOutput(vissimID, posX, posY);
}

void RSUListen::addToManagerLEVandLAS(int vID, bool approaching)
{
    HaifaMobilityManager *hManager = check_and_cast<HaifaMobilityManager *>(getModuleByPath("^.^.mobilityManager"));
    hManager->addToLEVandLAS(vID,approaching);
}

void RSUListen::addToManagerLPI(int vID, double distanceToRSU)
{
    HaifaMobilityManager *hManager = check_and_cast<HaifaMobilityManager *>(getModuleByPath("^.^.mobilityManager"));
    hManager->addToLPI(vID,distanceToRSU);
}

// !!!!! WHEN TRANSITIONING TO THE NEW INTERSECTION MODEL, THIS BLOCK MUST BE CHANGED ACCORDINGLY !!!!!
/*
const char *RSUListen::getApproach(double PosX, double PosY)
{
    if(PosX <= 165 && PosY <= 15 && PosY >= -20)// all vehicles from west
                      {
                           return "w";
                      }
               if(PosX > 180 && PosY <= 30 && PosY >= -10)// all vehicles from east
                        {
                           return "e";
                        }
               if(PosY >= 10 && PosX <= 180 && PosX >= 150)// all vehicles from north
                           {
                               return "n";
                           }
               if(PosY < -10 && PosX <= 190 && PosX >= 160)// all vehicles from south
                               {
                                   return "s";
                               }
               return "";
}
*/

void RSUListen::startTransmission()
{
    Enter_Method_Silent();
    if (!isActive && !wasActivated)
    {
    selfMsg->setKind(START);
    scheduleAt(simTime()+uniform(0,0.01), selfMsg);
    isActive = true;
    wasActivated = true;
    }
}

void RSUListen::stopTransmission()
{
    Enter_Method_Silent();
    if(isActive)
    {
        if(selfMsg)
        {
            cancelEvent(selfMsg);
            selfMsg->setKind(STOP);
            scheduleAt(simTime(), selfMsg);
            isActive = false;
        }
    }
}

bool RSUListen::handleNodeShutdown(IDoneCallback *doneCallback)
{
    if (selfMsg)
        cancelEvent(selfMsg);
    //TODO if(socket.isOpened()) socket.close();
    return true;
}

void RSUListen::handleNodeCrash()
{
    if (selfMsg)
        cancelEvent(selfMsg);
}

} // namespace inet

