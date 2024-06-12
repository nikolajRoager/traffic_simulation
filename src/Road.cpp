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
    //If we get here start and end are NOT NULL, and thus addRoad does not fail

    notconst_start->addRoad(this);
    notconst_end->addRoad(this);


    if (end==nullptr || start==nullptr)
        throw TrafficSimulation_error ("Error in Road "+std::to_string(roadID)+"; null end points");

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
