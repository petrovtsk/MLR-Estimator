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

#include "inet/applications/udpapp/InformRSU.h"

#include "inet/applications/base/ApplicationPacket_m.h"
#include "inet/networklayer/common/L3AddressResolver.h"
#include "inet/common/ModuleAccess.h"
#include "inet/mobility/contract/IMobility.h"
#include "inet/common/lifecycle/NodeOperations.h"
#include "inet/transportlayer/contract/udp/UDPControlInfo_m.h"
#include "inet/mobility/single/HaifaMobilityManager.h"
#include <stdio.h>

namespace inet {

Define_Module(InformRSU);

simsignal_t InformRSU::sentPkSignal = registerSignal("sentPk");
simsignal_t InformRSU::rcvdPkSignal = registerSignal("rcvdPk");
simsignal_t InformRSU::E2Edelay = registerSignal("E2Edelay");


InformRSU::~InformRSU()
{
    cancelAndDelete(selfMsg);
}

void InformRSU::initialize(int stage)
{
    ApplicationBase::initialize(stage);

    if (stage == INITSTAGE_LOCAL) {
        numSent = 0;
        numReceived = 0;
        WATCH(numSent);
        WATCH(numReceived);

        HaifaMobilityManager *hMobilityManager = check_and_cast<HaifaMobilityManager *>(getModuleByPath("^.^.mobilityManager"));
        RSUPosX = hMobilityManager->par("RSUPosX");
        RSUPosY = hMobilityManager->par("RSUPosY");

        localPort = par("localPort");
        destPort = par("destPort");
        destAdd = par("destAddresses");
        //startTime = par("startTime").doubleValue();
       // stopTime = par("stopTime").doubleValue();
        packetName = par("packetName");
        isActive = false;
        wasActivated = false;
        transmitting = true;
        approaching = true;
        lastPosX = 9999;
        lastPosY = 9999;
        //if (stopTime >= SIMTIME_ZERO && stopTime < startTime)
          //  throw cRuntimeError("Invalid startTime/stopTime parameters");
        selfMsg = new cMessage("sendTimer");
        changedApproach=false;
    }
}

void InformRSU::finish()
{
    recordScalar("packets sent", numSent);
    recordScalar("packets received", numReceived);
    ApplicationBase::finish();
}

void InformRSU::setSocketOptions()
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

L3Address InformRSU::chooseDestAddr()
{
    int k = intrand(destAddresses.size());
    if (destAddresses[k].isLinkLocal()) {    // KLUDGE for IPv6
        const char *destAddrs = destAdd;
        cStringTokenizer tokenizer(destAddrs);
        const char *token = nullptr;

        for (int i = 0; i <= k; ++i)
            token = tokenizer.nextToken();
        destAddresses[k] = L3AddressResolver().resolve(token);
    }
    return destAddresses[k];
}

void InformRSU::sendPacket()
{
    if(transmitting == true)
    {
    cModule *host = getContainingNode(this);
    EV_INFO << "hostModuleName: " << host;
    IMobility  *mod = check_and_cast<IMobility *>(host->getSubmodule("mobility"));
    Coord pos = mod->getCurrentPosition();
    HaifaMobility *hMob = check_and_cast<HaifaMobility *>(mod);
    double distance = sqrt(pow((RSUPosX-pos.x),2)+pow((RSUPosY-pos.y),2));
    double lastDistance = sqrt(pow((RSUPosX-lastPosX),2)+pow((RSUPosY-lastPosY),2));
    const char *direction=getApproach(pos.x,pos.y);
    if(changedApproach==false)
    {
        defaultApproach=direction;
        changedApproach=true;
    }
    if(strcmp(defaultApproach,direction)==0)
    {
        approaching = true;
    }
    else
    {
        approaching = false;
    }
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
    payload->setApproaching(approaching);
    int idv = hMob->getVisID();
    payload->setVisID(idv);
    seq << getParentModule();
    se = seq.str();
    //EV_INFO << "PackSequence: " << se <<endl;
    payload->setSequence(se.c_str());
    L3Address destAddr = chooseDestAddr();
    emit(sentPkSignal, payload);
    socket.sendTo(payload, destAddr, destPort);
    //std::cout << "Sending Packet func (InformRSU::sendPacket) from Node: " << getParentModule() << " at t: " << simTime().dbl() << endl;
    numSent++;
    PacketLossEvaluator *ev = check_and_cast<PacketLossEvaluator *>(getModuleByPath("ScenarioTest.evaluator"));
    ev->addSentPk();
    lastPosX=pos.x;
    lastPosY=pos.y;
    }
}

// !!!!! WHEN TRANSITIONING TO THE NEW INTERSECTION MODEL, THIS BLOCK MUST BE CHANGED ACCORDINGLY !!!!!
const char *InformRSU::getApproach(double PosX, double PosY)
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

void InformRSU::processStart()
{
    socket.setOutputGate(gate("udpOut"));
    const char *localAddress = par("localAddress");
    socket.bind(*localAddress ? L3AddressResolver().resolve(localAddress) : L3Address(), localPort);
    setSocketOptions();
    const char *destAddrs = destAdd;
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

void InformRSU::processSend()
{
    sendPacket();
    simtime_t d = simTime() + par("sendInterval").doubleValue();
        selfMsg->setKind(SEND);
        scheduleAt(d, selfMsg);
}

void InformRSU::processStop()
{
    socket.close();
}

void InformRSU::handleMessageWhenUp(cMessage *msg)
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

void InformRSU::refreshDisplay() const
{
    char buf[100];
    sprintf(buf, "rcvd: %d pks\nsent: %d pks", numReceived, numSent);
    getDisplayString().setTagArg("t", 0, buf);
}

void InformRSU::processPacket(cPacket *pk)
{
    emit(rcvdPkSignal, pk);
    EV_INFO << "Received packet: " << UDPSocket::getReceivedPacketInfo(pk) << endl;
    delete pk;
    numReceived++;
}

void InformRSU::prijmiPaket(VeinsRSUPacket *pk)
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
    addToOutputForVissim(pk->getVisID(), pk->getPosX(), pk->getPosY());
    //std::cout << "Received packet vissimID: " << pk->getVisID() << endl;
    }
    delete pk;
    numReceived++;

}

