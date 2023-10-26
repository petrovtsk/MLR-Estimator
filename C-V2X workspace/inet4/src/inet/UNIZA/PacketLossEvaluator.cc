
#include <fstream>
#include <sstream>
#include <algorithm>
#include "PacketLossEvaluator.h"

namespace inet{


Define_Module(PacketLossEvaluator);

void PacketLossEvaluator::initialize()
{
    update = new cMessage;
    updateInterval = par("updateInterval");
    outputFileName = par("outputFileName");
    scheduleAt(0.998, update);
    std::ofstream ofs;
    ofs.open(outputFileName, std::ios::out | std::ios::trunc);
    ofs << "SimTime;" << "SentPk;" << "RcvdPk;" << "LostPk;" << "PacketLoss" << endl;
    ofs.close();
    sentPackets = 0;
    receivedPackets = 0;
    lostPackets = 0;
    packetLoss = 0;
}

void PacketLossEvaluator::handleMessage(cMessage *msg)
{
    if (msg->isSelfMessage())
    {
        cMessage *updateMsg = new cMessage;
        scheduleAt(simTime()+updateInterval, updateMsg);
        calculateStatistics();
    }
    delete msg;
}

void PacketLossEvaluator::addSentPk()
{
    sentPackets++;
}

void PacketLossEvaluator::addRcvdPk()
{
    receivedPackets++;
}

void PacketLossEvaluator::calculateStatistics()
{
    lostPackets = sentPackets-receivedPackets;
    if (sentPackets != 0)
            {
                packetLoss = ((double)lostPackets/(double)sentPackets)*100;
                std::stringstream outputStream;
                outputStream << int(simTime().dbl()) << ";" << sentPackets << ";" << receivedPackets << ";" << lostPackets << ";" << packetLoss << endl;
                writeOutputToFile(outputStream.str());
                sentPackets = 0;
                receivedPackets = 0;
                lostPackets = 0;
                packetLoss = 0;
            }
}

void PacketLossEvaluator::writeOutputToFile(std::string outputStr)
{
    std::ofstream outfile;
    outfile.open(outputFileName, std::ios_base::app);
    outfile << outputStr;
    outfile.close();
}


}
