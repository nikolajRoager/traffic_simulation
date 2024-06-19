#include <gtest/gtest.h>

#include <memory>
#include <vector>
#include <string>
#include <sstream>

#include "Hellhole.hpp"
#include "TrafficExceptions.hpp"
#include "Road.hpp"
#include "RoadVehicle.hpp"
#include "CityNetwork.hpp"

#define tolerance 1e-8



CityNetwork CreateMockCity()
{
    std::string JSon_City_String(
    "{\n\
        \"nodes\":\n\
        [\n\
            {\n\
                \"type\":\"Hellhole\",\n\
                \"pos\":[-1000, -1000]\n\
            },\n\
            {\n\
                \"type\":\"Hellhole\",\n\
                \"pos\":[3000, -3000]\n\
            }\n\
        ],\n\
        \"roads\":\n\
        [\n\
            {\n\
                \"type\":\"Motortrafikvej\",\n\
                \"first\":0,\n\
                \"second\":1,\n\
                \"lanes\":3,\n\
                \"oneWay\":false,\n\
                \"noOvertake\":true\n\
            }\n\
        ]\n\
    }"
   );
    std::stringstream S(JSon_City_String);
    return CityNetwork(S);
}

TEST(Test_Vehicle, Add_VehicleToRoad) {

    CityNetwork Mocktown = CreateMockCity();

    car Volvo();

 //   CreateMockCity()

}

/*
Tests:
Add a car to a road, advance time and verify that the car has the expected location
Adding a car to the road, should have both the car and the road associated with each other.
verify that exceptions are thrown when "overstepping" updates for car
The number of cars exiting the road, should match the number of cars entering the road
At the end, the car should know that it is not on the road anymore.
The keyframe JSON file must have the expected keyframes
*/
