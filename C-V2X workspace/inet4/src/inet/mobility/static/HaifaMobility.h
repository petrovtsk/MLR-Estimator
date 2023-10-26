
#ifndef __INET_HaifaMobility_H
#define __INET_HaifaMobility_H

#include "inet/common/INETDefs.h"

#include "inet/mobility/base/MovingMobilityBase.h"
#include "inet/mobility/single/HaifaMobilityManager.h"

namespace inet {

class INET_API HaifaMobility : public MovingMobilityBase
{
  protected:
    virtual void initialize() override;

  public:

    HaifaMobility();
    void updatePositionOfVehicle();
    void setVisID(int vissimID);
    int getVisID();
    void setOmnetID(int omID);
    virtual void move() override;


    virtual Coord getCurrentPosition() override { return lastPosition; }

    virtual Coord getCurrentVelocity() override { return Coord::ZERO; }

    virtual double getMaxSpeed() const override { return 0; }

  private:
    int visID;
    int omnetID;
    double antennaHeight;
    simtime_t updateInterval;

};

}

#endif

