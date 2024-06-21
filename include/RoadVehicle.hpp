#pragma once

#include<map>
#include<string>

//Assume a 1 second reaction time of all drivers
#define reaction_time_margin 1.0

/**Vehicle base class, this is the interface the road knows about
* All vehicles have the same basic stats, like length, maxSpeed and acceleration
*
* The vehicles are either on a road, on a particular position, and lane.
*/

class Road;
class Node;

class RoadVehicle{
protected:

    //All vehicles have these basic stats measured in SI units
    double length;//m
    double maxSpeed;//m/s
    double acceleration;//m/s^2 (derivative of speed when accelerating)
    double braking;//m/s^2 (derivative of speed when slowing down)



    //Current physics keyframe, once again all road vehicles have these
    struct keyframe
    {
        double time=0;
        const Road* myRoad=nullptr;
        int lane=0;
        double speed=0;
        double pos=0.0;
        bool direction=false;//Do we go from first to second? or the other way

        keyframe(double t):time(t){}
        keyframe(double t, const Road* R, int l, double s, double p, bool d):time(t),myRoad(R),lane(l),speed(s),pos(p),direction(d){}

        bool operator<(const keyframe& that) const
        {
            return time<that.time;
        }
    };

    //The keyframes store PAST keyframes, in simulation mode the latest keyframe is used as basis for current position.
    //an ORDERED map is used for fast lookup
    std::map<double,keyframe> keyframes;

    keyframe current_state;

    //Additional stats must be added by derived classes

    const keyframe& get_lastFrame(double /*time*/)const
    {

        return (*keyframes.rbegin()).second;
    }

public:
    //A FAUX vehicle with a position, used only for comparison purposes (i.e. searching the std::set of vehicles in every lane by position)
    RoadVehicle(double pos):current_state(0,nullptr,0,0,pos,false)
    {
        //Everything is null or default, this vehicle is UNUSABLE
    }

    //@param _lenght length of vehicle in m
    //@max_speed highest speed vehicle can travel at
    //SecondsTo100kmh Seconds it takes the vehicle to reach 100 km/h from full stop, this is a common testing parameter available online for most vehicles
    //BrakingDist100kmh Meters to come to a full stop from 100 km/h, this is a common testing parameter available online for most vehicles
    RoadVehicle(double _length, double _maxSpeed, double SecondsTo100kmh, double BrakingDist100kmh,double time =0,const Road* R=nullptr,int lane=0,double speed=0.0, double pos=0.0/*There is nothing stopping us from spawning vehicles in weird places (though it really should be 0)*/, bool direction=false) noexcept;

    //At what time-point do we need to re-update, return -1 if not further updates
    double nextUpdate() noexcept;




    //Advance until this time
    //@param simulation time in secondes
    //@throw vehicle_past_update_exception if we advance past the next scheduled update
    void setTime(double time);
    double gotoUpdate(double time) noexcept;//Same as the above, but goes exactly to the next scheduled update, returns the new time, does nothing if no new updates exist

    //Drive onto this new road, We assume the road has already accepted us on this location
    //@param Road: what we enter, const, because the roads own the car, not the other way around
    //@param direction: do we drive from first to second node? or the other way around
    //@param lane: what lane do we enter on
    //@param simulation time in secondes
    void enterRoad(const Road* R, bool direction, int lane, double time) noexcept;

    //There will always be one keyframe in the set


    size_t getRoadID() const noexcept;
    int   getLane()    const noexcept;
    double getPos()    const noexcept;
    bool getDirection()const noexcept;
    double getSpeed()  const noexcept;
    double getTime()   const noexcept;


/*
    //mainly for Testing, debugging, all vehicles can tell exactly what road and lane we are on, and where we are on this
    //These always get the end
    size_t getRoadID(double time) const noexcept;
    int   getLane(double time)    const noexcept;
    double getPos(double time)    const noexcept;
    bool getDirection(double time)const noexcept;
    double getSpeed(double time)  const noexcept;
    double getTime(double time)   const noexcept;
*/

    double getLength()       const noexcept{return length;}
    double getMaxSpeed()     const noexcept{return maxSpeed;}
    double getAcceleration() const noexcept{return acceleration;}
    double getBraking()      const noexcept {return braking;}

    //At the current speed, what is the safe seperation distance required to avoid crashes, keep in mind: we do not know the braking acceleration of the vehicle in front, so we will assume it can instantly stop, we still want to have time to come to a full stop with a little margin for reaction time.
    double getSafeDistance() const noexcept
    {

        double braking_time = current_state.speed/braking;

        return /*Distance to break to a stop is = distance to accelerate from stop*/0.5*braking*braking*braking_time+reaction_time_margin*current_state.speed/*Drivers always factor in safety-margin for reaction time, when calculating ideal separation to car ahead*/;
    }

    //In particular used for collision detection: print current state as a string
    std::string toString()const;

    bool operator<(const RoadVehicle& that) const
    {
        return current_state.pos<that.current_state.pos;
    }

    bool operator<(double pos ) const
    {
        return current_state.pos<pos;
    }

};
