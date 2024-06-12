#include <gtest/gtest.h>
#include <memory>
#include <vector>
#include <string>

#include "Hellhole.hpp"
#include "TrafficExceptions.hpp"
#include "Road.hpp"


TEST(Test_Loading, Add_Null_road_to_Node_or_query_bad_address) {
    std::shared_ptr<Node> A = std::make_shared<Hellhole>(0);

    ASSERT_THROW(A->addRoad(nullptr),TrafficSimulation_error);

    ASSERT_THROW(A->getRoad(0), road_address_exception);
    ASSERT_THROW(A->getRoad(1), road_address_exception);
    ASSERT_THROW(A->getRoad(0,true), road_address_exception);
    ASSERT_THROW(A->getRoad(0,false), road_address_exception);
}



Json::Value to_Json(const std::string& text)
{

    Json::Value root;
    Json::Reader reader;
    bool parsingSuccessful = reader.parse( text, root );

    if (!parsingSuccessful)
    //Oh no, the test failed ... no not like that, the test itself failed.
    {
        //GTEST will catch this error and report the test as failed
        throw std::runtime_error("Error trying to set up test, the test JSON snippet was invalid ");
    }
    return root;
};

//A faux city network, with the bare minimum functions to trick the Road class into letting me test it
struct faux_CityNetwork : public ICityNetwork
{
    //This DOES NOT make sure ID's are never reused
    std::shared_ptr<Node> A = std::make_shared<Hellhole>(0);
    std::shared_ptr<Node> B = std::make_shared<Hellhole>(1);

    //I am not going to test what happens ifthe CityNetwork LIES about the size
    size_t getNodesSize() const noexcept {return 2;}
    size_t getRoadsSize() const noexcept {return 0;}

    //Expected to throw address errors when out of bounds
    virtual std::shared_ptr<Node> getNode(size_t NodeID)
    {
        switch (NodeID)
        {
            case 0: return A;
            case 1: return B;
            default: throw road_address_exception(NodeID, 2);
        }
    }
    virtual std::shared_ptr<Road> getRoad(size_t RoadID)
    {
        //STUB, won't be used for this test
        throw road_address_exception(RoadID, 0);
    }

};

TEST(Test_Loading, Initialize_Road_with_invalid_JSON) {


    //Get a new mock city interface each time, to not carry over any changes
    faux_CityNetwork mockTown0;
    Json::Value RoadRoot_missing_type=to_Json(
                        "{\n \
                          \"first\"     :0,\n \
                          \"second\"    :1,\n \
                          \"lanes\"     :3,\n \
                          \"oneWay\"    :false,\n\
                          \"noOvertake\":true\n\
                       }");
    ASSERT_THROW(Road(0,RoadRoot_missing_type,mockTown0),TrafficSimulation_error);

    faux_CityNetwork mockTown1;
    Json::Value RoadRoot_missing_first=to_Json(
                        "{\n \
                          \"type\"      :\"Motortrafikvej\",\n \
                          \"second\"    :1,\n \
                          \"lanes\"     :3,\n \
                          \"oneWay\"    :false,\n\
                          \"noOvertake\":true\n\
                       }");
    ASSERT_THROW(Road(0,RoadRoot_missing_first,mockTown1),TrafficSimulation_error);

    faux_CityNetwork mockTown2;
    Json::Value RoadRoot_missing_second=to_Json(
                        "{\n \
                          \"type\"      :\"Motortrafikvej\",\n \
                          \"first\"     :0,\n \
                          \"lanes\"     :3,\n \
                          \"oneWay\"    :false,\n\
                          \"noOvertake\":true\n\
                       }");

    ASSERT_THROW(Road(0,RoadRoot_missing_second,mockTown2),TrafficSimulation_error);


    faux_CityNetwork mockTown3;
    //Check with a type of transport which does not exist, for future compatability an example is chosen which will never exist
    Json::Value RoadRoot_wrong_type=to_Json(
                        "{\n \
                          \"type\"      :\"Hyperloop\",\n \
                          \"first\"     :0,\n \
                          \"second\"    :1,\n \
                          \"lanes\"     :3,\n \
                          \"oneWay\"    :false,\n\
                          \"noOvertake\":true\n\
                       }");
    ASSERT_THROW(Road(0,RoadRoot_wrong_type,mockTown3),TrafficSimulation_error);


    faux_CityNetwork mockTown4;
    Json::Value RoadRoot_WrongType_required=to_Json(
                        "{\n \
                          \"type\"      :\"Motortrafikvej\",\n \
                          \"first\"     :\"There\",\n \
                          \"second\"    :\"Back again\",\n \
                          \"lanes\"     :3,\n \
                          \"oneWay\"    :false,\n\
                          \"noOvertake\":true\n\
                       }");
    ASSERT_THROW(Road(0,RoadRoot_WrongType_required,mockTown4),TrafficSimulation_error);

    faux_CityNetwork mockTown5;
    Json::Value RoadRoot_RoundRoad=to_Json(
                        "{\n \
                          \"type\"      :\"Motortrafikvej\",\n \
                          \"first\"     :0,\n \
                          \"second\"    :0,\n \
                          \"lanes\"     :3,\n \
                          \"oneWay\"    :false,\n\
                          \"noOvertake\":true\n\
                       }");
    ASSERT_THROW(Road(0,RoadRoot_RoundRoad,mockTown5),TrafficSimulation_error);
}




