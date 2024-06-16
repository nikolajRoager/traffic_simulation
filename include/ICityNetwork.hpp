#pragma once
#include "json/json.h"

#include <memory>
#include "Node.hpp"
#include "Road.hpp"

/**
* An interface for the city network wrapper class, which contains all the roads and noads, and are used to talk with them
* the Roads and Nodes should not depend on the underlying container used to store them, they should ask the wrapper
* This interface is used for mocking the city network
*/

class ICityNetwork
{

public:

    virtual size_t getNodesSize() const noexcept =0;
    virtual size_t getRoadsSize() const noexcept =0;

    //Expected to throw address errors when out of bounds
    virtual std::shared_ptr<Node> getNode(size_t NodeID)=0;
    virtual std::shared_ptr<Road> getRoad(size_t RoadD)=0;
};
