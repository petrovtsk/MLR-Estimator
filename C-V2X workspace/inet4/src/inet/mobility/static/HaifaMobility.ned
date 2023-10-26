package inet.mobility.static;

import inet.mobility.base.MobilityBase;


simple HaifaMobility extends MobilityBase
{
    parameters:
        double initialX @unit(m) = default(uniform(this.constraintAreaMinX, this.constraintAreaMaxX));
        double initialY @unit(m) = default(uniform(this.constraintAreaMinY, this.constraintAreaMaxY));
        double initialZ @unit(m) = default(nanToZero(uniform(this.constraintAreaMinZ, this.constraintAreaMaxZ)));
        double initialLatitude @unit(deg) = default(0deg/0);
        double initialLongitude @unit(deg) = default(0deg/0);
        double initialAltitude @unit(m) = default(0m);
        double initialHeading @unit(deg) = default(0deg);
        double initialElevation @unit(deg) = default(0deg);
        double initialBank @unit(deg) = default(0deg);
        bool initFromDisplayString = default(true);
        double updateInterval @unit(s) = default(1s);
        bool updateFromDisplayString = default(true); 
        int vissimID;
        double antennaHeight;
        bool faceForward;
        @class(HaifaMobility);
}
