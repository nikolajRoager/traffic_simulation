#pragma once

#include <memory>
#include <vector>
#include <istream>
#include "Node.hpp"
#include "Road.hpp"
#include "ICityNetwork.hpp"

#include "TrafficExceptions.hpp"

/**
*The basic class loading and storing roads and nodes
*/

class CityNetwork : public ICityNetwork
{
private:
    //List of Nodes and Roads we can address
    std::vector<std::shared_ptr<Node> > Nodes;
    std::vector<std::shared_ptr<Road> > Roads;

    //HERE IS WHY THIS IS A std::vector and NOT ANY OTHER CONTAINER
    //We want a sequence container (std::array, std::vector, std::deque, std::list or std::forward_list), which is resizable (precluding std::array)
    //I would like easy random access with the [] operator (precluding linked lists)
    //I also like beeing able to insert in the middle or end (precluding deque)
    //I will not do a lot of insertion, the main thing I am doing is addressing the elements
    //All this makes the std::vector the best option.

    //Store sizes for easy lookup
    size_t nodeSize;
    size_t roadSize;


public:

    CityNetwork(std::istream& CityNetworkJsonStream);

    virtual size_t getNodesSize() const noexcept
    {
        return Nodes.size();
    }

    virtual size_t getRoadsSize() const noexcept
    {
        return Roads.size();
    }

    //Expected to throw address errors when out of bounds
    virtual std::shared_ptr<Node> getNode(size_t NodeID)
    {
        if(NodeID>=nodeSize)
            throw node_address_exception(NodeID,nodeSize);
        return Nodes[NodeID];
    }
    virtual std::shared_ptr<Road> getRoad(size_t RoadID)
    {
        if(RoadID>=nodeSize)
            throw road_address_exception(RoadID,roadSize);
        return Roads[RoadID];
    }
};
