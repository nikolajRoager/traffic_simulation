#include "Road.hpp"
#include "Node.hpp"
#include "json/json.h"
#include<string>
#include<vector>

using std::string, std::vector, std::to_string;

#include "TrafficExceptions.hpp"

Road::Road(size_t _roadID,Json::Value& object,vector<std::shared_ptr<Node> > Nodes){
    //Not required, and given a default value, no throw guaranteed
    roadID=_roadID;
    noOvertake= object.get("noOvertake",true).asBool();
    oneWay= object.get("oneWay",false).asBool();
    lanes  = object.get("lanes",1).asInt();

    size_t first ;
    size_t second;
    try
    {
        string type_name=object["type"].asString();
        first   = object["first"].asLargestUInt();
        second  = object["second"].asLargestUInt();
    }
    catch(Json::Exception& E)
    {
        throw city_loading_exception("Error loading required element in Road "+std::to_string(roadID)+"; Got JSON error: "+E.what());
    }
    if (first>Nodes.size())
        throw city_loading_exception("Error in Road "+std::to_string(roadID)+"; first road ID: "+to_string(first)+" illegal (should be between 0 and "+to_string(Nodes.size()));
    else if (second>Nodes.size())
        throw city_loading_exception("Error in Road "+std::to_string(roadID)+"; second road ID: "+to_string(second)+" illegal (should be between 0 and "+to_string(Nodes.size()));


    start=Nodes[first];
    end=Nodes[second];


    if (end==nullptr || start==nullptr)
        throw city_loading_exception("Error in Road "+std::to_string(roadID)+"; null end points");

    //If we get here start and end are NOT NULL


    //We call this on Nodes[first] and Nodes[second] because start and end are const, and this function is not const

    Nodes[first]->addRoad(this);
    Nodes[second]->addRoad(this);
}

//Get a reference to Node other than This, this is used by the Node when adding Road to verify that the Road they have been married to recognizes them AND for getting their neighbour for quick lookup
//@param this NodeID of the Node we are looking for, I send the ID and not a reference to the node itself, because we might need to call it from the pathfinder which only knows the ID's
//@throw city_loading_exception if This is not one of my ends, or if Start or End is null
const Node& Road::getOther(size_t ThisID) const
{
    //Our constructor guarantees that start end end are NOT nullptr
    if (ThisID == start->getNodeID())
        return *end;
    else if (ThisID == end->getNodeID())
        return *start;
    else
        throw city_loading_exception("Error in Road "+std::to_string(roadID)+"; asking for neighbour of "+std::to_string(ThisID)+" which is not connected to this Road");

}