void InformRSU::addToOutputForVissim(int vissimID, double posX, double posY)
{
    HaifaMobilityManager *hManager = check_and_cast<HaifaMobilityManager *>(getModuleByPath("^.^.mobilityManager"));
    hManager->addToVissimOutput(vissimID, posX, posY);
}

/*bool InformRSU::handleNodeStart(IDoneCallback *doneCallback)
{
    simtime_t start = std::max(startTime, simTime());
    if ((stopTime < SIMTIME_ZERO) || (start < stopTime) || (start == stopTime && startTime == stopTime)) {
        selfMsg->setKind(START);
        scheduleAt(start, selfMsg);
    }
    return true;
}
*/

void InformRSU::startTransmission()
{
    Enter_Method_Silent();
   // std::cout << "Func (InformRSU::startTransmission) for Node: " << getParentModule() << " accessed at t: " << simTime().dbl() << endl;
    if (isActive==false && wasActivated==false)
    {
    //std::cout << "Conditional statement in func (InformRSU::sendPacket) from Node: " << getParentModule() << " at t: " << simTime().dbl() << " is TRUE" << endl;
    selfMsg->setKind(START);
    scheduleAt(simTime()+uniform(0,0.01), selfMsg);
    isActive = true;
    wasActivated = true;
    }
    //std::cout << "Conditional statement in func (InformRSU::sendPacket) from Node: " << getParentModule() << " at t: " << simTime().dbl() << " is FALSE" << endl;
}

void InformRSU::stopTransmission()
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

bool InformRSU::handleNodeShutdown(IDoneCallback *doneCallback)
{
    if (selfMsg)
        cancelEvent(selfMsg);
    //TODO if(socket.isOpened()) socket.close();
    return true;
}

void InformRSU::handleNodeCrash()
{
    if (selfMsg)
        cancelEvent(selfMsg);
}

} // namespace inet

