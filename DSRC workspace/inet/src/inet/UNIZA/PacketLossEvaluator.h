#ifndef PacketLossEvaluator_h
#define PacketLossEvaluator_h

#undef INET_IMPORT
#include "inet/common/ModuleAccess.h"
#include "inet/common/geometry/common/Coord.h"
#include "inet/mobility/contract/IMobility.h"
#include "inet/networklayer/ipv4/IPv4RoutingTable.h"
#include <inet/networklayer/common/InterfaceTable.h>
#include <inet/networklayer/common/InterfaceEntry.h>
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
    //void getVehicleDistances();
    void writeOutputToFile(std::string outputStr);
    //void readVehicles(std::string filename);
    void calculateStatistics();

  public:
    double updateInterval;
    void addSentPk();
    void addRcvdPk();


  private:
    cMessage *update;
    const char *outputFileName;
    //const char *vehicleNamesFile;
    //const char *EVmoduleName;
    //std::vector<std::string> vehiclesVector;
    //std::vector<IMobility> mobilities;
    //double startTime;
    int sentPackets;
    int receivedPackets;
    int lostPackets;
    double packetLoss;
    //std::stringstream outputStream;


};

//Class which stores vehicle�s Omnet ID, VISSIM ID and position loaded from a file
}
#endif // ifndef SimulationManager_h



