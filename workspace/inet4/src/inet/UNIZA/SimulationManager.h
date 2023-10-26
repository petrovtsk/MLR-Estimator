#ifndef SimulationManager_h
#define SimulationManager_h

#undef INET_IMPORT
#include "inet/common/ModuleAccess.h"
#include "inet/common/geometry/common/Coord.h"
#include "inet/mobility/contract/IMobility.h"
#include "inet/networklayer/ipv4/Ipv4RoutingTable.h"
#include <inet/networklayer/common/InterfaceTable.h>
#include <inet/networklayer/common/InterfaceEntry.h>
#include <string>
#include "omnetpp.h"
#include <vector>
#include <time.h>

namespace inet{

class SimulationManager : public cSimpleModule
{

  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    void getVehicleDistances();
    void writeOutputToFile(std::string outputStr);
    void readVehicles(std::string filename);

  public:
    double updateInterval;


  private:
    cMessage *update;
    const char *outputFileName;
    const char *vehicleNamesFile;
    const char *EVmoduleName;
    std::vector<std::string> vehiclesVector;
    std::vector<IMobility> mobilities;
    double startTime;


};

}
#endif



