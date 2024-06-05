#pragma once
/**The main source of traffic: the private Car
*
*/


#include "RoadVehicle.hpp"


class Car : public RoadVehicle
{
private:
    /*
    We inherited the following parameters from our base class

    //All vehicles have these basic stats measured in SI units
    double length;//m
    double maxSpeed;//m/s
    double acceleration;//m/s^2 (derivative of speed when accelerating)
    double braking;//m/s^2 (derivative of speed when slowing down)


    //When was our position last updated
    double last_update_time=0;

    //Current physics and state, once again all road vehicles have these
    int roadId=-1;//-1 : the car has despawned/not spawned yet
    int lane=0;
    double speed=0;
    double pos=0.0;

    */

    //Specific private car members stubbed out

public:
    //Default Car is the Volvo V60
    Car(double _length=4.761, double _maxSpeed=50/*m/s=180 km/h*/, double SecondsTo100kmh =8 , double BrakingDist100kmh=35) noexcept;

    //We inherit the following unmodified from our base class:
    /*

    //It is easier to look up the time it takes to go between 0 and 100 km/h using the vehicles engine and the braking distance from 100 km/h
    RoadVehicle(double _length, double _maxSpeed, double SecondsTo100kmh, double BrakingDist100kmh) noexcept;

    //At what time-point do we need to re-update, return -1 if not further updates
    double nextUpdate(double time) noexcept;


    //Advance until this time
    //@throws an exception if we advance past the next scheduled update
    void setTime(double time);
    double gotoUpdate() noexcept;//Same as the above, but goes exactly to the next scheduled update, returns the new time, does nothing if no new updates exist

    //Drive onto this new road
    void enterRoad(double time) noexcept;


    //mainly for Testing, debugging, all vehicles can tell exactly what road and lane we are on, and where we are on this
    int getRoadId() const noexcept {return roadId;}
    int   getLane() const noexcept {return lane;}
    double getPos() const noexcept {return pos;}


    */

};
