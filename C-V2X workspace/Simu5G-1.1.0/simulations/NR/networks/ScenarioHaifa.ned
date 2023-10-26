package simu5g.simulations.NR.networks;

import inet.networklayer.configurator.ipv4.Ipv4NetworkConfigurator;
import inet.networklayer.ipv4.RoutingTableRecorder;
import inet.node.ethernet.Eth10G;
import inet.node.inet.Router;
import inet.node.inet.StandardHost;
import simu5g.common.binder.Binder;
import simu5g.common.carrierAggregation.CarrierAggregation;
import simu5g.nodes.Upf;
import simu5g.nodes.NR.gNodeB;
import simu5g.nodes.NR.NRUe;
import simu5g.world.radio.LteChannelControl;
import inet.mobility.single.HaifaMobilityManager;
import inet.UNIZA.PacketLossEvaluator;

network ScenarioHaifa
{
    parameters:
        int numUe = default(1);
        @display("i=block/network2;bgb=991,558;bgi=background/pisa");
    submodules:
        evaluator: PacketLossEvaluator {
            @display("p=51,417");
        }
        mobilityManager: HaifaMobilityManager {
            parameters:
                @display("p=50,335;r=,,#707070");
        }
        channelControl: LteChannelControl {
            @display("p=50,25;is=s");
        }
        routingRecorder: RoutingTableRecorder {
            @display("p=50,75;is=s");
        }
        configurator: Ipv4NetworkConfigurator {
            @display("p=50,125");
            config = xmldoc("demo.xml");
        }
        binder: Binder {
            @display("p=50,175;is=s");
        }
        carrierAggregation: CarrierAggregation {
            @display("p=50.993748,258.7;is=s");
        }
        rsu_west: StandardHost {
            @display("p=520,80;is=n;i=device/server");
        }
        rsu_east: StandardHost {
            @display("p=520,80;is=n;i=device/server");
        }
        rsu_north: StandardHost {
            @display("p=520,80;is=n;i=device/server");
        }
        rsu_south: StandardHost {
            @display("p=520,80;is=n;i=device/server");
        }
        rsu_none: StandardHost {
            @display("p=520,80;is=n;i=device/server");
        }
        router: Router {
            @display("p=363.175,115.66875;i=device/smallrouter");
        }
        upf: Upf {
            @display("p=527.35,116.9125;is=l");
        }
        gnb: gNodeB {
            @display("p=486,80;is=vl");
        }

        node[numUe]: NRUe {
            @display("p=2000,2000");
        }
    connections:
        rsu_west.pppg++ <--> Eth10G <--> router.pppg++;
        rsu_east.pppg++ <--> Eth10G <--> router.pppg++;
        rsu_north.pppg++ <--> Eth10G <--> router.pppg++;
        rsu_south.pppg++ <--> Eth10G <--> router.pppg++;
        rsu_none.pppg++ <--> Eth10G <--> router.pppg++;
        router.pppg++ <--> Eth10G <--> upf.filterGate;
        upf.pppg++ <--> Eth10G <--> gnb.ppp;
}
