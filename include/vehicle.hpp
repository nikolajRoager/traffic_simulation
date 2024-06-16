#pragma once
#include "json/json.h"

#include <memory>//Shared pointers

/**
* Vehicles are the base class for all road vehicles (Bicycles and Trains are not derived from that, as they do not need to fit on the road)
*
*These are all the things ALL road vehicles need to talk with the roads and nodes
*/

class Vehicle
{
private:
    double length;
public:
    Vehicle();


    //For waiting in queues, width doesn't matter
    double getLength() const {return length;}

};