TEST(Test_Loading, Initialize_Road_with_valid_or_invalid_JSON) {
    faux_CityNetwork mockTown6;
    Json::Value RoadRoot_all_data=to_Json(
                        "{\n \
                          \"type\"      :\"Motortrafikvej\",\n \
                          \"first\"     :0,\n \
                          \"second\"    :1,\n \
                          \"lanes\"     :3,\n \
                          \"oneWay\"    :false,\n\
                          \"noOvertake\":true\n\
                       }");
    //Before creating the road, both nodes are empty
    ASSERT_EQ(mockTown6.A->getRoadNumber(),0);
    ASSERT_EQ(mockTown6.A->getMaxRoadNumber(),1);

    //There is no harm in copypasting EVERY BLOODY THING
    ASSERT_EQ(mockTown6.B->getRoadNumber(),0);
    ASSERT_EQ(mockTown6.B->getMaxRoadNumber(),1);

    //Should not throw error (if it does gtest will catch it and record a failed test
    Road R(555,RoadRoot_all_data,mockTown6);//Use a weird number ID, to make sure we don't default to 0 and succeed tests we should fail

    //Now it is set
    ASSERT_EQ(mockTown6.A->getRoadNumber(),1);
    ASSERT_EQ(mockTown6.B->getRoadNumber(),1);

    //Verify that the Road knows its ends
    ASSERT_EQ(R.getOther(mockTown6.A->getNodeID()).getNodeID(),mockTown6.B->getNodeID());
    ASSERT_EQ(R.getOther(mockTown6.B->getNodeID()).getNodeID(),mockTown6.A->getNodeID());


    //Using Global ID
    ASSERT_EQ(mockTown6.A->getRoad(R.getRoadID()).getRoadID(), R.getRoadID());
    //The one and only road
    ASSERT_EQ(mockTown6.A->getRoad(0,true).getRoadID(), R.getRoadID());

//Same thing but with the neighbour node

    //Using Global ID
    ASSERT_EQ(mockTown6.A->getNeighbour(R.getRoadID()).getNodeID(), mockTown6.B->getNodeID());
    //The one and only road
    ASSERT_EQ(mockTown6.A->getNeighbour(0,true).getNodeID(), mockTown6.B->getNodeID());

//A few illegal ones

    ASSERT_THROW(mockTown6.A->getRoad(static_cast<size_t>(-1)/*VERY invalid ID*/), road_address_exception);
    ASSERT_THROW(mockTown6.A->getRoad(mockTown6.A->getMaxRoadNumber()+1/*Not in our local list of neighbours*/,true), road_address_exception);

    ASSERT_THROW(mockTown6.A->getNeighbour(static_cast<size_t>(-1)).getNodeID(), road_address_exception);
    //The one and only road
    ASSERT_THROW(mockTown6.A->getNeighbour(mockTown6.A->getMaxRoadNumber()+1,true).getNodeID(), road_address_exception);


    //Now do the exact same thing with the other node, just to see that they connect to each other



    //Using Global ID
    ASSERT_EQ(mockTown6.B->getRoad(R.getRoadID()).getRoadID(), R.getRoadID());
    //The one and only road
    ASSERT_EQ(mockTown6.B->getRoad(0,true).getRoadID(), R.getRoadID());

//Same thing but with the neighbour node

    //Using Global ID
    ASSERT_EQ(mockTown6.B->getNeighbour(R.getRoadID()).getNodeID(), mockTown6.A->getNodeID());
    //The one and only road
    ASSERT_EQ(mockTown6.B->getNeighbour(0,true).getNodeID(), mockTown6.A->getNodeID());

//B few illegal ones

    ASSERT_THROW(mockTown6.B->getRoad(static_cast<size_t>(-1)/*VERY invalid ID*/), road_address_exception);
    ASSERT_THROW(mockTown6.B->getRoad(mockTown6.B->getMaxRoadNumber()+1/*Not in our local list of neighbours*/,true), road_address_exception);

    ASSERT_THROW(mockTown6.B->getNeighbour(static_cast<size_t>(-1)).getNodeID(), road_address_exception);
    //The one and only road
    ASSERT_THROW(mockTown6.B->getNeighbour(mockTown6.B->getMaxRoadNumber()+1,true).getNodeID(), road_address_exception);



}


/*MISSING
Add a car to a road, advance time and verify that the car
verify that exceptions are thrown when "overstepping" updates for car
Adding a car to the road, should have both the car and the road associated with each other.
The number of cars exiting the road, should match the number of cars entering the road
At the end, the car should know that it is not on the road anymore.
The keyframe JSON file must have the expected keyframes
*/


int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
