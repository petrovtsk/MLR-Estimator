//
// Copyright (C) 2000 Institut fuer Telematik, Universitaet Karlsruhe
// Copyright (C) 2004,2011 OpenSim Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later
//


#include "inet/applications/udpapp/RSUListen.h"

#include "inet/applications/base/VeinsRSUPacket_m.h"
#include "inet/common/ModuleAccess.h"
#include "inet/common/TagBase_m.h"
#include "inet/common/TimeTag_m.h"
#include "inet/mobility/contract/IMobility.h"
#include "inet/mobility/single/HaifaMobilityManager.h"
#include "inet/common/lifecycle/ModuleOperations.h"
#include "inet/common/packet/Packet.h"
#include "inet/networklayer/common/FragmentationTag_m.h"
#include "inet/networklayer/common/L3AddressResolver.h"
#include "inet/transportlayer/contract/udp/UdpControlInfo_m.h"
#include <stdio.h>
#include <iostream>
#include <fstream>

namespace inet {

Define_Module(RSUListen);

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
        packetName = par("packetName");
        dontFragment = par("dontFragment");
        posRSUx = par("posRSUx");
        posRSUy = par("posRSUy");

        isActive = false;
        wasActivated = false;
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

    int dscp = par("dscp");
    if (dscp != -1)
        socket.setDscp(dscp);

    int tos = par("tos");
    if (tos != -1)
        socket.setTos(tos);

    const char *multicastInterface = par("multicastInterface");
    if (multicastInterface[0]) {
        IInterfaceTable *ift = getModuleFromPar<IInterfaceTable>(par("interfaceTableModule"), this);
        InterfaceEntry *ie = ift->findInterfaceByName(multicastInterface);
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
    socket.setCallback(this);
}

L3Address RSUListen::chooseDestAddr()
{
    int k = intrand(destAddresses.size());
    if (destAddresses[k].isUnspecified() || destAddresses[k].isLinkLocal()) {
        L3AddressResolver().tryResolve(destAddressStr[k].c_str(), destAddresses[k]);
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
    str << packetName << "-" << numSent;
    Packet *packet = new Packet(str.str().c_str());
    if (dontFragment)
        packet->addTag<FragmentationReq>()->setDontFragment(true);
    const auto& payload = makeShared<VeinsRSUPacket>();
    payload->setChunkLength(B(par("messageLength")));
    payload->setSequenceNumber(numSent);
    payload->setCasOdoslania(simTime());
    payload->setPrikaz(100);
    payload->setPosX(pos.x);
    payload->setPosY(pos.y);
    int idv = hMob->getVisID();
    payload->setVisID(idv);
    packet->insertAtBack(payload);
    L3Address destAddr = chooseDestAddr();
    emit(packetSentSignal, packet);
    socket.sendTo(packet, destAddr, destPort);
    std::cout << "Paket bol odoslany v case: " << simTime() << " appkou RSUListen." << endl;
    numSent++;
}

void RSUListen::processStart()
{
    socket.setOutputGate(gate("socketOut"));
    const char *localAddress = par("localAddress");
    socket.bind(*localAddress ? L3AddressResolver().resolve(localAddress) : L3Address(), localPort);
    setSocketOptions();

    const char *destAddrs = par("destAddresses");
    cStringTokenizer tokenizer(destAddrs);
    const char *token;

    while ((token = tokenizer.nextToken()) != nullptr) {
        destAddressStr.push_back(token);
        L3Address result;
        L3AddressResolver().tryResolve(token, result);
        if (result.isUnspecified())
            EV_ERROR << "cannot resolve destination address: " << token << endl;
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
    simtime_t d = par("sendInterval");
    if (stopTime < SIMTIME_ZERO || simTime() + d < stopTime) {
        selfMsg->setKind(SEND);
        scheduleAt(d, selfMsg);
    }
    else {
        selfMsg->setKind(STOP);
        scheduleAt(stopTime, selfMsg);
    }
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
    else
        socket.processMessage(msg);
}

void RSUListen::socketDataArrived(UdpSocket *socket, Packet *packet)
{
    prijmiPaket(packet);
}

void RSUListen::socketErrorArrived(UdpSocket *socket, Indication *indication)
{
    EV_WARN << "Ignoring UDP error report " << indication->getName() << endl;
    delete indication;
}

void RSUListen::socketClosed(UdpSocket *socket)
{
    if (operationalState == State::STOPPING_OPERATION)
        startActiveOperationExtraTimeOrFinish(par("stopOperationExtraTime"));
}

void RSUListen::refreshDisplay() const
{
    ApplicationBase::refreshDisplay();

    char buf[100];
    sprintf(buf, "rcvd: %d pks\nsent: %d pks", numReceived, numSent);
    getDisplayString().setTagArg("t", 0, buf);
}

void RSUListen::processPacket(Packet *pk)
{
    emit(packetReceivedSignal, pk);
    EV_INFO << "Received packet: " << UdpSocket::getReceivedPacketInfo(pk) << endl;
    delete pk;
    numReceived++;
}

void RSUListen::prijmiPaket(Packet *packet)
{
    std::cout << "Paket bol prijaty v case: " << simTime() << " appkou RSUListen." << endl;
    cModule *hostRSU = getContainingNode(this);

    const auto& pk = packet->peekData<VeinsRSUPacket>();
    double distance = sqrt(pow((posRSUx-pk->getPosX()),2)+pow((posRSUy-pk->getPosY()),2));
    
    emit(packetReceivedSignal, packet);
    
    EV_INFO << "Received packet: " << UdpSocket::getReceivedPacketInfo(packet) << endl;
    if (pk->getPrikaz() == 100)
    {
    latency = ((simTime() - pk->getCasOdoslania())*1000);
    emit(E2Edelay, latency);
    EV_INFO << "PackSequence: " << pk->getSequence() << ";" << pk->getCasOdoslania() << ";"<< std::endl;
    EV_INFO  << "E2ELatency: " << latency.dbl() << "\n";
    if(pk->getCasOdoslania() > int(simTime().dbl()))
    {
    addToOutputForVissim(pk->getVisID(), pk->getPosX(), pk->getPosY());
    addToManagerLPI(pk->getVisID(),distance);
    addToManagerLEVandLAS(pk->getVisID(),pk->getApproaching());
    emit(rcvdPkByVissimSignal, packet);
    numReceivedByVissim++;
    std::ofstream receivedPKVis;
    receivedPKVis.open("receivedPKByVISSIM.csv", std::ios_base::app);
    receivedPKVis << pk->getVisID() << ";" << int(simTime().dbl()) << ";" << latency.dbl() << ";" << numReceivedByVissim << endl;
    receivedPKVis.close();
    }
    }
    

    delete packet;
    numReceived++;
    PacketLossEvaluator *ev = check_and_cast<PacketLossEvaluator *>(getModuleByPath("ScenarioHaifa.evaluator"));
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

void RSUListen::handleStartOperation(LifecycleOperation *operation)
{

}

void RSUListen::handleStopOperation(LifecycleOperation *operation)
{

}

void RSUListen::handleCrashOperation(LifecycleOperation *operation)
{

}

}

