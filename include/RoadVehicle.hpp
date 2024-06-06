#pragma once


#include <vector>

/**Vehicle base class, this is the interface the road knows about
* All vehicles have the same basic stats, like length, maxSpeed and acceleration
*
* The vehicles are either on a road, on a particular position, and lane.
*/

class RoadVehicle{
protected:
    //All vehicles have these basic stats measured in SI units
    double length;//m
    double maxSpeed;//m/s
    double acceleration;//m/s^2 (derivative of speed when accelerating)
    double braking;//m/s^2 (derivative of speed when slowing down)


    //When was our position last updated
    double last_update_time=0;

    //Current physics and state, once again all road vehicles have these
    size_t roadId=-1;//-1 : the car has despawned/not spawned yet
    int lane=0;
    double speed=0;
    double pos=0.0;

    //Additional stats must be added by derived classes

public:

    //@param _lenght length of vehicle in m
    //@max_speed highest speed vehicle can travel at
    //SecondsTo100kmh Seconds it takes the vehicle to reach 100 km/h from full stop, this is a common testing parameter available online for most vehicles
    //BrakingDist100kmh Meters to come to a full stop from 100 km/h, this is a common testing parameter available online for most vehicles
    RoadVehicle(double _length, double _maxSpeed, double SecondsTo100kmh, double BrakingDist100kmh) noexcept;

    //At what time-point do we need to re-update, return -1 if not further updates
    double nextUpdate() noexcept;


    //Advance until this time
    //@param simulation time in secondes
    //@throw vehicle_past_update_exception if we advance past the next scheduled update
    void setTime(double time);
    double gotoUpdate() noexcept;//Same as the above, but goes exactly to the next scheduled update, returns the new time, does nothing if no new updates exist

    //Drive onto this new road
    //@param simulation time in secondes
    void enterRoad(double time) noexcept;


    //mainly for Testing, debugging, all vehicles can tell exactly what road and lane we are on, and where we are on this
    size_t getRoadId() const noexcept {return roadId;}
    int   getLane() const noexcept {return lane;}
    double getPos() const noexcept {return pos;}
};
