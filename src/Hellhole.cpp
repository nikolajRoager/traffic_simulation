#include "Hellhole.hpp"

#include <iostream>

#include "TrafficExceptions.hpp"

#include "Road.hpp"

void Hellhole::addRoad(const Road* R){
    //If there is no pre-existing road

    if (myRoad!=nullptr)
        throw road_address_exception(1,1,getNodeID());
    else if(R==nullptr)
        throw TrafficSimulation_error("Adding NULL road to Node "+std::to_string(getNodeID()));
    else
    {
        //NOTE this only works if addRoad is called AFTER both end and start has been assigned correctly
        myNeighbour =nullptr;
        myRoad =nullptr;

        //If this throws an exception, BOTH myNeighbour and myRoad are null, so if EITHER is nullptr so is the other
        //

        //Get a const Node* since we DO NOT transfer ownership, it exits as a weak pointer, but we can not keep it as such, since myNeighbour need to be initialized as nullptr (if only briefly)
        myNeighbour = &(R->getOther(getNodeID()));
        myRoad=R;

    }
}


const Road &Hellhole::getRoad(size_t roadID, bool local){

    if (local){
        if(roadID!=0 || myRoad==nullptr )
            throw road_address_exception(roadID,1,getNodeID());
    }
    //This IS safe because || ONLY evaluates the right hand side if myRoad!=null
    else if (myRoad==nullptr || myRoad->getRoadID()!=roadID)
        throw road_address_exception(roadID,1,getNodeID());

    //NOT NULL if we got here
    return *myRoad;//As there is only one road, just return tha
}

const Node &Hellhole::getNeighbour(size_t roadID, bool local){
    if (local){
        if(roadID!=0 || myRoad==nullptr )
            throw road_address_exception(roadID,1,getNodeID());
    }
    //This IS safe because || ONLY evaluates the right hand side if myRoad!=null
    else if (myRoad==nullptr || myRoad->getRoadID()!=roadID)
        throw road_address_exception(roadID,1,getNodeID());

    //myRoad is only non null if myNeighbour is not null so if we got here myNeighbour is NOT NULL
    return *myNeighbour;//As there is only one road, just return tha
}
