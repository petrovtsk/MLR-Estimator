
#include "SimulationManager.h"
#include <fstream>
#include <sstream>
#include <algorithm>

namespace inet{


Define_Module(SimulationManager);

void SimulationManager::initialize()
{
    update = new cMessage;
    updateInterval = par("updateInterval");
    outputFileName = par("outputFileName");
    vehicleNamesFile = par("vehicleNamesFile");
    EVmoduleName = par("EVmoduleName");
    startTime = par("startTime");
    readVehicles(vehicleNamesFile);
    scheduleAt(startTime, update);
    std::ofstream ofs;
    ofs.open(outputFileName, std::ios::out | std::ios::trunc);
    ofs << "SimTime" << ";" << "EVpos" << ";" << "EVspeed" << ";" << "VehPos" << ";" << "VehSpeed" << ";" << "Distance;VehicleName" << endl;
    ofs.close();
}

void SimulationManager::handleMessage(cMessage *msg)
{
    if (msg->isSelfMessage())
    {
	getVehicleDistances();
        cMessage *updateMsg = new cMessage;
        scheduleAt(simTime()+updateInterval, updateMsg);
    }
    delete msg;
}

void SimulationManager::getVehicleDistances()
{
    std::stringstream outputString;
    std::stringstream evrtable;
    evrtable << "^." << EVmoduleName << ".routingTable";
    std::string EVRT = evrtable.str();
    cModule *EVT = getModuleByPath(EVRT.c_str());
    if(EVT)
    {
        IPv4RoutingTable *EVtable = check_and_cast<IPv4RoutingTable*>(EVT);
        std::cout << "SimTime is: " << simTime() << endl;
        EVtable->printRoutingTable();
    }


    for(auto&& x: vehiclesVector)
    {
        std::stringstream moname;
        moname << "^." << x << ".mobility";
        std::stringstream evname;
        evname << "^." << EVmoduleName << ".mobility";
        std::stringstream VIFtable;
        VIFtable << "^." << x << ".interfaceTable";
        std::stringstream IFsstr;
        IFsstr << "^." << x << ".interfaceTable.idToInterface";
        std::string vehIFtable = VIFtable.str();

        std::string moduleName = moname.str();
        std::string EVmodule = evname.str();
        std::string IFstr = IFsstr.str();
        EV_INFO << "mobility Module Name EV: " << EVmodule << endl;
        EV_INFO << "mobility Module Name veh: " << moduleName << endl;
        cModule *modVehMob = getModuleByPath(moduleName.c_str());
        cModule *modEVmob = getModuleByPath(EVmodule.c_str());
        if (modEVmob)
        {
            if (modVehMob)
            {
                IMobility *vehMob = check_and_cast<IMobility*>(modVehMob);
                IMobility *EVmob = check_and_cast<IMobility*>(modEVmob);
                inet::Coord curVehPos = vehMob->getCurrentPosition();
                inet::Coord curVehSpeed = vehMob->getCurrentSpeed();
                inet::Coord curEVpos = EVmob->getCurrentPosition();
                inet::Coord curEVspeed = EVmob->getCurrentSpeed();
                double distance = sqrt(pow((curVehPos.x-curEVpos.x),2)+pow((curVehPos.y-curEVpos.y),2));
                outputString << simTime() << ";" << curEVpos << ";" << curEVspeed << ";" << curVehPos << ";" << curVehSpeed << ";" << distance << ";" << x << endl;

                InterfaceTable *IFtable = check_and_cast<InterfaceTable*>(getModuleByPath(vehIFtable.c_str()));


                //std::string ifinfo = IFtable->info();
               // std::vector<InterfaceEntry*> ientries;

                InterfaceEntry *IFentry = IFtable->getInterface(1);
                std::string iEntry = IFentry->str();
                std::cout << "InterfaceTable info of node [" << x << "]: " << iEntry<< endl;

            }
            else
            {
                outputString << "Module " << moduleName << " is non existent in simulation." << endl;
            }
        }
        else
        {
            outputString << "Module " << EVmodule << " is non existent in simulation." << endl;
        }

    }
    writeOutputToFile(outputString.str());
}

void SimulationManager::writeOutputToFile(std::string outputStr)
{
    std::ofstream outfile;
    outfile.open(outputFileName, std::ios_base::app);
    outfile << outputStr;
    outfile.close();
}

void SimulationManager::readVehicles(std::string filename)
{
    using namespace std;
    ifstream ifile;
    ifile.open(filename);
    stringstream vehiclesbuf;
    vehiclesbuf << ifile.rdbuf();
    ifile.close();
    string vehicles = vehiclesbuf.str();
    string line;
    while (getline(vehiclesbuf, line))
    {
        vehiclesVector.push_back(line);
    }
}

}
