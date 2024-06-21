#pragma once

#include<exception>
#include<string>
#include<vector>

//A collection of errors, related to the traffic simulation


#include "RoadVehicle.hpp"


//Used as a parent class for all MY errors, also used as a generic error message
class TrafficSimulation_error : public std::exception
{
private:
    //Declare the message here, as a C++ string, that way the c_str() will live as long as the object itself
    std::string msg;

protected:
//In the event our derived classes need to add things after constructions (i.e. it need to loop through an array, or run some calculation)
    void addend(const std::string& S)
    {
        msg+=S;
    }
public:
    TrafficSimulation_error (std::string _msg) : msg(_msg){}
    char const* what() const noexcept
    {
        //Pointer remains ok for the duration of the error handling
        return msg.c_str();
    };
};




class vehicle_past_update_exception: public TrafficSimulation_error{
public:
    vehicle_past_update_exception(int vehicleID, double requested_time, double next_update) noexcept : TrafficSimulation_error ("Vehicle ID "+std::to_string(vehicleID)+" asked to update until "+std::to_string(requested_time)+", but has updated queued for "+std::to_string(next_update)){}
};



class road_address_exception: public TrafficSimulation_error
{
public:
    road_address_exception(int roadID, int maxRoads) noexcept : TrafficSimulation_error("Asked for global Road "+std::to_string(roadID)+"  should be less than "+std::to_string(maxRoads)){}

    road_address_exception(int roadID, int maxRoads, int nodeID) noexcept : TrafficSimulation_error("Asked for local Road "+std::to_string(roadID)+" should be less than "+std::to_string(maxRoads)+" at Node "+std::to_string(nodeID)){}

    //This is called when we asked for a roadID not in the legal road IDs
    road_address_exception(int roadID, std::vector<int> legalRoadID, int nodeID)noexcept : TrafficSimulation_error("Asked for global Road ID "+std::to_string(roadID)+" at Node "+std::to_string(nodeID)+" valid roads are:")
    {
        for (int i : legalRoadID)
            TrafficSimulation_error::addend(' '+std::to_string(i));
    }
};

class node_address_exception: public TrafficSimulation_error
{
public:
    node_address_exception(int nodeID, int maxNodes) noexcept : TrafficSimulation_error("Asked for Node with ID"+std::to_string(nodeID)+" should be less than "+std::to_string(maxNodes)){}
};


class InvalidRoadAddress: public TrafficSimulation_error
{
public:
    InvalidRoadAddress(int roadID, const std::string& reason) noexcept : TrafficSimulation_error("Illegal location on road "+std::to_string(roadID)+" : "+ reason){}
};


class InvalidLane: public InvalidRoadAddress
{
public:
    InvalidLane(int roadID,int lane, int maxlane) noexcept : InvalidRoadAddress(roadID, " illegal lane "+std::to_string(lane)+" , must be between 0 and "+std::to_string(maxlane)){}
};


class InvalidPos: public InvalidRoadAddress
{
public:
    InvalidPos(int roadID,double pos, double length) noexcept : InvalidRoadAddress(roadID, " illegal position "+std::to_string(pos)+" m , must be between 0 m and "+std::to_string(length)+" m"){}
};


class InvalidDirection: public InvalidRoadAddress
{
public:
    InvalidDirection(int roadID) noexcept : InvalidRoadAddress(roadID, " illegal direction, asked for return lane on one-way road"){}
};


//These two cars crashed, thus crashing the simulation
class CarCrash: public TrafficSimulation_error
{
public:
    CarCrash(const RoadVehicle& front, const RoadVehicle& rear) noexcept : TrafficSimulation_error("Car collision, this car: "+front.toString()+" drove into the rear of this car: "+rear.toString()){}
};

//These two cars crashed, thus crashing the simulation
class CarCrashSpawn: public TrafficSimulation_error
{
public:
    CarCrashSpawn(const RoadVehicle& front, const RoadVehicle& rear) noexcept : TrafficSimulation_error("Car collision, this car: "+front.toString()+" attempted to spawn inside this car: "+rear.toString()+", note: current stats of the spawned cars may not have been updated when this error was thrown"){}
};


