//
// Copyright (C) 2006 Andras Varga
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

   // loadFromFile(simSecond);
    //++simSecond;
    cModule *mobilityManager = getModuleByPath("^.^.mobilityManager");
   // updateInterval = mobilityManager->par("updateInterval");
    HaifaMobilityManager *hManager = check_and_cast<HaifaMobilityManager*>(mobilityManager);
    if(visID != 9999999)
    {
    inet::Coord nextPosLoaded = hManager->returnVehiclePosition(visID);
    if (visID == 8888888)
    {
        InformRSU *application = check_and_cast<InformRSU*>(getParentModule()->getSubmodule("udpApp",0));
        application->stopTransmission();
    }
    else
    {
    lastPosition.x = nextPosLoaded.x;
    lastPosition.y = nextPosLoaded.y;
    lastPosition.z = antennaHeight;
    //std::cout<< "Mobility parent module: " << getParentModule() << endl;
    //std::cout << "Application submodule: " << getParentModule()->getSubmodule("udpApp",0) << endl;
    InformRSU *application = check_and_cast<InformRSU*>(getParentModule()->getSubmodule("udpApp",0));
    application->startTransmission();
    }
    }
    //nextChange = simTime() + updateInterval;
}

} // namespace inet

