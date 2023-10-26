//
// Copyright (C) 2006-2017 Christoph Sommer <sommer@ccs-labs.org>
//
// Documentation for these modules is at http://veins.car2x.org/
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

package org.car2x.veins.subprojects.veins_inet.example;

import inet.physicallayer.ieee80211.packetlevel.Ieee80211ScalarRadioMedium;
import inet.visualizer.integrated.IntegratedCanvasVisualizer;
import inet.mobility.single.HaifaMobilityManager;
import inet.UNIZA.PacketLossEvaluator;
import inet.environment.common.PhysicalEnvironment;

network ScenarioTest
{
    parameters:
        int maxHosts;
        @display("bgb=600,400");


    submodules:
        radioMedium: Ieee80211ScalarRadioMedium;
        evaluator: PacketLossEvaluator;
        physicalEnvironment: PhysicalEnvironment;
        mobilityManager: HaifaMobilityManager {
            parameters:
                @display("p=282,93;r=,,#707070");
        }
        node[maxHosts]: CarVissim {
            parameters:
                @display("p=300,300;r=,,#707070");
        }
        rsu_north: Car {
            parameters:
                @display("p=486,80;r=,,#707070");
        }
        rsu_south: Car {
            parameters:
                @display("p=486,80;r=,,#707070");
        }
        rsu_west: Car {
            parameters:
                @display("p=486,80;r=,,#707070");
        }
        rsu_east: Car {
            parameters:
                @display("p=486,80;r=,,#707070");
        }
        rsu_none: Car {
            parameters:
                @display("p=486,80;r=,,#707070");
        }
}
