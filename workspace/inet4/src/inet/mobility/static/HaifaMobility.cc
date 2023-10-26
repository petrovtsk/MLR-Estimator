
#include "inet/mobility/static/HaifaMobility.h"
#include "inet/applications/udpapp/InformRSU.h"

namespace inet {

Define_Module(HaifaMobility);

HaifaMobility::HaifaMobility()
{

}

void HaifaMobility::initialize()
{
    visID = par("vissimID");
    antennaHeight = par("antennaHeight");
}

void HaifaMobility::setVisID(int vissimID)
{
    visID = vissimID;
}

void HaifaMobility::setOmnetID(int omID)
{
    omnetID = omID;
}

int HaifaMobility::getVisID()
{
    return visID;
}

void HaifaMobility::move()
{

    cModule *mobilityManager = getModuleByPath("^.^.mobilityManager");
    HaifaMobilityManager *hManager = check_and_cast<HaifaMobilityManager*>(mobilityManager);
    if(visID != 9999999)
    {
    inet::Coord nextPosLoaded = hManager->returnVehiclePosition(visID);
    if (visID == 8888888)
    {
        InformRSU *application = check_and_cast<InformRSU*>(getParentModule()->getSubmodule("app",0));
        application->stopTransmission();
    }
    else
    {
    lastPosition.x = nextPosLoaded.x;
    lastPosition.y = nextPosLoaded.y;
    lastPosition.z = antennaHeight;
    InformRSU *application = check_and_cast<InformRSU*>(getParentModule()->getSubmodule("app",0));
    application->startTransmission();
    }
    }
}

}

