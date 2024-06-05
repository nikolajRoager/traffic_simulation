#include "RoadVehicle.hpp"

#include "TrafficExceptions.hpp"


//100 km/h = 100000/3600 m/s = 1000/36 m/s=250/9 m/s, divide by the time it took for us to reach this speed to get average acceleration, this undersells acceleration at low speed and oversells it at high speed
#define SecondsTo100kmh_factor (250.0/9.0)


/*
Going from 100 km/h=250/9 m/s to 0 m/s with breaking acceleration, is the same as accelerating to 250/9 m/s from 0 m/s with acceleration b. This takes t=(250/9)/b seconds, during that time we travel d=b*t^2/2 = (250/9)^2/2b -> b =(250/9)^2/2d

So  b = (250/9 m/s)^2/2d = (31250/81 m/s)/d
*/


#define BrakingDist100kmh_factor (31250/81)


RoadVehicle::RoadVehicle(double _length, double _maxSpeed, double SecondsTo100kmh , double BrakingDist100kmh) noexcept:
length(_length),
maxSpeed(_maxSpeed),
acceleration(SecondsTo100kmh_factor/SecondsTo100kmh),
braking(BrakingDist100kmh_factor/BrakingDist100kmh)
{

}


//At what time-point do we need to re-update, return -1 if not further updates
double RoadVehicle::nextUpdate() noexcept
{
    return -1.0;//STUB
}


//Advance until this time
//@throws an exception if we advance past the next scheduled update
void RoadVehicle::setTime(double /*time*/)
{
    //STUB
}

//Same as the above, but goes exactly to the next scheduled update, returns the new time, does nothing if no new updates exist
double RoadVehicle::gotoUpdate() noexcept
{

    return 0.0;//STUB
}

//Drive onto this new road
void RoadVehicle::enterRoad(double /*time*/) noexcept
{
    //STUB
}

