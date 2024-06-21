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

#define KMH_TO_MS (1000.0/(60.0*60.0))


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


//A scandinavian stationcar please
RoadVehicle CreateVolvoV60() noexcept
{
    //From https://www.media.volvocars.com/ca/en-ca/models/v60/2021/specifications
    return RoadVehicle(4.635 , 180/*idk how much this is, couldn't get a source, but it is above all speed limits so whatever*/, 6.7, 35);
}


//Verify that this example car has the expected stats
TEST(Test_Vehicle, Make_Volvo_offroad) {
    RoadVehicle VolvoV60 = CreateVolvoV60();
    ASSERT_NEAR(VolvoV60.getLength(),4.635,tolerance);
    ASSERT_NEAR(VolvoV60.getMaxSpeed(),180,tolerance);

    //Verify that it takes 6.7 seconds to reach 100 km/h


    ASSERT_NEAR(VolvoV60.getAcceleration()*6.7,100*KMH_TO_MS,tolerance);

    ////Verify that braking at braking acceleration from 100 km/h takes 35 m to stop
    double time_to_stop = 100*KMH_TO_MS/VolvoV60.getBraking();

    ASSERT_NEAR(0.5*time_to_stop*time_to_stop*VolvoV60.getBraking(),35,tolerance);

    //We should not have set a road, so everything should be default
    ASSERT_EQ(VolvoV60.getRoadID(),-1);
    ASSERT_EQ(VolvoV60.getLane(),0);
    ASSERT_EQ(VolvoV60.getPos(),0);
    ASSERT_EQ(VolvoV60.getDirection(),false);
    ASSERT_EQ(VolvoV60.getSpeed(),0);
    ASSERT_EQ(VolvoV60.getTime(),0);
}

//Drive a Volvo on a road
TEST(Test_Vehicle, drive_Volvo) {
    RoadVehicle VolvoV60 = CreateVolvoV60();
    ASSERT_NEAR(VolvoV60.getLength(),4.635,tolerance);
    ASSERT_NEAR(VolvoV60.getMaxSpeed(),180,tolerance);

    //Verify that it takes 6.7 seconds to reach 100 km/h


    ASSERT_NEAR(VolvoV60.getAcceleration()*6.7,100*KMH_TO_MS,tolerance);

    ////Verify that braking at braking acceleration from 100 km/h takes 35 m to stop
    double time_to_stop = 100*KMH_TO_MS/VolvoV60.getBraking();

    ASSERT_NEAR(0.5*time_to_stop*time_to_stop*VolvoV60.getBraking(),35,tolerance);

    //We should not have set a road, so everything should be default
    ASSERT_EQ(VolvoV60.getRoadID(),-1);
    ASSERT_EQ(VolvoV60.getLane(),0);
    ASSERT_EQ(VolvoV60.getPos(),0);
    ASSERT_EQ(VolvoV60.getDirection(),false);
    ASSERT_EQ(VolvoV60.getSpeed(),0);
    ASSERT_EQ(VolvoV60.getTime(),0);
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
