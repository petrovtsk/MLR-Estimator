
#include "HaifaMobilityManager.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <chrono>
#include <ctime>


namespace inet{


Define_Module(HaifaMobilityManager);

void HaifaMobilityManager::initialize()
{
    vissimOutput = "";
    update = new cMessage;
    update->setKind(1555);
    numVehicles = par("maxVehicles");
    updateInterval = par("updateInterval");
    waitForVissim = par("waitForVissim");
    vissimFileName = par("vissimFilePath");
    simSecFileName = par("simSecFilePath");
    pythonScriptFileName = par("pythonScriptFilePath");
    omnetFileName = par("temporaryOmnetOutputFilePath");
    finalOutputFileName = par("omnetOutputFilePath");
    copiedCarsTxtPath = par("copiedCarsTxtPath");
    ignoreWest = par("ignoreWest");
    nextNode = 0;
    numReceivedTxtGenerated = 1;

    timeWindow = par("timeWindow");
    averageSpeedWEST = par("averageSpeedWEST");
    averageSpeedEAST = par("averageSpeedEAST");
    averageSpeedNORTH = par("averageSpeedNORTH");
    averageSpeedSOUTH = par("averageSpeedSOUTH");
    P_WEST = par("P_W");
    P_EAST = par("P_E");
    P_NORTH = par("P_N");
    P_SOUTH = par("P_S");

    std::cout << "All set! Omnet will wait for VISSIM for " << waitForVissim/1000 << " sec." << endl;

    scheduleAt(updateInterval, update);
    cMessage *visOutput = new cMessage;
    visOutput->setKind(1666);
    scheduleAt(1.999, visOutput);

    cMessage *MLRAlgorithm = new cMessage;
    MLRAlgorithm->setKind(1999);
    scheduleAt(1.9999, MLRAlgorithm);

    std::ofstream assignedVehs;
    assignedVehs.open("assignedVehs.csv", std::ios::out | std::ios::trunc);
    assignedVehs << "OmnetModule;VissimID;SimSecond" << endl;
    assignedVehs.close();

    std::ofstream EstimatedMLR;
    EstimatedMLR.open("EstimatedMLR.csv", std::ios::out | std::ios::trunc);
    EstimatedMLR << "SimSec;EstMLR_W;EstMLR_E;EstMLR_N;EstMLR_S" << endl;
    EstimatedMLR.close();

    std::ofstream EstTotLost;
    EstTotLost.open("EstTotLost.csv", std::ios::out | std::ios::trunc);
    EstTotLost << "SimSec;EstTotLost_WEST;EstTotLost_EAST;EstTotLost_NORTH;EstTotLost_SOUTH" << endl;
    EstTotLost.close();
}

void HaifaMobilityManager::addToVissimOutput(int visID, double posX, double posY)
{
    if(ignoreWest == 1)
    {
        if(posX <= 165 && posY <= 15 && posY >= -20)
        {
        }

        else
        {
    if(visID != 9999)
    {
        if ( std::find(vectorVissimOutput.begin(), vectorVissimOutput.end(), visID) != vectorVissimOutput.end() )
        {

        }
        else
        {
            vectorVissimOutput.push_back(visID);
            vissimOutput.append(std::to_string(visID));
            vissimOutput.append("\n");
        }
    }
    }
    }

    if(ignoreWest == 0)
    {
        if(visID != 9999)
        {
            if ( std::find(vectorVissimOutput.begin(), vectorVissimOutput.end(), visID) != vectorVissimOutput.end() )
            {

            }
            else
            {
                vectorVissimOutput.push_back(visID);
                vissimOutput.append(std::to_string(visID));
                vissimOutput.append("\n");
            }
        }
    }
}

void HaifaMobilityManager::writeVissimOutputToFile()
{
    std::ofstream oFile;
    std::stringstream oFileName;
    std::ofstream backupFile;
    std::stringstream backupFileName;
    oFileName << omnetFileName;
    auto writeTime = std::chrono::system_clock::now();
    std::time_t currentWriteTime = std::chrono::system_clock::to_time_t(writeTime);
    std::cout << "Writing Received.txt file at: " << std::ctime(&currentWriteTime) << "; current Omnet SimSecond is: " << int(simTime().dbl()) << endl;
    oFile.open(oFileName.str());
    backupFileName << "tmpFiles/" << int(simTime().dbl()) << ".rcvd";
    backupFile.open(backupFileName.str());

    if(! vissimOutput.empty())
    {
        vissimOutput.pop_back();
        oFile << vissimOutput;
        oFile << endl << "SimSecond=" << int(simTime().dbl());
        oFile << endl << "Received.txt#: " << numReceivedTxtGenerated;

        backupFile << vissimOutput;
        backupFile << endl << "SimSecond=" << int(simTime().dbl());
        backupFile << endl << "Received.txt#: " << numReceivedTxtGenerated;

    }
    else
    {
        oFile << "empty";
        oFile << endl << "SimSecond=" << int(simTime().dbl());
        oFile << endl << "Received.txt#: " << numReceivedTxtGenerated;

        backupFile << "empty";
        backupFile << endl << "SimSecond=" << int(simTime().dbl());
        backupFile << endl << "Received.txt#: " << numReceivedTxtGenerated;

    }
    oFile.close();
    numReceivedTxtGenerated++;
    backupFile.close();
    vectorVissimOutput.clear();
    copyFile(omnetFileName, finalOutputFileName);
}

void HaifaMobilityManager::handleMessage(cMessage *msg)
{
    if (msg->isSelfMessage())
    {
        if (msg->getKind()==1555)
        {
            EV_INFO << "Message Kind: " << msg->getKind() << endl;
            getPositions();
            cMessage *updateMsg = new cMessage;
            updateMsg->setKind(1555);
            scheduleAt(simTime()+updateInterval, updateMsg);
        }

        if (msg->getKind()==1666)
        {
            EV_INFO << "Message Kind: " << msg->getKind() << endl;
            writeVissimOutputToFile();
            updateListOfLeftVehicles();
            estimateMLRfromKnownVehicles();
            vissimOutput = "";
            cMessage *visOut = new cMessage;
            visOut->setKind(1666);
            scheduleAt(simTime()+updateInterval,visOut);
        }

        if (msg->getKind()==1999)
        {
            estimateMLR();
            cMessage *MLRmessage = new cMessage;
            MLRmessage->setKind(1999);
            scheduleAt(simTime()+timeWindow,MLRmessage);
        }
    }
    delete msg;
}

std::string HaifaMobilityManager::getFileName()
{
    std::stringstream fileName;
    std::string baseFileName = "";
    fileName << baseFileName << int(simTime().dbl()) << ".fzp";
    return fileName.str();
}

void HaifaMobilityManager::sleepcp(int milliseconds)
{
    clock_t time_end;
    time_end = clock() + milliseconds * CLOCKS_PER_SEC/1000;
    while (clock() < time_end)
    {
    }
}

void HaifaMobilityManager::getPositions()
{
   bool opened = false;
    while (!opened)
    {
        sleepcp(100);
        std::ifstream fin(vissimFileName);
        if(fin.is_open())
        {
           fin.close();
           opened = true;
        }
    }
    std::ofstream simSecFile;
    simSecFile.open(simSecFileName);
    simSecFile << int(simTime().dbl());
    simSecFile.close();
    std::stringstream command;
    copyFile(vissimFileName, copiedCarsTxtPath);
    auto readTime = std::chrono::system_clock::now();
    std::time_t currentReadTime = std::chrono::system_clock::to_time_t(readTime);
    std::cout << "Reading Cars.txt file at: " << std::ctime(&currentReadTime) << "; current Omnet SimSecond is: " << int(simTime().dbl()) << endl;
    updateActiveVissimIds();
    std::string file = readFile(copiedCarsTxtPath);
    file.pop_back();
    size_t pos = file.find("$VEHICLE:SIMSEC;");
    std::stringstream erasedFile;
    erasedFile << int(simTime().dbl()) << ".simsec";
    eraseHeaderFromFile(copiedCarsTxtPath, erasedFile.str(), pos+67);
    deleteOriginalFile();
    for (auto& vehicle : vehicles)
        {
        std::cout << "Currently obtaining position for vehicle: " << vehicle.vehID << endl;
        inet::Coord pos = getVehicleCoordinatesFromFile(std::to_string(vehicle.vehID),erasedFile.str(),int(simTime().dbl()));
           vehicle.set_Position(pos);
           std::cout << "Currently obtaining detection second for vehicle: " << vehicle.vehID << endl;
           int detectionSecond = getVehicleDetectionSecond(std::to_string(vehicle.vehID),erasedFile.str(),int(simTime().dbl()));
           vehicle.firstDetectionSecond = detectionSecond;
           std::cout << "Vehicle " << vehicle.vehID << " has been first detected in simsec " << vehicle.firstDetectionSecond << endl;
           if (vehicle.assignedDirection == false)
           {
           const char *direct = getVehicleDirection(std::to_string(vehicle.vehID),erasedFile.str(),int(simTime().dbl()));
           if(strcmp(direct,"w")==0)
                  {
                       vehicle.direction = "w";

                       const auto& tmp = vehicle.vehID;
                           std::vector<VehPos>::iterator it;
                           it = std::find_if(allVehiclesEverInSim.begin(), allVehiclesEverInSim.end(), [&tmp](VehPos const& obj){
                           return obj.vehID == tmp;
                           } );
                           if(it != allVehiclesEverInSim.end())
                           {
                           it->firstDetectionSecond = detectionSecond;
                           it->direction = "w";
                           }
                           else
                           {
                           std::cout << "Vehicle " << vehicle.vehID << " was not found in file allVehicleEverInSim vector" << endl;
                           }

                       std::stringstream nodeName;
                       nodeName << "^.node[" << vehicle.omnetID << "]";
                       InformRSU *application = check_and_cast<InformRSU*>(getModuleByPath(nodeName.str().c_str())->getSubmodule("app", 0));
                       application->destAdd = "ScenarioHaifa.rsu_west";
                       std::cout << "Vehicle " << nodeName.str() << " was assigned to " << vehicle.direction << endl;
                  }
           if(strcmp(direct,"e")==0)
                    {
                         vehicle.direction = "e";

                                                const auto& tmp = vehicle.vehID;
                                                    std::vector<VehPos>::iterator it;
                                                    it = std::find_if(allVehiclesEverInSim.begin(), allVehiclesEverInSim.end(), [&tmp](VehPos const& obj){
                                                    return obj.vehID == tmp;
                                                    } );
                                                    if(it != allVehiclesEverInSim.end())
                                                    {
                                                    it->firstDetectionSecond = detectionSecond;
                                                    it->direction = "e";
                                                    }
                                                    else
                                                    {
                                                    std::cout << "Vehicle " << vehicle.vehID << " was not found in file allVehicleEverInSim vector" << endl;
                                                    }

                         std::stringstream nodeName;
                         nodeName << "^.node[" << vehicle.omnetID << "]";
                         InformRSU *application = check_and_cast<InformRSU*>(getModuleByPath(nodeName.str().c_str())->getSubmodule("app", 0));
                          application->destAdd = "ScenarioHaifa.rsu_east";
                          std::cout << "Vehicle " << nodeName.str() << " was assigned to " << vehicle.direction << endl;
                    }
           if(strcmp(direct,"n")==0)
                       {
                            vehicle.direction = "n";

                            const auto& tmp = vehicle.vehID;
                                                       std::vector<VehPos>::iterator it;
                                                       it = std::find_if(allVehiclesEverInSim.begin(), allVehiclesEverInSim.end(), [&tmp](VehPos const& obj){
                                                       return obj.vehID == tmp;
                                                       } );
                                                       if(it != allVehiclesEverInSim.end())
                                                       {
                                                       it->firstDetectionSecond = detectionSecond;
                                                       it->direction = "n";
                                                       }
                                                       else
                                                       {
                                                       std::cout << "Vehicle " << vehicle.vehID << " was not found in file allVehicleEverInSim vector" << endl;
                                                       }

                            std::stringstream nodeName;
                            nodeName << "^.node[" << vehicle.omnetID << "]";
                            InformRSU *application = check_and_cast<InformRSU*>(getModuleByPath(nodeName.str().c_str())->getSubmodule("app", 0));
                            application->destAdd = "ScenarioHaifa.rsu_north";
                            std::cout << "Vehicle " << nodeName.str() << " was assigned to " << vehicle.direction << endl;
                       }
           if(strcmp(direct,"s")==0)
                           {
                                vehicle.direction = "s";

                                const auto& tmp = vehicle.vehID;
                                                           std::vector<VehPos>::iterator it;
                                                           it = std::find_if(allVehiclesEverInSim.begin(), allVehiclesEverInSim.end(), [&tmp](VehPos const& obj){
                                                           return obj.vehID == tmp;
                                                           } );
                                                           if(it != allVehiclesEverInSim.end())
                                                           {
                                                           it->firstDetectionSecond = detectionSecond;
                                                           it->direction = "s";
                                                           }
                                                           else
                                                           {
                                                           std::cout << "Vehicle " << vehicle.vehID << " was not found in file allVehicleEverInSim vector" << endl;
                                                           }

                                std::stringstream nodeName;
                                nodeName << "^.node[" << vehicle.omnetID << "]";
                                InformRSU *application = check_and_cast<InformRSU*>(getModuleByPath(nodeName.str().c_str())->getSubmodule("app", 0));
                                application->destAdd = "ScenarioHaifa.rsu_south";
                                std::cout << "Vehicle " << nodeName.str() << " was assigned to " << vehicle.direction << endl;
                           }
           vehicle.assignedDirection = true;
           }
        }

}

void HaifaMobilityManager::setMessageTransmitting(bool value)
{
    for (auto& vehicle : vehicles)
        {
        std::stringstream nodeName;
        nodeName << "^.node[" << vehicle.omnetID << "]";
        InformRSU *application = check_and_cast<InformRSU*>(getModuleByPath(nodeName.str().c_str())->getSubmodule("app", 0));
        application->transmitting = value;
        }
    if(value == false)
    std::cout << "No transmissions are now made by all vehicles." << endl;
}

void HaifaMobilityManager::deleteOriginalFile()
{
    if( remove( vissimFileName ) != 0 )
        std::cout <<  "Error deleting file" << endl;
      else
      {
          auto deleteTime = std::chrono::system_clock::now();
          std::time_t currentDeleteTime = std::chrono::system_clock::to_time_t(deleteTime);
          std::cout << "Deleting Cars.txt file at: " << std::ctime(&currentDeleteTime) << "; current Omnet SimSecond is: " << int(simTime().dbl()) << endl;
      }
        std::cout <<  "File successfully deleted" << endl;
}


inet::Coord HaifaMobilityManager::returnVehiclePosition(int vissimVehicleID)
{
    const auto& tmp = vissimVehicleID;
    std::vector<VehPos>::iterator it;
    it = std::find_if(vehicles.begin(), vehicles.end(), [&tmp](VehPos const& obj){
    return obj.vehID == tmp;
    } );
    if(it != vehicles.end())
    return it->position;
    else
    {
    return Coord::NIL;
    EV_INFO << "Vehicle " << vissimVehicleID << " was not found in file " << getFileName() << endl;
    }
}


void HaifaMobilityManager::copyFile(const char *SRC, const char* DEST)
{
    std::ifstream src(SRC, std::ios::binary);
    std::ofstream dest(DEST, std::ios::binary);
    dest << src.rdbuf();
}

std::string HaifaMobilityManager::readFile(std::string fileName)
{
    bool opened = false;
        while (!opened)
        {
            sleepcp(10);
            std::ifstream fin(copiedCarsTxtPath);
            if(fin.is_open())
            {
               fin.close();
               opened = true;
            }
        }
    std::ifstream mobFile;
    mobFile.open(fileName);
    auto readTime = std::chrono::system_clock::now();
    std::string file;
    std::stringstream buffer;
    buffer << mobFile.rdbuf();
    mobFile.close();
    file = buffer.str();
    return file;
}

void HaifaMobilityManager::eraseHeaderFromFile(std::string oldFileName, std::string newFileName, size_t position)
{
    std::ofstream mobFile;
    mobFile.open(newFileName);
    std::string file = readFile(oldFileName);
    file.erase(0, position);
    file.pop_back();
    mobFile << file;
    mobFile.close();
}

bool HaifaMobilityManager::isNullOrWhitespace(const std::string& str) {
    return str.empty()
        || std::all_of(str.begin(), str.end(), [](char c) {
        return std::isspace(static_cast<unsigned char>(c));
    });
}

void HaifaMobilityManager::updateActiveVissimIds()
{
    std::ifstream inputFile;
    inputFile.open(copiedCarsTxtPath);
    std::stringstream inputContentBuffer;
    inputContentBuffer << inputFile.rdbuf();
    inputFile.close();
    std::string inputContent = inputContentBuffer.str();
    std::string strippedContent;
    strippedContent = inputContent.substr(inputContent.find("$VEHICLE:SIMSEC")+67);
    std::stringstream strippedBuffer;
    strippedBuffer << strippedContent;
    std::stringstream stripBuff;
    stripBuff << strippedContent;
    std::string line = "";
    std::string fullFile = strippedBuffer.str();
    std::size_t found;
    std::cout << fullFile << ";" << endl;
    found = fullFile.find("9999");

    if (found!=std::string::npos)
    {
        setMessageTransmitting(false);
        activeVissimIds = previousActiveVissimIdsCopy;
    }
    else
    {
        setMessageTransmitting(true);
    }

    while (std::getline(strippedBuffer, line))
    {
        std::string outS1;
        std::cout << "line: " << line << endl;
        outS1 = line.substr(line.find(';') + 1);
        std::cout << "outS1: " << outS1 << endl;
        std::string currentString = outS1.substr(0, outS1.find(';'));
        std::cout << "currentString: " << currentString << endl;
        activeVissimIds.push_back(stoi(currentString));
        std::cout << "Adding vehicle with ID: " << currentString << " to activeVehicleVissimIds" << endl;


        std::stringstream test(line);
        std::string segment;
        std::vector<std::string> seglist;

        while(std::getline(test, segment, ';'))
        {
           seglist.push_back(segment);
        }

        auto strit = std::find(seglist.begin(), seglist.end(), "BUS");
        if(strit!=seglist.end()){
            addToVissimOutput(stoi(currentString), 0, 0);
        }
    }

    for (const auto& vehactiveID : activeVissimIds)
    {
        std::vector<VehPos>::iterator it;
        int fid = vehactiveID;
        it = std::find_if(vehicles.begin(), vehicles.end(), [&fid](VehPos const& obj){
        return obj.vehID == fid;
        } );

        if(it == vehicles.end())
                       {
                            if(vehactiveID != 9999){
                            VehPos vehicle;
                            vehicle.vehID = vehactiveID;
                            vehicle.omnetID = nextNode;
                            vehicle.enteredSimulation = true;
                            vehicle.assignedDirection = false;
                            vehicles.push_back(vehicle);
                            allVehiclesEverInSim.push_back(vehicle);
                            assignIds(vehactiveID,nextNode);
                            }
                       }

    }

tempVehicles = vehicles;

    for( auto& veh:tempVehicles)
    {
        int id = veh.vehID;
                               std::vector<int>::iterator itera = std::find(activeVissimIds.begin(), activeVissimIds.end(), id);

                               if (itera == activeVissimIds.end())
                               {

                                   std::vector<VehPos>::iterator itit = std::find_if(vehicles.begin(), vehicles.end(), [&id](VehPos const& objec){
                                           return objec.vehID == id;
                                       } );

                                   if(itit != vehicles.end())
                                   {
                                       int index = itit-vehicles.begin();
                                       int deletedOmnetID = vehicles[index].omnetID;
                                       assignStopVissimID(deletedOmnetID);
                                       vehicles.erase(itit);
                                       std::cout << "Erasing vehicle with ID: " << id << endl;
                                   }

                               }
    }
        previousActiveVissimIdsCopy = activeVissimIds;
        activeVissimIds.clear();
        tempVehicles.clear();
}

void HaifaMobilityManager::assignIds(int visID, int omnetID)
{
    std::stringstream modName;
    modName << "^.node[" << nextNode << "].mobility";
    HaifaMobility *hNode = check_and_cast<HaifaMobility*>(getModuleByPath(modName.str().c_str()));
    hNode->setVisID(visID);
    hNode->setOmnetID(omnetID);
    std::ofstream asVehs;
    asVehs.open("assignedVehs.csv", std::ios_base::app);
    asVehs << modName.str() << ";" << visID << ";" << int(simTime().dbl()) << endl;
    asVehs.close();
    nextNode++;
}

void HaifaMobilityManager::assignStopVissimID(int omnetID)
{
    std::stringstream modName;
    modName << "^.node[" << omnetID << "]";
    cModule *node = getModuleByPath(modName.str().c_str());
    node->callFinish();
    node->deleteModule();
}

inet::Coord HaifaMobilityManager::getVehicleCoordinatesFromFile(std::string vehID, std::string fileName, int simSecond)
{
    inet::Coord vehPos;
    std::string file = readFile(fileName);
    std::stringstream position;

    position << int(simTime().dbl()) << ";" << vehID;
    std::string sx, sy, sz;
    double x, y, z;
    if(file.find(position.str()) == std::string::npos)
    {
        return Coord::ZERO;
    }
    else
    {
    size_t pos = file.find(position.str());

    std::string substring = file.substr(pos);
    size_t pospos = substring.find('\n');
    std::string subsubstring = substring.substr(0, pospos);
    std::string spos_s = subsubstring.substr(subsubstring.find(';'));
    std::string spos_ss = spos_s.substr(spos_s.find(';') + 1);
    std::string spos_sss = spos_ss.substr(spos_ss.find(';') + 1);
    std::string spos_ssss = spos_sss.substr(0, spos_sss.find(';'));

    sx = spos_ssss.substr(0, spos_ssss.find(' '));
    std::string sy_s = spos_ssss.substr(spos_ssss.find(' ') + 1);
    sy = sy_s.substr(0, sy_s.find(' '));
    std::string sz_s = sy_s.substr(sy_s.find(' ') + 1);
    sz = sz_s.substr(0);

    x = stod(sx);
    y = stod(sy);
    z = stod(sz);

    vehPos.x = x;
    vehPos.y = y;
    vehPos.z = z;

    return vehPos;
}
}

int HaifaMobilityManager::getVehicleDetectionSecond(std::string vehID, std::string fileName, int simSecond)
{
    std::string file = readFile(fileName);
    std::stringstream position;

    position << int(simTime().dbl()) << ";" << vehID;
    if(file.find(position.str()) == std::string::npos)
        {
            return 99999;
        }
    else
    {

    size_t pos = file.find(position.str());

    std::string substring = file.substr(pos);
    size_t pospos = substring.find('\n');
    std::string subsubstring = substring.substr(0, pospos);
    std::string spos_s = subsubstring.substr(subsubstring.find(';'));
    std::string spos_ss = spos_s.substr(spos_s.find(';') + 1);
    std::string spos_sss = spos_ss.substr(spos_ss.find(';') + 1);

    std::string temp1 = spos_sss.substr(spos_sss.find(';')+1);
    std::string temp2 = temp1.substr(temp1.find(';')+1);
    std::string temp3 = temp2.substr(0,temp2.find(';'));

    int firstDetectedSecond = stoi(temp3);

    return firstDetectedSecond;
}
}

const char *HaifaMobilityManager::getVehicleDirection(std::string vehID, std::string fileName, int simSecond)
{
    std::string file = readFile(fileName);
    std::stringstream position;

    position << int(simTime().dbl()) << ";" << vehID;
    if(file.find(position.str()) == std::string::npos)
        {
            return "falseApproach";
        }
    else
    {

    size_t pos = file.find(position.str());

    std::string substring = file.substr(pos);
    size_t pospos = substring.find('\n');
    std::string subsubstring = substring.substr(0, pospos);
    std::string spos_s = subsubstring.substr(subsubstring.find(';'));
    std::string spos_ss = spos_s.substr(spos_s.find(';') + 1);
    std::string spos_sss = spos_ss.substr(spos_ss.find(';') + 1);

    std::string temp1 = spos_sss.substr(spos_sss.find(';')+1);
    std::string temp2 = temp1.substr(temp1.find(';')+1);
    std::string temp3 = temp2.substr(temp2.find(';')+1);
    std::string temp4 = temp3.substr(temp3.find(';')+1);

    int linkNumber = stoi(temp4);

    if(linkNumber==8||linkNumber==2||linkNumber==10||linkNumber==14||linkNumber==12||linkNumber==13||linkNumber==10013)
    {
        const char *vehDirection = "e";
        return vehDirection;
    }
    else if(linkNumber==5||linkNumber==6||linkNumber==7)
    {
        const char *vehDirection = "n";
        return vehDirection;
    }
    else if(linkNumber==3||linkNumber==4)
    {
        const char *vehDirection = "s";
        return vehDirection;
    }
    else if(linkNumber==9||linkNumber==1||linkNumber==11)
    {
        const char *vehDirection = "w";
        return vehDirection;
    }
    else
    {
        std::cout << "Vehicle " << vehID << "does not belong to any approach!!!" << endl;
    }

    return NULL;
}
}


void HaifaMobilityManager::addToLPI(int vehID, double distanceToRSU)
{
    const char *approach;
    const auto& tmp = vehID;
        std::vector<VehPos>::iterator cit;
        cit = std::find_if(allVehiclesEverInSim.begin(), allVehiclesEverInSim.end(), [&tmp](VehPos const& obj){
        return obj.vehID == tmp;
        } );
        if(cit != allVehiclesEverInSim.end())
        approach =  cit->direction;
        else
        {
        EV_INFO << "Vehicle " << vehID << " was not found in allVehiclesEverInSim vector when adding to LPI" << endl;
        }

        std::vector<VehPos>::iterator it;
        it = std::find_if(allVehiclesInSimulation.begin(), allVehiclesInSimulation.end(), [&vehID](VehPos const& obj){
                return obj.vehID == vehID;
                } );

                if(it == allVehiclesInSimulation.end())
                               {
                                    int firstDetectedSec;
                                    std::stringstream erasedFile;
                                    erasedFile << int(simTime().dbl()) << ".simsec";
                                    std::vector<VehPos>::iterator itit;
                                            itit = std::find_if(vehicles.begin(), vehicles.end(), [&vehID](VehPos const& obj){
                                                    return obj.vehID == vehID;
                                                    } );
                                    if(itit != vehicles.end())
                                    {
                                        std::cout << "Found vehicle " << vehID << " in vector \"vehicles\" as: " << itit->vehID << endl;
                                        firstDetectedSec = itit->firstDetectionSecond;
                                    }
                                    else
                                    {
                                        std::cout << "Fatal Error: vehicle" << itit->vehID << " not found in \"vehicles\" vector!!" << endl;
                                    }
                                    VehPos vehicle;
                                    vehicle.vehID = vehID;
                                    vehicle.direction = approach;
                                    vehicle.distanceToRSU = distanceToRSU;
                                    vehicle.firstDetectionSecond = firstDetectedSec;
                                    vehicle.firstCommunicatedSecond = int(simTime().dbl());
                                    allVehiclesInSimulation.push_back(vehicle);

                                    if(strcmp(approach,"w")==0)
                                    {
                                        VehPos vehicle;
                                        vehicle.vehID = vehID;
                                        vehicle.direction = approach;
                                        vehicle.distanceToRSU = distanceToRSU;
                                        vehicle.firstDetectionSecond = firstDetectedSec;
                                        vehicle.firstCommunicatedSecond = int(simTime().dbl());
                                        L_WEST.push_back(vehicle);
                                    }
                                    if(strcmp(approach,"e")==0)
                                    {
                                        VehPos vehicle;
                                        vehicle.vehID = vehID;
                                        vehicle.direction = approach;
                                        vehicle.distanceToRSU = distanceToRSU;
                                        vehicle.firstDetectionSecond = firstDetectedSec;
                                        vehicle.firstCommunicatedSecond = int(simTime().dbl());
                                        L_EAST.push_back(vehicle);
                                    }
                                    if(strcmp(approach,"n")==0)
                                    {
                                        VehPos vehicle;
                                        vehicle.vehID = vehID;
                                        vehicle.direction = approach;
                                        vehicle.distanceToRSU = distanceToRSU;
                                        vehicle.firstDetectionSecond = firstDetectedSec;
                                        vehicle.firstCommunicatedSecond = int(simTime().dbl());
                                        L_NORTH.push_back(vehicle);
                                    }
                                    if(strcmp(approach,"s")==0)
                                    {
                                        VehPos vehicle;
                                        vehicle.vehID = vehID;
                                        vehicle.direction = approach;
                                        vehicle.distanceToRSU = distanceToRSU;
                                        vehicle.firstDetectionSecond = firstDetectedSec;
                                        vehicle.firstCommunicatedSecond = int(simTime().dbl());
                                        L_SOUTH.push_back(vehicle);
                                    }
                               }
}

void HaifaMobilityManager::addToLEVandLAS(int vID, bool approaching)
{
    int vehID=vID;

    const char *approach="";
       const auto& tmp = vehID;
           std::vector<VehPos>::iterator cit;
           cit = std::find_if(allVehiclesEverInSim.begin(), allVehiclesEverInSim.end(), [&tmp](VehPos const& obj){
           return obj.vehID == tmp;
           } );
           if(cit != allVehiclesEverInSim.end())
           approach =  cit->direction;
           else
           {
           EV_INFO << "Vehicle " << vehID << " was not found in allVehiclesEverInSim vector when adding to LEV and LAS" << endl;
           }

    if(strcmp(approach,"")!=0 && strcmp(approach,"falseApproach")!=0)
    {
    std::vector<VehPos>::iterator it;
           it = std::find_if(LEV.begin(), LEV.end(), [&vID](VehPos const& obj){
                   return obj.vehID == vID;
                   } );
                   if(it == LEV.end())
                                  {
                       std::cout << "Vehicle " << vehID << " is not in LEV vector - ADDING IT" << endl;
                                       VehPos vehicle;
                                       vehicle.vehID = vehID;
                                       vehicle.direction = approach;
                                       vehicle.startTime = simTime().dbl();
                                       LEV.push_back(vehicle);
                                       if(strcmp(approach,"w")==0)
                                       {
                                           LAS_WEST.push_back(vehicle);
                                       }
                                       if(strcmp(approach,"e")==0)
                                       {
                                           LAS_EAST.push_back(vehicle);
                                       }
                                       if(strcmp(approach,"n")==0)
                                       {
                                           LAS_NORTH.push_back(vehicle);
                                       }
                                       if(strcmp(approach,"s")==0)
                                       {
                                           LAS_SOUTH.push_back(vehicle);
                                       }
                                   }
                   if(it != LEV.end())
                   {
                       std::cout << "Vehicle " << vehID << " with direction "<<approach<<" IS in the LEV vector - adding it to LAS" << endl;
                       VehPos vehicle;
                       vehicle.vehID=vehID;
                       vehicle.direction = approach;
                       if(strcmp(approach,"w")==0)
                           {
                               LAS_WEST.push_back(vehicle);
                           }
                       if(strcmp(approach,"e")==0)
                           {
                               LAS_EAST.push_back(vehicle);
                           }
                       if(strcmp(approach,"n")==0)
                           {
                               LAS_NORTH.push_back(vehicle);
                           }
                       if(strcmp(approach,"s")==0)
                           {
                               LAS_SOUTH.push_back(vehicle);
                           }
                   }

}
}

void HaifaMobilityManager::updateListOfLeftVehicles()
{
    int currentSec = int(simTime().dbl());
    if(currentSec>=2)
    {
       std::list<int> thisSecVehs,lastSecVehs;
       thisSecVehs = getVehicleIdsFromFile(currentSec);
       lastSecVehs = getVehicleIdsFromFile(currentSec-1);
       for(auto& veh:lastSecVehs)
       {
            std::list<int>::iterator it;
            it = std::find(thisSecVehs.begin(), thisSecVehs.end(), veh);
            if(it != thisSecVehs.end())
            {
                //vehicle from last second is present this second
            }
            else
            {
                listOfLeftVehicles.push_back(veh);
            }
            
       }
    }
}

std::list<int> HaifaMobilityManager::getVehicleIdsFromFile(int second)
{
    std::stringstream inFileName;
    inFileName << second << ".simsec";
    std::ifstream inputFile;
    inputFile.open(inFileName.str());

    std::stringstream inputContentBuffer;
    inputContentBuffer << inputFile.rdbuf();
    inputFile.close();

    std::string inputContent = inputContentBuffer.str();
    std::stringstream strippedBuffer;
    strippedBuffer << inputContent;
    std::stringstream stripBuff;
    std::string line = "";
    std::string fullFile = strippedBuffer.str();
    std::size_t found;
    found = fullFile.find("9999");

    if (found!=std::string::npos)
    {
        std::list<int> newList;
        newList.clear();
        return newList;
    }
    else
    {

    std::list<int> newList;
    while (std::getline(strippedBuffer, line))
    {
        std::string outS1;
        outS1 = line.substr(line.find(';') + 1);
        std::string currentString = outS1.substr(0, outS1.find(';'));
        newList.push_back(stoi(currentString));
    }
    return newList;
    }
}

void HaifaMobilityManager::estimateMLRfromKnownVehicles()
{
       std::vector<VehPos> totalCommunicatedVehicles = LAS_WEST;
       totalCommunicatedVehicles.insert(totalCommunicatedVehicles.end(),LAS_EAST.begin(),LAS_EAST.end());
       totalCommunicatedVehicles.insert(totalCommunicatedVehicles.end(),LAS_NORTH.begin(),LAS_NORTH.end());
       totalCommunicatedVehicles.insert(totalCommunicatedVehicles.end(),LAS_SOUTH.begin(),LAS_SOUTH.end());
       double numKnownPacketLost_WEST = 0.0;
       double numKnownPacketLost_EAST = 0.0;
       double numKnownPacketLost_NORTH = 0.0;
       double numKnownPacketLost_SOUTH = 0.0;

       auto iter = LEV.begin();
       int deleted=0;
       for ( ; iter !=  LEV.end(); iter++) 
       {
            std::vector<int>::iterator listiter;
            listiter = std::find(listOfLeftVehicles.begin(), listOfLeftVehicles.end(), iter->vehID);
            if(listiter != listOfLeftVehicles.end())
            {
                std::cout << "Stage 10: Deleting left vehicle " << iter->vehID << " from the LEV vector!!!" << endl; 
                LEV.erase(iter);
                --iter;
                deleted++;
                std::cout << "Stage 11: The vehicle has been successfully deleted." << endl;
            }
        }
       std::cout << "Stage12: in LEV, there are: " << LEV.size() << " vehicles" << endl;
       std::cout << "Stage13: There were " << deleted << " vehicles deleted from LEV in this simsecond" << endl;
       for( auto& veh:LEV)
           {
               int vehID=veh.vehID;
               std::vector<VehPos>::iterator it;
               it = std::find_if(totalCommunicatedVehicles.begin(), totalCommunicatedVehicles.end(), [&vehID](VehPos const& obj){
                      return obj.vehID == vehID;
                      } );
               if(it==totalCommunicatedVehicles.end())
               {
                   const char *approach = veh.direction;
                   std::cout << "estimateMLRfromKnown:: vehicle " << veh.vehID << " approach is " << approach << endl;
                   if(strcmp(approach,"w")==0)
                   {
                       numKnownPacketLost_WEST++;
                   }
                   if(strcmp(approach,"e")==0)
                   {
                       numKnownPacketLost_EAST++;
                   }
                   if(strcmp(approach,"n")==0)
                   {
                       numKnownPacketLost_NORTH++;
                   }
                   if(strcmp(approach,"s")==0)
                   {
                       numKnownPacketLost_SOUTH++;
                   }

               }
           }

       std::cout << "numKnownPacketLost_WENS: "<<numKnownPacketLost_WEST<<", " << numKnownPacketLost_EAST<<", "<<numKnownPacketLost_NORTH<<", "<<numKnownPacketLost_SOUTH<<endl;
       double EstTotalLost_WEST = numKnownPacketLost_WEST+NumUnknown_WEST;
       double EstTotalLost_EAST = numKnownPacketLost_EAST+NumUnknown_EAST;
       double EstTotalLost_NORTH = numKnownPacketLost_NORTH+NumUnknown_NORTH;
       double EstTotalLost_SOUTH = numKnownPacketLost_SOUTH+NumUnknown_SOUTH;

       std::ofstream EstTotLost;
       EstTotLost.open("EstTotLost.csv", std::ios_base::app);
       EstTotLost << int(simTime().dbl()) << ";" << EstTotalLost_WEST << ";" << EstTotalLost_EAST << ";" << EstTotalLost_NORTH << ";" << EstTotalLost_SOUTH << endl;
       EstTotLost.close();

       LAS_WEST.clear();
       LAS_EAST.clear();
       LAS_NORTH.clear();
       LAS_SOUTH.clear();
       totalCommunicatedVehicles.clear();
       numKnownPacketLost_WEST=0;
       numKnownPacketLost_EAST=0;
       numKnownPacketLost_NORTH=0;
       numKnownPacketLost_SOUTH=0;
}
void HaifaMobilityManager::estimateMLR()
{
    std::vector<double> distances_WEST;
    std::vector<int> times_WEST;
    for (const auto& vehicle : L_WEST)
    {
        distances_WEST.push_back(vehicle.distanceToRSU);
        if (vehicle.firstCommunicatedSecond!=99999)
        {
        int duration = vehicle.firstCommunicatedSecond-vehicle.firstDetectionSecond;
        std::cout << "Vehicle " << vehicle.vehID << " first Detected at: " << vehicle.firstDetectionSecond << "s and first communicated at: " << vehicle.firstCommunicatedSecond << "s. Duration= " << duration << endl;
        times_WEST.push_back(duration);
        }
        else
        {
            std::cout << "estimateMLR():: Vehicle " << vehicle.vehID << " has a false firstDetectionSecond" << endl;
        }
    }
    std::cout << "DistancesWEST vector size: " << distances_WEST.size() << endl;

    std::vector<double> distances_EAST;
    std::vector<int> times_EAST;
        for (const auto& vehicle : L_EAST)
        {
            distances_EAST.push_back(vehicle.distanceToRSU);
            if (vehicle.firstCommunicatedSecond!=99999)
            {
                int duration = vehicle.firstCommunicatedSecond-vehicle.firstDetectionSecond;
                std::cout << "Vehicle " << vehicle.vehID << " first Detected at: " << vehicle.firstDetectionSecond << "s and first communicated at: " << vehicle.firstCommunicatedSecond << "s. Duration= " << duration << endl;
                times_EAST.push_back(duration);
            }
            else
            {
                std::cout << "estimateMLR():: Vehicle " << vehicle.vehID << " has a false firstDetectionSecond" << endl;
            }
        }
        std::cout << "DistancesEAST vector size: " << distances_EAST.size() << endl;

   std::vector<double> distances_NORTH;
   std::vector<int> times_NORTH;
   for (const auto& vehicle : L_NORTH)
   {
       distances_NORTH.push_back(vehicle.distanceToRSU);
       if (vehicle.firstCommunicatedSecond!=99999)
            {
                int duration = vehicle.firstCommunicatedSecond-vehicle.firstDetectionSecond;
                std::cout << "Vehicle " << vehicle.vehID << " first Detected at: " << vehicle.firstDetectionSecond << "s and first communicated at: " << vehicle.firstCommunicatedSecond << "s. Duration= " << duration << endl;
                times_NORTH.push_back(duration);
            }
            else
            {
                std::cout << "estimateMLR():: Vehicle " << vehicle.vehID << " has a false firstDetectionSecond" << endl;
            }
   }
   std::cout << "DistancesNORTH vector size: " << distances_NORTH.size() << endl;

   std::vector<double> distances_SOUTH;
   std::vector<int> times_SOUTH;
       for (const auto& vehicle : L_SOUTH)
       {
           distances_SOUTH.push_back(vehicle.distanceToRSU);
           if (vehicle.firstCommunicatedSecond!=99999)
            {
                int duration = vehicle.firstCommunicatedSecond-vehicle.firstDetectionSecond;
                std::cout << "Vehicle " << vehicle.vehID << " first Detected at: " << vehicle.firstDetectionSecond << "s and first communicated at: " << vehicle.firstCommunicatedSecond << "s. Duration= " << duration << endl;
                times_SOUTH.push_back(duration);
            }
            else
            {
                std::cout << "estimateMLR():: Vehicle " << vehicle.vehID << " has a false firstDetectionSecond" << endl;
            }
       }
   std::cout << "DistancesSOUTH vector size: " << distances_SOUTH.size() << endl;

   double C_WEST = std::accumulate(std::begin(distances_WEST), std::end(distances_WEST), 0.0) / distances_WEST.size(); //calculate average C_WEST
   double C_EAST = std::accumulate(std::begin(distances_EAST), std::end(distances_EAST), 0.0) / distances_EAST.size(); //calculate average C_EAST
   double C_NORTH = std::accumulate(std::begin(distances_NORTH), std::end(distances_NORTH), 0.0) / distances_NORTH.size(); //calculate average C_NORTH
   double C_SOUTH = std::accumulate(std::begin(distances_SOUTH), std::end(distances_SOUTH), 0.0) / distances_SOUTH.size(); //calculate average C_SOUTH

   double T_WEST = (double)std::accumulate(std::begin(times_WEST), std::end(times_WEST), 0.0) / times_WEST.size();
   double T_EAST = (double)std::accumulate(std::begin(times_EAST), std::end(times_EAST), 0.0) / times_EAST.size();
   double T_NORTH = (double)std::accumulate(std::begin(times_NORTH), std::end(times_NORTH), 0.0) / times_NORTH.size();
   double T_SOUTH = (double)std::accumulate(std::begin(times_SOUTH), std::end(times_SOUTH), 0.0) / times_SOUTH.size();

   std::cout << "Average T_WEST: " << T_WEST << " s" << endl;
   std::cout << "Average T_EAST: " << T_EAST << " s" << endl;
   std::cout << "Average T_NORTH: " << T_NORTH << " s" << endl;
   std::cout << "Average T_SOUTH: " << T_SOUTH << " s" << endl;

   std::cout << "Average C_WEST: " << C_WEST << " m" << endl;
   std::cout << "Average C_EAST: " << C_EAST << " m" << endl;
   std::cout << "Average C_NORTH: " << C_NORTH << " m" << endl;
   std::cout << "Average C_SOUTH: " << C_SOUTH << " m" << endl;

   //TASK 2: return F
   double F_WEST = (double)L_WEST.size()/(double)timeWindow;
   double F_EAST = (double)L_EAST.size()/(double)timeWindow;
   double F_NORTH = (double)L_NORTH.size()/(double)timeWindow;
   double F_SOUTH = (double)L_SOUTH.size()/(double)timeWindow;

   std::cout << "Intensity of the vehicle flow F_WEST=" << F_WEST << "vehicles/s" << endl;
   std::cout << "Intensity of the vehicle flow F_EAST=" << F_EAST << "vehicles/s" << endl;
   std::cout << "Intensity of the vehicle flow F_NORTH=" << F_NORTH << "vehicles/s" << endl;
   std::cout << "Intensity of the vehicle flow F_SOUTH=" << F_SOUTH << "vehicles/s" << endl;


   double NumUnknown_WEST = T_WEST*F_WEST;
   double NumUnknown_EAST = T_EAST*F_EAST;
   double NumUnknown_NORTH = T_NORTH*F_NORTH;
   double NumUnknown_SOUTH = T_SOUTH*F_SOUTH;

   std::cout << "Estimated number of unknown vehicles in WEST approach: " << NumUnknown_WEST << " vehicles" << endl;
   std::cout << "Estimated number of unknown vehicles in EAST approach: " << NumUnknown_EAST << " vehicles" << endl;
   std::cout << "Estimated number of unknown vehicles in NORTH approach: " << NumUnknown_NORTH << " vehicles" << endl;
   std::cout << "Estimated number of unknown vehicles in SOUTH approach: " << NumUnknown_SOUTH << " vehicles" << endl;

   L_WEST.clear();
   L_EAST.clear();
   L_NORTH.clear();
   L_SOUTH.clear();
   distances_WEST.clear();
   distances_EAST.clear();
   distances_NORTH.clear();
   distances_SOUTH.clear();
   times_WEST.clear();
   times_EAST.clear();
   times_NORTH.clear();
   times_SOUTH.clear();
}

}
