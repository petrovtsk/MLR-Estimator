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

#ifndef __INET_INFORMRSU_H
#define __INET_INFORMRSU_H

#include <vector>

#include "inet/common/INETDefs.h"

#include "inet/applications/base/ApplicationBase.h"
#include "inet/transportlayer/contract/udp/UDPSocket.h"
#include "inet/applications/base/VeinsRSUPacket_m.h"
#include "inet/UNIZA/PacketLossEvaluator.h"
#include <iostream>
#include <cmath>
#include <string>

namespace inet {

/**
 * UDP application. See NED for more info.
 */
class INET_API InformRSU : public ApplicationBase
{
  protected:
    enum SelfMsgKinds { START = 1, SEND, STOP };

    // parameters
    std::vector<L3Address> destAddresses;
    int localPort = -1, destPort = -1;
    //simtime_t startTime;
    //simtime_t stopTime;
    simtime_t latency;
    std::string vissimOutput;
	const char *indexFileName = nullptr;

    const char *packetName = nullptr;

    // state
    UDPSocket socket;
    cMessage *selfMsg = nullptr;

    // statistics
    int numSent = 0;
    int numReceived = 0;
    cOutVector E2Elatency;

    static simsignal_t sentPkSignal;
    static simsignal_t rcvdPkSignal;
    static simsignal_t E2Edelay;

  protected:
    virtual int numInitStages() const override { return NUM_INIT_STAGES; }
    virtual void initialize(int stage) override;
    virtual void handleMessageWhenUp(cMessage *msg) override;
    virtual void finish() override;
    virtual void refreshDisplay() const override;

    // chooses random destination address
    virtual L3Address chooseDestAddr();
    virtual void sendPacket();
    virtual void processPacket(cPacket *msg);
    virtual void setSocketOptions();
    virtual void prijmiPaket(VeinsRSUPacket *pk);
    void addToOutputForVissim(int vissimID, double posX, double posY);
    const char *getApproach(double PosX, double PosY);

   // virtual bool handleNodeStart(IDoneCallback *doneCallback) override;
    virtual bool handleNodeShutdown(IDoneCallback *doneCallback) override;
    virtual void handleNodeCrash() override;

  public:
    InformRSU() {}
   // E2Elatency.setName("E2ELatency [ms]");
    ~InformRSU();
    virtual void processStop();
    virtual void processStart();
    virtual void processSend();
    void startTransmission();
    void stopTransmission();
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

} // namespace inet

#endif // ifndef __INET_INFORMRSU_H

