#pragma once
#include "json/json.h"

#include <memory>//Shared pointers

/**
* Nodes are connected by differing numbers of roads (depending on the type of node)
* Nodes can serve as end-points for journeys, or connect other roads
*/

class Road;//We don't need to use any members of road in this header file

class Node{
protected:
    int nodeID;//A unique ID for this node (the index in the node list), used to speed up the pathfinding algorithm


public:
    //Load, without loading the roads (they get loaded later, and then they are matched to the nodes)
    //@param ID the nodeID of this node
    Node(int ID) noexcept : nodeID(ID){};

    int getNodeID() const noexcept {return nodeID;}



    //Get number of roads, and max legal number of roads
    virtual int getRoadNumber() const noexcept = 0;
    virtual int getMaxRoadNumber() const noexcept = 0;

    //Add a new road, or set a particular road
    //@param R the road to add or set
    //@param i the id to set it at, for some intersections this effects transfer speeds
    //@throw road_address_exception
    virtual void addRoad(std::shared_ptr<Road> R) =0;
    virtual void setRoad(int i,std::shared_ptr<Road> R) =0;

    //@param roadID the roadID of the road we are looking for
    //@param localID Use the ID in the list of roads of this node instead (0 to RoadNumber) the latter is more uesful for pathfinding
    //@throw road_address_exception on illegal roadID
    //Get a const pointer to the road with this roadID
    virtual std::shared_ptr<const Road> getRoad(int roadId, bool )=0;


    //CORRECTION, THIS SHOULD BE MOVED TO AN INTERSECTION SUBCLASS, IT DOES NOT MAKE SENSE FOR END-NODES
    //How long does it (ideally: with no traffic) take to transit from this one road to this other (indexed by RoadID, not the number they have in the road).
    //@param roadID the roadID of the road we are looking for
    //@param localID Use the ID in the list of roads of this node instead (0 to RoadNumber) the latter is more uesful for pathfinding
    //@throw a road_address_exception on illegal roadID
    //virtual double idealTransferTime(int road0id,int road1id)=0;
};
