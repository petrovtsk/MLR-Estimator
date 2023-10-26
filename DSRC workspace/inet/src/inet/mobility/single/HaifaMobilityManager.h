#ifndef HaifaMobilityManager_h
#define HaifaMobilityManager_h

#undef INET_IMPORT
#include "inet/common/ModuleAccess.h"
#include "inet/common/geometry/common/Coord.h"
#include "inet/mobility/contract/IMobility.h"
#include "inet/mobility/static/HaifaMobility.h"
#include "inet/applications/udpapp/InformRSU.h"
#include <string>
#include "omnetpp.h"
#include <vector>
#include "VehPos.h"
#include <time.h>
#include <numeric>
#include <iterator>

namespace inet{

class HaifaMobilityManager : public cSimpleModule
{

  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    void loadVehicleIds();
    std::string getFileName();
    void getPositions();
    std::string readFile(std::string fileName);
    void eraseHeaderFromFile(std::string oldFileName, std::string newFileName, size_t position);
    inet::Coord getVehicleCoordinatesFromFile(std::string vehID, std::string fileName, int simSecond);
    int getVehicleDetectionSecond(std::string vehID, std::string fileName, int simSecond);
    const char *getVehicleDirection(std::string vehID, std::string fileName, int simSecond);
    void updateVehiclePositions();
    void sleepcp(int milliseconds);
    void updateActiveVissimIds();
    void deleteOriginalFile();
    void assignIds(int visID, int omnetID);
    void assignStopVissimID(int omnetID);
    void copyFile(const char *SRC, const char* DEST);
    void setMessageTransmitting(bool value);

  public:
    int numVehicles; //number of vehicles from omnetpp.ini
    double updateInterval;
    int waitForVissim;
    inet::Coord returnVehiclePosition(int vissimVehicleID);
    void addToVissimOutput(int visID, double posX, double posY);
    void writeVissimOutputToFile();
    void addToLPI(int vID, double distanceToRSU); //START of MLR Estimation Algorithm
    void estimateMLR(); //calculate MLR according to the new Algorithm
    void addToLEVandLAS(int vID,bool approaching);
    void updateListOfLeftVehicles();
    std::list<int> getVehicleIdsFromFile(int second);
    void estimateMLRfromKnownVehicles();
    bool isNullOrWhitespace(const std::string& str);


  private:
    cMessage *update;
    std::vector<VehPos> vehicles;
    std::vector<VehPos> tempVehicles;
    std::vector<int> activeVissimIds;
    std::vector<int> previousActiveVissimIdsCopy;
    std::vector<int> vectorVissimOutput;
    std::vector<int> listOfLeftVehicles;
    std::string vissimOutput;
    const char *vissimFileName;
    const char *omnetFileName;
    const char *copiedCarsTxtPath;
    const char *finalOutputFileName;
    const char *simSecFileName;
    const char *pythonScriptFileName;
    int nextNode;
    int numReceivedTxtGenerated;
    int ignoreWest;
    int timeWindow;
    std::vector<VehPos> L_WEST; //list of vehicles that first time communicated with RSU
    std::vector<VehPos> L_EAST; //list of vehicles that first time communicated with RSU
    std::vector<VehPos> L_NORTH; //list of vehicles that first time communicated with RSU
    std::vector<VehPos> L_SOUTH; //list of vehicles that first time communicated with RSU
    std::vector<VehPos> allVehiclesInSimulation;
    //double C_WEST; //communication perimeter
    //double C_EAST; //communication perimeter
    //double C_NORTH; //communication perimeter
    //double C_SOUTH; //communication perimeter
    double averageSpeedWEST; //vehicle average speed
    double averageSpeedEAST; //vehicle average speed
    double averageSpeedNORTH; //vehicle average speed
    double averageSpeedSOUTH; //vehicle average speed
    double P_WEST; //observed perimeter
    double P_EAST; //observed perimeter
    double P_NORTH; //observed perimeter
    double P_SOUTH; //observed perimeter
    double NumUnknown_WEST; //number of unknown vehicles in (P-C) perimeter
    double NumUnknown_EAST; //number of unknown vehicles in (P-C) perimeter
    double NumUnknown_NORTH; //number of unknown vehicles in (P-C) perimeter
    double NumUnknown_SOUTH; //number of unknown vehicles in (P-C) perimeter

    std::vector<VehPos> LEV;
    std::vector<VehPos> LAS_WEST;
    std::vector<VehPos> LAS_EAST;
    std::vector<VehPos> LAS_NORTH;
    std::vector<VehPos> LAS_SOUTH;
    std::vector<VehPos> allVehiclesEverInSim;

};

//Class which stores vehicle�s Omnet ID, VISSIM ID and position loaded from a file
}
#endif // ifndef HaifaMobilityManager_h



