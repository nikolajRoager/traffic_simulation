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

    //We NEED to use pointers in this case, we CAN NOT use references or week_pointers, because these need to be undefined while we do the loading,  so we NEED pointers, and unique_ptr and shared_ptr would lead to circular ownership
    const Road *myRoad=nullptr;
    const Node *myNeighbour=nullptr;//Only one neighbour

public:
    //Load, without loading the roads (they get loaded later, and then they are matched to the nodes)
    //@param ID the nodeID of this node
    Hellhole(size_t ID) noexcept :Node(ID),myRoad(nullptr){};

    //We inherit this function from our parent class
    //int getNodeID() const noexcept {return nodeID;}

    //Get number of roads, and max legal number of roads
    virtual size_t getRoadNumber() const noexcept {return myRoad==nullptr ?  0 : 1;}
    virtual size_t getMaxRoadNumber() const noexcept {return 1;}


    /*Add a new road, or set a particular road
    *We use a raw const Road pointer in this case, because the road starts unitialized (nullptr) ruling out references.
    *And smart pointers would go in a circle
    *
    *@param R the road to add or set. This is NOT a shared or unique pointer, because the road alraedy has a shared pointer to the Node, which would result in a circular reference, and a weak_ptr can not easilly be created from inside the constructor of the Road (which is where add_road is called from). A const Road* is perfectly safe, since we can not delete it from this Node.
    *@param i the id to set it at, for some intersections this effects transfer speeds
    *@throw road_address_exception
    */
    virtual void addRoad(const Road *R);




    /*Get a const pointer to the road with this roadID
    *@param roadID the roadID of the road we are looking for
    *@param localID Use the ID in the list of roads of this node instead (0 to RoadNumber) the latter is more uesful for pathfinding
    *@throw road_address_exception on illegal roadID (including road not loaded)*/
    virtual const Road &getRoad(size_t roadId, bool local=false);

    /*Get a const pointer to the neighbour at the end of this roadID
    *@param roadID the roadID of the road we are looking for
    *@param localID Use the ID in the list of roads of this node instead (0 to RoadNumber) the latter is more uesful for pathfinding
    *@throw road_address_exception on illegal roadID (including road not loaded)
    *Get a const pointer to the Neighbour of this Node at the end of this road*/
    virtual const Node &getNeighbour(size_t roadId, bool local=false);


};
