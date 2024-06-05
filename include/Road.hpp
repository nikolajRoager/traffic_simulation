#pragma once
#include "json/json.h"

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


    int roadID;//A unique ID for this road (the index in the road list), used to speed up the pathfinding algorithm

    //Node start;
    //Node end;


    RoadType type;


    bool no_overtake;
    bool one_way;
    int lanes;

    //For telling if a car has driven of the end, the length of the road from one end to another
    float length;


public:
    //@throws city_loader_errors
    Road(Json::Value& val);

    //For the pathfinding algorithm, get the ID
    int getRoadID() const noexcept {return roadID;}
};
