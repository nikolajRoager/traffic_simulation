#include "RoadVehicle.hpp"
#include "Node.hpp"
#include "Road.hpp"

#include "TrafficExceptions.hpp"

//100 km/h = 100000/3600 m/s = 1000/36 m/s=250/9 m/s, divide by the time it took for us to reach this speed to get average acceleration, this undersells acceleration at low speed and oversells it at high speed
#define SecondsTo100kmh_factor (250.0/9.0)

/*
Going from 100 km/h=250/9 m/s to 0 m/s with breaking acceleration, is the same as accelerating to 250/9 m/s from 0 m/s with acceleration b. This takes t=(250/9)/b seconds, during that time we travel d=b*t^2/2 = (250/9)^2/2b -> b =(250/9)^2/2d

So  b = (250/9 m/s)^2/2d = (31250/81 m/s)/d
*/

#define BrakingDist100kmh_factor (31250.0/81.0)

RoadVehicle::RoadVehicle(double _length, double _maxSpeed, double SecondsTo100kmh , double BrakingDist100kmh,double time,const Road* R,int lane,double speed, double pos, bool direction) noexcept:
length(_length),
maxSpeed(_maxSpeed),
acceleration(SecondsTo100kmh_factor/SecondsTo100kmh),
braking(BrakingDist100kmh_factor/BrakingDist100kmh),
current_state(time,R,lane,speed,pos,direction)
{
}


//At what time-point do we need to re-update, return -1 if not further updates
double RoadVehicle::nextUpdate() noexcept
{
    return -1.0;//STUB
}


//Advance until this time
//@throws an exception if we advance past the next scheduled update
void RoadVehicle::setTime(double time)
{
    //If the current state is null, there is nothing to do
    if (current_state.myRoad!=nullptr)
    {
        current_state.time=time;
    }
}

//Same as the above, but goes exactly to the next scheduled update, returns the new time, does nothing if no new updates exist
double RoadVehicle::gotoUpdate(double time) noexcept
{
    //No change, if no update
    return time;//STUB
}

//Drive onto this new road
void RoadVehicle::enterRoad(const Road* R, bool _direction, int _lane, double time) noexcept
{
    if (R!=nullptr)
        current_state=keyframe(time,R,_lane,0.0,0.0,_direction);
    else
        current_state=keyframe(time);
    keyframes.insert({time,current_state});
}


std::string RoadVehicle::toString()const
{
    return "[Car with length "+std::to_string(length)+" m top speed "+std::to_string(maxSpeed)+" m/s acceleration "+std::to_string(acceleration)+" driving "+(current_state.myRoad==nullptr ? "offroad]" :" on road "+std::to_string(current_state.myRoad->getRoadID())+", position "+std::to_string(current_state.pos)+" m, lane "+std::to_string(current_state.lane)+", going direction "+std::to_string(current_state.direction)+", with speed "+std::to_string(current_state.speed)+", m/s at time "+std::to_string(current_state.time)+ " s]");
}


/*
size_t RoadVehicle::getRoadID(double time) const noexcept
{
    const keyframe& K = get_lastFrame(time);
    //Get highest
    return K.myRoad ==nullptr ? -1 :K.myRoad->getRoadID();
}

int   RoadVehicle::getLane(double time) const noexcept
{
    return get_lastFrame(time).lane;
}

double RoadVehicle::getPos(double time) const noexcept
{
    return get_lastFrame(time).pos;
}

bool RoadVehicle::getDirection(double time) const noexcept
{
    return get_lastFrame(time).direction;
}

double RoadVehicle::getSpeed(double time) const noexcept
{
    return get_lastFrame(time).speed;
}

double RoadVehicle::getTime(double time) const noexcept
{
    return get_lastFrame(time).time;
}*/



size_t RoadVehicle::getRoadID() const noexcept
{
    return current_state.myRoad ==nullptr ? -1 :current_state.myRoad->getRoadID();
}

int   RoadVehicle::getLane() const noexcept
{
    return current_state.lane;
}

double RoadVehicle::getPos() const noexcept
{
    return current_state.pos;
}

bool RoadVehicle::getDirection() const noexcept
{
    return current_state.direction;
}

double RoadVehicle::getSpeed() const noexcept
{
    return current_state.speed;
}

double RoadVehicle::getTime() const noexcept
{
    return current_state.time;
}
