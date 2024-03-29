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

#ifndef __INET_HaifaMobility_H
#define __INET_HaifaMobility_H

#include "inet/common/INETDefs.h"

#include "inet/mobility/base/MovingMobilityBase.h"
#include "inet/mobility/single/HaifaMobilityManager.h"

namespace inet {

/**
 * This mobility module does not move at all; it can be used for standalone stationary nodes.
 *
 * @ingroup mobility
 * @author Andras Varga
 */
class INET_API HaifaMobility : public MovingMobilityBase
{
  protected:
    /** @brief Never happens. */
    virtual void initialize() override;

  public:

    HaifaMobility();
    void updatePositionOfVehicle();
    void setVisID(int vissimID);
    int getVisID();
    void setOmnetID(int omID);
    virtual void move() override;

    //HaifaMobility(Coord position) { lastPosition = position; }

    virtual Coord getCurrentPosition() override { return lastPosition; }

    virtual Coord getCurrentSpeed() override { return Coord::ZERO; }

    virtual double getMaxSpeed() const override { return 0; }

    //virtual Coord getConstraintAreaMax() const override { return lastPosition; }
    //virtual Coord getConstraintAreaMin() const override { return lastPosition; }

  private:
    int visID;
    int omnetID;
    double antennaHeight;
    simtime_t updateInterval;

};

} // namespace inet

#endif // ifndef __INET_STATIONARYMOBILITY_H

