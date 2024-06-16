#pragma once
#include "json/json.h"

#include <memory>//Shared pointers

/**
* Nodes are connected by differing numbers of roads (depending on the type of node)
* Nodes can serve as end-points for journeys, or connect other roads

WARNING
IT IS EXPECTED THAT ALL ROADS AND NODES ARE DELETED AT THE SAME TIME, CALLING MEMBER FUNCTIONS AFTER DELETING THE NODES OR ROADS BUT NOT THE OTHER WILL RESULT IN UNDEFINED BEHAVIOR.

THIS IS INTENTIONAL, IMPLEMENTING METHODS TO HANDLE IT WOULD REQUIRE THE NODES AND ROADS TO STORE NON-CONST REFERENCES TO EACH OTHER, WHICH WE SHOULD NOT DO.
*/

class Road;//We don't need to use any members of road in this header file

class Node{
    //Derived classes should only modify this during the constructor
private:
    size_t nodeID;//A unique ID for this node (the index in the node list), used to speed up the pathfinding algorithm

    //Position on 2D map in meters
    double x;
    double y;

public:
    //Load, without loading the roads (they get loaded later, and then they are matched to the nodes)
    //@param ID the nodeID of this node
    Node(size_t ID,double _x, double _y) noexcept : nodeID(ID),x(_x),y(_y){};

    double getDist(const Node& Other)const;


    //Checks all own

    size_t getNodeID() const noexcept {return nodeID;}



    //Get number of roads, and max legal number of roads
    virtual size_t getRoadNumber() const noexcept = 0;
    virtual size_t getMaxRoadNumber() const noexcept = 0;

    /*Add a new road, or set a particular road
    *We use a raw const Road pointer in this case, because the road starts unitialized (nullptr) ruling out references.
    *And smart pointers would go in a circle
    *
    *@param R the road to add or set. This is NOT a shared or unique pointer, because the road alraedy has a shared pointer to the Node, which would result in a circular reference, and a weak_ptr can not easilly be created from inside the constructor of the Road (which is where add_road is called from). A const Road* is perfectly safe, since we can not delete it from this Node.
    *@param i the id to set it at, for some intersections this effects transfer speeds
    *@throw road_address_exception
    */
    virtual void addRoad(const Road* R) =0;


    /*Get a const reference to the road with this roadID
    *@param roadID the roadID of the road we are looking for
    *@param localID Use the ID in the list of roads of this node instead (0 to RoadNumber) the latter is more uesful for pathfinding
    *@throw road_address_exception on illegal roadID (including road not loaded)*/
    virtual const Road &getRoad(size_t roadId, bool local=false)=0;




    /*Get a const reference to the neighbour at the end of this roadID
    *@param roadID the roadID of the road we are looking for
    *@param localID Use the ID in the list of roads of this node instead (0 to RoadNumber) the latter is more uesful for pathfinding
    *@throw road_address_exception on illegal roadID (including road not loaded)
    *Get a const pointer to the Neighbour of this Node at the end of this road*/
    virtual const Node &getNeighbour(size_t roadId, bool local=false)=0;


    //CORRECTION, THIS SHOULD BE MOVED TO AN INTERSECTION SUBCLASS, IT DOES NOT MAKE SENSE FOR END-NODES
    //How long does it (ideally: with no traffic) take to transit from this one road to this other (indexed by RoadID, not the number they have in the road).
    //@param roadID the roadID of the road we are looking for
    //@param localID Use the ID in the list of roads of this node instead (0 to RoadNumber) the latter is more uesful for pathfinding
    //@throw a road_address_exception on illegal roadID
    //virtual double idealTransferTime(size_t road0id,size_t road1id)=0;
};
