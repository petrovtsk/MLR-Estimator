//
// Copyright (C) 2006-2017 Tibor Petrov <tibor.petrov@erachair.uniza.sk>
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

package inet.mobility.single;

//
// @author Tibor Petrov
//

simple HaifaMobilityManager
{
    parameters:
        @display("i=block/network2");
        @class(HaifaMobilityManager);
        //string fileName;  // fileName of a file with stripped header
        //string vehicleIdFile; // fileName of a file with all vehicle IDs used for init
      	double updateInterval=default(1); //how often the position should be updated in seconds
		int maxVehicles; //number of vehicles in the simulation
		int simStartTime; //time when the vehicles begin to move
		int waitForVissim; //how much time in milliseconds should Omnet wait for VISSIM to simulate the next second
		string vissimFilePath;
		string omnetOutputFilePath;
		string temporaryOmnetOutputFilePath;
		string simSecFilePath;
		string pythonScriptFilePath;
		string copiedCarsTxtPath;
		int ignoreWest = default(0); //ignores messages comming from vehicles approaching from west
		int timeWindow = default(10); //time window for estimating the vehicle flow [s]
		double averageSpeedWEST = default(50); //average speed of vehicles in [kph]
		double averageSpeedEAST = default(50); //average speed of vehicles in [kph]
		double averageSpeedNORTH = default(50); //average speed of vehicles in [kph]
		double averageSpeedSOUTH = default(50); //average speed of vehicles in [kph]
		double P_W; //observed perimeter in [m]
		double P_E; //observed perimeter in [m]
		double P_N; //observed perimeter in [m]
		double P_S; //observed perimeter in [m]
		double RSUPosX;
		double RSUPosY;
}

