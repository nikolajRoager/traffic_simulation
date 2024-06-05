#include "Hellhole.hpp"



//Add a new road, or set a particular road
//@param R the road to add or set
//@param i the id to set it at, for some intersections this effects transfer speeds
//@throw road_address_exception
void Hellhole::addRoad(std::shared_ptr<Road> R){
    if (myRoad==nullptr)
        myRoad=R;
    else
    {
        //STUB, THROW ERROR
    }
}

void Hellhole::setRoad(int i,std::shared_ptr<Road> R){
    if (i==0)
    {
        myRoad=R;
    }
    else
    {
        //STUB, THROW ERROR
    }
}

//@param roadID the roadID of the road we are looking for
//@param localID Use the ID in the list of roads of this node instead (0 to RoadNumber) the latter is more uesful for pathfinding
//@throw road_address_exception on illegal roadID
//Get a const pointer to the road with this roadID
std::shared_ptr<const Road> Hellhole::getRoad(int /*roadId*/, bool /*local*/){
    //STUB
    //SHOULD DO CHECK THAT THE ARGUMENT IS LEGAL

    return myRoad;//As there is only one road, just return tha
}

