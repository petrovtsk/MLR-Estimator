//
// Copyright (C) 2000 Institut fuer Telematik, Universitaet Karlsruhe
// Copyright (C) 2004,2011 OpenSim Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later
//


#ifndef __INET_INFORMRSU_H
#define __INET_INFORMRSU_H

#include <vector>

#include "inet/common/INETDefs.h"

#include "inet/applications/base/ApplicationBase.h"
#include "inet/transportlayer/contract/udp/UdpSocket.h"
#include "inet/applications/base/VeinsRSUPacket_m.h"
#include "inet/UNIZA/PacketLossEvaluator.h"
#include <iostream>
#include <cmath>
#include <string>

namespace inet {

class INET_API InformRSU : public ApplicationBase, public UdpSocket::ICallback
{
  protected:
    enum SelfMsgKinds { START = 1, SEND, STOP };
    std::vector<L3Address> destAddresses;
    std::vector<std::string> destAddressStr;
    int localPort = -1, destPort = -1;
    simtime_t startTime;
    simtime_t stopTime;
    bool dontFragment = false;
    simtime_t latency;
    std::string vissimOutput;
	const char *indexFileName = nullptr;
    const char *packetName = nullptr;

    UdpSocket socket;
    cMessage *selfMsg = nullptr;

    int numSent = 0;
    int numReceived = 0;
    cOutVector E2Elatency;
    static simsignal_t E2Edelay;

  protected:
    virtual int numInitStages() const override { return NUM_INIT_STAGES; }
    virtual void initialize(int stage) override;
    virtual void handleMessageWhenUp(cMessage *msg) override;
    virtual void finish() override;
    virtual void refreshDisplay() const override;

    virtual L3Address chooseDestAddr();
    virtual void sendPacket();
    virtual void processPacket(Packet *msg);
    virtual void setSocketOptions();
    void addToOutputForVissim(int vissimID, double posX, double posY);
    const char *getApproach(double PosX, double PosY);

    virtual void processStart();
    virtual void processSend();
    virtual void processStop();

    virtual void handleStartOperation(LifecycleOperation *operation) override;
    virtual void handleStopOperation(LifecycleOperation *operation) override;
    virtual void handleCrashOperation(LifecycleOperation *operation) override;

    virtual void socketDataArrived(UdpSocket *socket, Packet *packet) override;
    virtual void socketErrorArrived(UdpSocket *socket, Indication *indication) override;
    virtual void socketClosed(UdpSocket *socket) override;

  public:
    InformRSU() {}
    ~InformRSU();
    void startTransmission();
    void stopTransmission();
    void prijmiPaket(Packet *packet);
    bool isActive;
    bool wasActivated;
    bool transmitting;
    const char *destAdd;
    const char *defaultApproach;
    double lastPosX,lastPosY;
    bool approaching;
    double RSUPosX, RSUPosY;
    bool changedApproach;
};

}

#endif

