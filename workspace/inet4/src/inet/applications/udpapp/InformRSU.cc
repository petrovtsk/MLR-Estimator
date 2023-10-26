//
// Copyright (C) 2000 Institut fuer Telematik, Universitaet Karlsruhe
// Copyright (C) 2004,2011 OpenSim Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later
//


#include "inet/applications/udpapp/InformRSU.h"

#include "inet/applications/base/VeinsRSUPacket_m.h"
#include "inet/applications/base/ApplicationPacket_m.h"
#include "inet/applications/base/VeinsRSUPacketPacket_m.h"
#include "inet/common/ModuleAccess.h"
#include "inet/common/TagBase_m.h"
#include "inet/common/TimeTag_m.h"
#include "inet/common/lifecycle/ModuleOperations.h"
#include "inet/common/packet/Packet.h"
#include "inet/networklayer/common/FragmentationTag_m.h"
#include "inet/networklayer/common/L3AddressResolver.h"
#include "inet/transportlayer/contract/udp/UdpControlInfo_m.h"
#include "inet/mobility/single/HaifaMobilityManager.h"
#include <stdio.h>

namespace inet {

Define_Module(InformRSU);

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
        destAdd = par("destAddresses");

        localPort = par("localPort");
        destPort = par("destPort");
        packetName = par("packetName");
        dontFragment = par("dontFragment");
        isActive = false;
        wasActivated = false;
        transmitting = true;
        approaching = true;
        lastPosX = 9999;
        lastPosY = 9999;
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

    int dscp = par("dscp");
    if (dscp != -1)
        socket.setDscp(dscp);

    int tos = par("tos");
    if (tos != -1)
        socket.setTos(tos);
    socket.setCallback(this);
}

L3Address InformRSU::chooseDestAddr()
{
    int k = intrand(destAddresses.size());
    if (destAddresses[k].isUnspecified() || destAddresses[k].isLinkLocal()) {
        L3AddressResolver().tryResolve(destAddressStr[k].c_str(), destAddresses[k]);
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
    Packet *packet = new Packet(str.str().c_str());
    if (dontFragment)
        packet->addTag<FragmentationReq>()->setDontFragment(true);
    const auto& payload = makeShared<VeinsRSUPacket>();
    payload->setChunkLength(B(par("messageLength")));
    payload->setSequenceNumber(numSent);

    payload->addTag<CreationTimeTag>()->setCreationTime(simTime());

    payload->setCasOdoslania(simTime());
    payload->setPrikaz(100);
    payload->setPosX(pos.x);
    payload->setPosY(pos.y);
    payload->setApproaching(approaching);
    int idv = hMob->getVisID();
    payload->setVisID(idv);

    seq << getParentModule();
    se = seq.str();
    packet->insertAtBack(payload);
    L3Address destAddr = chooseDestAddr();
    emit(packetSentSignal, packet);
    socket.sendTo(packet, destAddr, destPort);
    numSent++;
    std::cout << "InformRSU:Paket bol poslany v case: " << simTime() << endl;
    std::cout << "InformRSU:Dlzka paketu je: " << packet->getByteLength() << endl;
    std::cout << "InformRSU:PacketName: " << packet->getFullName() << endl;
    PacketLossEvaluator *ev = check_and_cast<PacketLossEvaluator *>(getModuleByPath("ScenarioHaifa.evaluator"));
    ev->addSentPk();
    lastPosX=pos.x;
    lastPosY=pos.y;
    }
}

const char *InformRSU::getApproach(double PosX, double PosY)
{
    if(PosX <= 165 && PosY <= 15 && PosY >= -20)
                      {
                           return "w";
                      }
               if(PosX > 180 && PosY <= 30 && PosY >= -10)
                        {
                           return "e";
                        }
               if(PosY >= 10 && PosX <= 180 && PosX >= 150)
                           {
                               return "n";
                           }
               if(PosY < -10 && PosX <= 190 && PosX >= 160)
                               {
                                   return "s";
                               }
               return "";
}

