#pragma once
#include "json/json.h"
#include "ICityNetwork.hpp"
#include <memory>//Shared pointers
#include <set>
#include <vector>


#include "RoadVehicle.hpp"

class Node;//We don't need to know the details of the Node class in this header file

/**
* Roads are used by road vehicles (for the purpose of this simulations, streets are also considered part of the road class)
*/


//TEMP, move this to a LAW class
//Different types of roads all have exactly the same methods, so it is not worth using derived classes, instead this enum tells us what traffic laws to load
/*This is largely based on Danish designations, streets (byvej) are inside urban areas and have low speed limits (50 km/h in most cases) their main purpose is connecting houses together, traffic signals are generally not required; country roads (landevej) are outside major cities, they are both used for relatively fast transportation (80 km/h) and also to connect farmers to their fields, given their rural nature they generally don't use traffic signals (roundabouts are common); Motortrafficroad (motortrafikvej has no good English translation) are exclusively designed for high throughput between cities at speeds often at 90 km/h (In Denmark, some roads inside cities are legally designated as country roads, effectively functioning as intra-city motortrafficroads) they never connect to buildings or fields, and generally use traffic light, multiple lanes are common and overtaking in the opposing lane is not legal (and often impossible),  it is NOT legal to bike or walk directly at the side of these roads (unless dedicated paths are installed); Highways (motorvej/Europavej) are the EU highway network with speed limits of 130 km/h. They function very similarly to motortrafficroads, except they don't have signal-lights, relying instead on high speed on and off-ramps.

In practice, the limit between classes is oft blurry, motortrafficroads are very much a spectrum between country roads and highways.

*/
enum RoadType: int {street=0,countryRoad,motortrafficroad,highway};


class Road{
private:


    size_t roadID;//A unique ID for this road (the index in the road list), used to speed up the pathfinding algorithm

    //Guaranteed NOT NULL after the constructor
    //I still use shared pointers, this way they will not risk accidentally deleting the pointers
    std::shared_ptr<const Node> start;
    std::shared_ptr<const Node> end;


    RoadType type;


    bool noOvertake;
    bool oneWay;
    int lanes;

    //For telling if a car has driven of the end, the length of the road from one end to another
    float length;


    //One set of cars per lane (2*lanes if we go both ways), sorted by position on the road (0 is always the end)
    //We use a set for each lane, because we want to access the next and before car based on position, we want to insert and remove cars at certain positions when overtaking, this is the comparison we will use
    struct CompareVehicleOnLane
    {

        bool operator()(const std::shared_ptr<RoadVehicle>& left , const std::shared_ptr<RoadVehicle>& right) const {
            return *left < *right;
        }
    };


    //BEWARE: if cars drive through their neighbours, we will encounter undefined behaviour, so DO NOT LET THAT HAPPEN
    //We use a VECTOR as the container for each lanes, because it is the fastest for random access, and we will not insert or delete lanes during the simulation
    //We use shared pointers, not to take over ownership, but because we want a reference which can access non-const member functions (ruling out const pointers), which does not allow deletion of the element pointed to (ruling out non-const pointers), and which can work in a set (for some stupid reason C++ references don't allow that)
    std::vector< std::set<std::shared_ptr<RoadVehicle>,CompareVehicleOnLane > > cars_on_lanes;



    //INTERNAL version of get next, where we have already checked that the address is legal, and translated it into a single lane_address (which is lane+lanes if going reverse direction). This SKIPS boundary checks, ASSUMING that we have already done it
    //@param lane_address is the lane we are looking for when going normal direction, and lane+lanes when going the other direction
    //Returned as a const pointer because it can be null, doesn't give ownership of the Vehicle, and doesn't allow deletion or modification. If a vehicle exist EXACTLY at pos, it is returned by both
    const inline RoadVehicle* getNext_NOCHECK(int lane_address,double pos) ;
    const inline RoadVehicle* getPrev_NOCHECK(int lane_address,double pos) ;

    //Same, but get the first or last car on the road, slightly faster
    const inline RoadVehicle* getFirst_NOCHECK(int lane_address)
    {
        return cars_on_lanes[lane_address].empty() ? nullptr : static_cast<const RoadVehicle*>(cars_on_lanes[lane_address].begin()->get());
    }
    const inline RoadVehicle* getLast_NOCHECK(int lane_address)
    {
        return cars_on_lanes[lane_address].empty() ? nullptr : static_cast<const RoadVehicle*>(cars_on_lanes[lane_address].rbegin()->get());
    }



public:
    //@throws city_loader_errors
    Road(size_t _roadID,Json::Value& object,ICityNetwork& City/*Not const, as we will be updating the nodes we connect to*/);

    ~Road();

    //For the pathfinding algorithm, get the ID
    size_t getRoadID() const noexcept {return roadID;}


    //Get a reference to Node other than This, this is used by the Node when adding Road to verify that the Road they have been married to recognizes them AND for getting their neighbour for quick lookup
    //@param this NodeID of the Node we are looking for, I send the ID and not a reference to the node itself, because we might need to call it from the pathfinder which only knows the ID's
    //@throw TrafficSimulation_error if This is not one of my ends, or if Start or End is null
    const Node& getOther(size_t ThisID) const;

    //Add a vehicle to the road RIGHT NOW, this is not a reservation of a slot in the future.
    //@param lane the lane we enter into
    //@param direction are we going from first to second (false) or the other way
    //@param vehicle: a reference of the vehicle to add, WE will update the vehicle accordingly
    //@param time: The time at which the vehicle is added, presumably now in terms of the simulation
    //@throw InvalidRoadAddress if the vehicle is inserted onto an existing vehicle (since there is no negotiating for slots, this is the only option)
    void addVehicle(int _lane, bool direction,std::shared_ptr<RoadVehicle> Vehicle,double time);

    //For accessing vehicles on the road, get the vehicle directly in front of, or directly behind this position, return null if no vehicle present (including if lane/position does not exist). If a vehicle exist EXACTLY at pos, it is returned by both
    //@param lane the lane we are requesting
    //@param direction the direction lane we are requesting
    //@param pos the position we want to get the next or previous vehicle from
    //@throw InvalidRoadAddress if the vehicle is inserted onto an existing vehicle (since there is no negotiating for slots, this is the only option)
    //Returned as a const pointer because it can be null, doesn't give ownership of the Vehicle, and doesn't allow deletion or modification
    const RoadVehicle* getNext(int lane, bool direction, double pos) ;
    const RoadVehicle* getPrev(int lane, bool direction, double pos) ;
    //Same, but get first or last vehicle on the road (even faster), throws same exceptions
    const RoadVehicle* getFirst(int lane, bool direction) ;
    const RoadVehicle* getLast(int lane, bool direction) ;



    //Mainly for Testing that the JSon file is loaded correctly
    RoadType getType() const{return type;}
    bool getNoOvertake() const{return noOvertake;}
    bool getOneWay() const{return oneWay;}
    int  getLanes() const{return lanes;}
    double getLength() const{return length;}
};
