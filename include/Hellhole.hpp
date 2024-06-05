#pragma once
#include "json/json.h"

#include <memory>//Shared pointers
#include "Node.hpp"

/**
* Hellholes, are a special type of nodes, with only one road.
* They are only useful for debugging and testing.
* When a car drives into a hellhole, it disappears from the simulation
*/

class Road;//We don't need to use any members of road in this header file

class Hellhole: public Node{
private:


    std::shared_ptr<Road> myRoad=nullptr;

public:
    //Load, without loading the roads (they get loaded later, and then they are matched to the nodes)
    //@param ID the nodeID of this node
    Hellhole(int ID) noexcept :Node(ID),myRoad(nullptr){};

    //We inherit this function from our parent class
    //int getNodeID() const noexcept {return nodeID;}

    //Get number of roads, and max legal number of roads
    virtual int getRoadNumber() const noexcept {return myRoad==nullptr ?  0 : 1;}
    virtual int getMaxRoadNumber() const noexcept {return 1;}

    //Add a new road, or set a particular road
    //@param R the road to add or set
    //@param i the id to set it at, for some intersections this effects transfer speeds
    //@throw road_address_exception
    virtual void addRoad(std::shared_ptr<Road> R);
    virtual void setRoad(int i,std::shared_ptr<Road> R);

    //@param roadID the roadID of the road we are looking for
    //@param localID Use the ID in the list of roads of this node instead (0 to RoadNumber) the latter is more uesful for pathfinding
    //@throw road_address_exception on illegal roadID
    //Get a const pointer to the road with this roadID
    virtual std::shared_ptr<const Road> getRoad(int roadId, bool local);

};
