#include "CityNetwork.hpp"
#include "ICityNetwork.hpp"
#include "Hellhole.hpp"

CityNetwork::CityNetwork(std::istream& CityNetworkJsonStream)
{
    Nodes=std::vector<std::shared_ptr<Node> >();
    Roads=std::vector<std::shared_ptr<Road> >();
    try
    {
        Json::Value root;
        CityNetworkJsonStream>>root;


        if (!root.isMember("nodes"))
            throw TrafficSimulation_error(std::string("Error loading City Network; JSON did not contain nodes:"));
        if (!root.isMember("roads"))
            throw TrafficSimulation_error(std::string("Error loading City Network; JSON did not contain roads:"));

        Json::Value NodesJson=root["nodes"];
        Json::Value RoadsJson=root["roads"];

        int id = 0;
        for (Json::Value& V : NodesJson)
        {
            if (!V.isMember("type"))
                throw TrafficSimulation_error(std::string("Error loading City Network; Node without type"));
            if (!V.isMember("pos"))
                throw TrafficSimulation_error(std::string("Error loading City Network; Node without position"));

            std::string Type = V["type"].asString();
            Json::Value& Pos = V["pos"];

            //TEMP insert more checks

            if (Type.compare("Hellhole")==0)
            {
                Nodes.push_back(std::make_shared<Hellhole>(id++,Pos[0].asInt(),Pos[1].asInt()));
            }
            else
                throw TrafficSimulation_error(std::string("Error loading City Network; no Node type: ")+Type);


        }


        id=0;
        for (Json::Value& V : RoadsJson)
            Roads.push_back(std::make_shared<Road>(id++,V,*this));
    }
    catch(Json::Exception& E)
    {
        throw TrafficSimulation_error(std::string("Error loading City Network; Got JSON error: ")+E.what());
    }

    nodeSize=Nodes.size();
    roadSize=Roads.size();
}

