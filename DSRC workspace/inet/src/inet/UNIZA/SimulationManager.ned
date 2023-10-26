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

package inet.UNIZA;

//
// @author Tibor Petrov
//

simple SimulationManager
{
    parameters:
        @display("i=block/network2");
        @class(SimulationManager);
      	double updateInterval=default(1); //how often the position should be updated in seconds
		string outputFileName;
		string vehicleNamesFile;
		string EVmoduleName;
		double startTime;
}

