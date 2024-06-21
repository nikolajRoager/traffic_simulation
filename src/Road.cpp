#include "Road.hpp"
#include "Node.hpp"
#include "json/json.h"
#include<string>
#include<algorithm>
#include<iostream>

//Yanked from https://en.cppreference.com/w/cpp/string/byte/tolower
inline std::string str_tolower(std::string s)
{
    std::transform(s.begin(), s.end(), s.begin(),[](unsigned char c){ return std::tolower(c); });
    return s;
}


using std::string,  std::to_string;

#include "TrafficExceptions.hpp"

Road::Road(size_t _roadID,Json::Value& object,ICityNetwork& City){
    size_t first ;
    size_t second;
    try
    {
    //Not required, and given a default value, no throw guaranteed

        //TEMP, we need to ensure compliance with traffic laws
        roadID=_roadID;
        noOvertake= object.get("noOvertake",true).asBool();
        oneWay= object.get("oneWay",false).asBool();
        lanes  = object.get("lanes",1).asInt();

        if (!object.isMember("type"))
            throw TrafficSimulation_error("Error loading required element in Road "+std::to_string(roadID)+"; \"type\" not found");
        if (!object.isMember("first"))
            throw TrafficSimulation_error("Error loading required element in Road "+std::to_string(roadID)+"; \"first\" not found");
        if (!object.isMember("second"))
            throw TrafficSimulation_error("Error loading required element in Road "+std::to_string(roadID)+"; \"second\" not found");

        string type_name=object.get("type","null").asString();
        first   = object["first"].asLargestUInt();
        second  = object["second"].asLargestUInt();

        if (first==second)
            throw TrafficSimulation_error("Error loading required element in Road "+std::to_string(roadID)+"; \"first\" and \"second\" element must be unique");

        if (str_tolower(type_name).compare("street")==0||str_tolower(type_name).compare("byvej")==0)
        {
            type=street;
        }
        else if (str_tolower(type_name).compare("country road")==0||str_tolower(type_name).compare("landevej")==0)
        {
            type=countryRoad;
        }
        else if (str_tolower(type_name).compare("motortrafficroad")==0||str_tolower(type_name).compare("motortrafikvej")==0)
        {
            type=motortrafficroad;
        }
        else if (str_tolower(type_name).compare("highway")==0||type_name.compare("motorvej")==0)
        {
            type=highway;
        }
        else
            throw TrafficSimulation_error("Error loading required element in Road "+std::to_string(roadID)+"; type does not exist : "+type_name);


        if (first>City.getNodesSize())
            throw TrafficSimulation_error("Error in Road "+std::to_string(roadID)+"; first road ID: "+to_string(first)+" illegal (should be between 0 and "+to_string(City.getNodesSize()));
        else if (second>City.getNodesSize())
            throw TrafficSimulation_error("Error in Road "+std::to_string(roadID)+"; second road ID: "+to_string(second)+" illegal (should be between 0 and "+to_string(City.getNodesSize()));

    }
    catch(Json::Exception& E)
    {
        throw TrafficSimulation_error("Error loading required element in Road "+std::to_string(roadID)+"; Got JSON error: "+E.what());
    }


    //This is our only chance to modify the nodes
    std::shared_ptr<Node> notconst_start=City.getNode(first);
    std::shared_ptr<Node> notconst_end=City.getNode(second);

    start=notconst_start;
    end=notconst_end;
    if (end==nullptr || start==nullptr)
        throw TrafficSimulation_error ("Error in Road "+std::to_string(roadID)+"; null end points");
    //If we get here start and end are NOT NULL, and thus addRoad does not fail

    length =start->getDist(*end);

    notconst_start->addRoad(this);
    notconst_end->addRoad(this);


    //cars_on_lanes[0] is the outer lane
    //For Two-way roads cars_on_lanes[lanes+0] is the outer lane of the other way
    cars_on_lanes=std::vector< std::set< std::shared_ptr<RoadVehicle>,CompareVehicleOnLane > >(oneWay ? lanes : lanes*2);
}

