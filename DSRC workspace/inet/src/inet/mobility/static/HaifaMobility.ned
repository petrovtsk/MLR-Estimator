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

package inet.mobility.static;

import inet.mobility.base.MobilityBase;

//
// This mobility module does nothing; it can be used for stationary nodes.
//
// @author Andras Varga
//
simple HaifaMobility extends MobilityBase
{
    parameters:
        double initialX @unit(m) = default(uniform(this.constraintAreaMinX, this.constraintAreaMaxX));
        double initialY @unit(m) = default(uniform(this.constraintAreaMinY, this.constraintAreaMaxY));
        double initialZ @unit(m) = default(nanToZero(uniform(this.constraintAreaMinZ, this.constraintAreaMaxZ)));
        double initialLatitude @unit(deg) = default(0deg/0);
        double initialLongitude @unit(deg) = default(0deg/0);
        double initialAltitude @unit(m) = default(0m);
        bool initFromDisplayString = default(true);
        double updateInterval @unit(s) = default(1s);
        int vissimID;
        double antennaHeight;
        @class(HaifaMobility);
}