void InformRSU::processStart()
{
    socket.setOutputGate(gate("socketOut"));
    const char *localAddress = par("localAddress");
    socket.bind(*localAddress ? L3AddressResolver().resolve(localAddress) : L3Address(), localPort);
    setSocketOptions();

    const char *destAddrs = destAdd;
    cStringTokenizer tokenizer(destAddrs);
    const char *token;

    while ((token = tokenizer.nextToken()) != nullptr) {
        destAddressStr.push_back(token);
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
    simtime_t d = simTime() + par("sendInterval");
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
        prijmiPaket(check_and_cast<Packet *>(msg));
    }
    else
        socket.processMessage(msg);
}

void InformRSU::socketDataArrived(UdpSocket *socket, Packet *packet)
{
    processPacket(packet);
    prijmiPaket(packet);
}

void InformRSU::socketErrorArrived(UdpSocket *socket, Indication *indication)
{
    EV_WARN << "Ignoring UDP error report " << indication->getName() << endl;
    delete indication;
}

void InformRSU::socketClosed(UdpSocket *socket)
{
    if (operationalState == State::STOPPING_OPERATION)
        startActiveOperationExtraTimeOrFinish(par("stopOperationExtraTime"));
}

void InformRSU::refreshDisplay() const
{
    ApplicationBase::refreshDisplay();

    char buf[100];
    sprintf(buf, "rcvd: %d pks\nsent: %d pks", numReceived, numSent);
    getDisplayString().setTagArg("t", 0, buf);
}

void InformRSU::processPacket(Packet *pk)
{
    emit(packetReceivedSignal, pk);
    EV_INFO << "Received packet: " << UdpSocket::getReceivedPacketInfo(pk) << endl;
    delete pk;
    numReceived++;
}

void InformRSU::prijmiPaket(Packet *packet)
{
    std::cout << "Paket bol prijaty v case: " << simTime() << " appkou InformRSU." << endl;
    const auto& pk = packet->peekData<VeinsRSUPacket>();
    cModule *hostRSU = getContainingNode(this);
    EV_INFO << "RSUHostModuleName: " << hostRSU;
    IMobility  *modRSU = check_and_cast<IMobility *>(hostRSU->getSubmodule("mobility"));
    Coord posRSU = modRSU->getCurrentPosition();
    double distance = sqrt(pow((posRSU.x-pk->getPosX()),2)+pow((posRSU.y-pk->getPosY()),2));
    emit(packetReceivedSignal, packet);
    EV_INFO << "Received packet: " << UdpSocket::getReceivedPacketInfo(packet) << endl;
    if (pk->getPrikaz() == 100)
    {
    latency = ((simTime() - pk->getCasOdoslania())*1000);
    emit(E2Edelay, latency);
    EV_INFO << "PackSequence: " << pk->getSequence() << ";" << pk->getCasOdoslania() << ";"<< std::endl;
    EV_INFO  << "E2ELatency: " << latency.dbl() << "\n";
    addToOutputForVissim(pk->getVisID(), pk->getPosX(), pk->getPosY());
    }
    delete packet;
    numReceived++;
}

void InformRSU::addToOutputForVissim(int vissimID, double posX, double posY)
{
    HaifaMobilityManager *hManager = check_and_cast<HaifaMobilityManager *>(getModuleByPath("^.^.mobilityManager"));
    hManager->addToVissimOutput(vissimID, posX, posY);
}

void InformRSU::startTransmission()
{
    Enter_Method_Silent();
    if (isActive==false && wasActivated==false)
    {
    selfMsg->setKind(START);
    scheduleAt(simTime()+uniform(0.001,0.01), selfMsg);
    isActive = true;
    wasActivated = true;
    }
}

void InformRSU::handleStartOperation(LifecycleOperation *operation)
{

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

void InformRSU::handleStopOperation(LifecycleOperation *operation)
{
    cancelEvent(selfMsg);
    socket.close();
    delayActiveOperationFinish(par("stopOperationTimeout"));
}

void InformRSU::handleCrashOperation(LifecycleOperation *operation)
{
    cancelEvent(selfMsg);
    socket.destroy();
}

} // namespace inet