//Get a reference to Node other than This, this is used by the Node when adding Road to verify that the Road they have been married to recognizes them AND for getting their neighbour for quick lookup
//@param this NodeID of the Node we are looking for, I send the ID and not a reference to the node itself, because we might need to call it from the pathfinder which only knows the ID's
//@throw TrafficSimulation_error if This is not one of my ends, or if Start or End is null
const Node& Road::getOther(size_t ThisID) const
{
    //Our constructor guarantees that start end end are NOT nullptr
    if (ThisID == start->getNodeID())
        return *end;
    else if (ThisID == end->getNodeID())
        return *start;
    else
        throw TrafficSimulation_error("Error in Road "+std::to_string(roadID)+"; asking for neighbour Node ID "+std::to_string(ThisID)+" which is not connected to this Road");

}


Road::~Road()
{

}

void Road::addVehicle(int target_lane, bool direction,std::shared_ptr<RoadVehicle> Vehicle,double time)
{
    //Check errors in lane requested, and also get the ID on the list of lanes
    if (target_lane>=lanes)
        throw InvalidLane(roadID,target_lane,lanes);
    if (direction)
    {
        if (oneWay)
            throw InvalidDirection(roadID);

        target_lane+=lanes;
    }

    const RoadVehicle* Next = getFirst_NOCHECK(target_lane);


    if (Next!=nullptr && Vehicle->getLength()<Next->getPos())
    {
        throw CarCrashSpawn(*Vehicle,*Next);
    }

    //Ok now add the vehicle
    cars_on_lanes[target_lane].insert(Vehicle);
    Vehicle->enterRoad(this, direction, target_lane, time);

}

//For accessing vehicles on the road, get the vehicle directly in front of, or directly behind this position, return null if no vehicle present (including if lane/position does not exist)
const RoadVehicle* Road::getNext(int lane, bool direction, double pos)
{
    if (lane>=lanes)
        throw InvalidLane(roadID,lane,lanes);
    if (direction)
    {
        if (oneWay)
            throw InvalidDirection(roadID);

        lane+=lanes;
    }

    return getNext_NOCHECK(lane,pos);
}

const RoadVehicle* Road::getPrev(int lane, bool direction, double pos)
{
    if (lane>=lanes)
        throw InvalidLane(roadID,lane,lanes);
    if (direction)
    {
        if (oneWay)
            throw InvalidDirection(roadID);

        lane+=lanes;
    }

    return getPrev_NOCHECK(lane,pos);
}


const RoadVehicle* Road::getFirst(int lane, bool direction)
{
    if (lane>=lanes)
        throw InvalidLane(roadID,lane,lanes);
    if (direction)
    {
        if (oneWay)
            throw InvalidDirection(roadID);

        lane+=lanes;
    }

    return getFirst_NOCHECK(lane);
}

const RoadVehicle* Road::getLast(int lane, bool direction)
{
    if (lane>=lanes)
        throw InvalidLane(roadID,lane,lanes);
    if (direction)
    {
        if (oneWay)
            throw InvalidDirection(roadID);

        lane+=lanes;
    }

    return getLast_NOCHECK(lane);
}

const inline RoadVehicle* Road::getNext_NOCHECK(int lane_address,double pos)
{
    std::shared_ptr<RoadVehicle> faux_vehicle=std::make_shared<RoadVehicle>(pos);

    const auto Out_iterator = cars_on_lanes[lane_address].upper_bound(faux_vehicle);

    //end() is used to indicate a failed search, if not failed the iterator points to a valid member of the set, and we can call get on the underlying shared pointer to get the pointer, which we cast to const.
    return Out_iterator==  cars_on_lanes[lane_address].end() ? nullptr : static_cast<const RoadVehicle*>(Out_iterator->get());
}

const inline RoadVehicle* Road::getPrev_NOCHECK(int lane_address,double pos)
{

    std::shared_ptr<RoadVehicle> faux_vehicle=std::make_shared<RoadVehicle>(pos);

    const auto Out_iterator = cars_on_lanes[lane_address].lower_bound(faux_vehicle);

    //end() is used to indicate a failed search, if not failed the iterator points to a valid member of the set, and we can call get on the underlying shared pointer to get the pointer, which we cast to const.
    return Out_iterator==  cars_on_lanes[lane_address].end() ? nullptr : static_cast<const RoadVehicle*>(Out_iterator->get());


}
