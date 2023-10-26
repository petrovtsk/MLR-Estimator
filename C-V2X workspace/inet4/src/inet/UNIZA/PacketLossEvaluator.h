#ifndef PacketLossEvaluator_h
#define PacketLossEvaluator_h

#undef INET_IMPORT
#include "inet/common/ModuleAccess.h"
#include "inet/common/geometry/common/Coord.h"
#include "inet/mobility/contract/IMobility.h"
#include "inet/networklayer/ipv4/Ipv4RoutingTable.h"
#include <inet/networklayer/common/InterfaceTable.h>
#include <string>
#include "omnetpp.h"
#include <vector>
#include <time.h>

namespace inet{

class PacketLossEvaluator : public cSimpleModule
{

  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    void writeOutputToFile(std::string outputStr);
    void calculateStatistics();

  public:
    double updateInterval;
    void addSentPk();
    void addRcvdPk();


  private:
    cMessage *update;
    const char *outputFileName;

    int sentPackets;
    int receivedPackets;
    int lostPackets;
    double packetLoss;


};

}
#endif


