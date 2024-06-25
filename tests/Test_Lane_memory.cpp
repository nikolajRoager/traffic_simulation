#include <gtest/gtest.h>
#include<iostream>
#include<vector>


//Special version of lane test, where we do monitor memory

//Activate a memory counter, titled loading_counter, which increments when an internal node is loaded, and decrements when it is deleted
#define DEBUG_loading_counter
#include"Lane.hpp"


#define tolerance 1e-8


#include<mutex>
//any tests which uses the loading counter should not be multi-threaded
std::mutex global_loading_counter_test_lock;



//We are going to repeat this for all of these fundamental types, which have < operators with themself and double, and can be made from unsigned integers.
typedef ::testing::Types<unsigned char, char, int, long, unsigned int, unsigned long, size_t, float, double> fundamentalTypes;

template <typename T>
    class LaneFundamentalTypeTest : public ::testing::Test {
    public:
        typedef Lane<T> TestClass;
        typedef Lane<T>::iterator TestIteraor;
        typedef T TestType;
        typedef std::vector<T> TestVector;
};

//Now anything in this Suite will get repeated for every type
TYPED_TEST_SUITE(LaneFundamentalTypeTest , fundamentalTypes);



//With the iterator working for fundamental types (save fo the -> operator, which doesn't make sense for fundamental types), we can move on to testing the Lane itself
TYPED_TEST(LaneFundamentalTypeTest , testLane_memory_after_constructors) {


    //We will not have assert statements until after the lock has been unlocked, lest a single failed assert halts the entire test-suite.

    bool ExtraLane_is_thatLane_after_move =false;
    bool thisLane_is_thatLane_after_copy =false;
    global_loading_counter_test_lock.lock();
    {
        //Try copy constructor
        typename TestFixture::TestClass thisLane({1,0,2,3,4,5,6,7});
        typename TestFixture::TestClass thatLane(thisLane);


        thisLane_is_thatLane_after_copy = (thisLane == thatLane);

        //Test move constructor
        typename TestFixture::TestClass ExtraLane(std::move(thisLane));
        ExtraLane_is_thatLane_after_move =ExtraLane == thatLane;

    }
    loading_counter_lock.lock();
    auto nodes_loaded_after_delete =loading_counter;
    loading_counter_lock.unlock();
    global_loading_counter_test_lock.unlock();

//All assert statements are outside the lock, lest we lock-up the test on failure
    ASSERT_EQ(nodes_loaded_after_delete ,0);
    ASSERT_EQ(ExtraLane_is_thatLane_after_move,true);
    ASSERT_EQ(thisLane_is_thatLane_after_copy,true);

}

//With the iterator working for fundamental types (save fo the -> operator, which doesn't make sense for fundamental types), we can move on to testing the Lane itself
TYPED_TEST(LaneFundamentalTypeTest , memory_after_copy_and_move_assignments) {


    //Once again, ASSERT only goes after we unlock, lest a failed test halt the suite
    bool memory_freed_after_copy_assign=false;
    bool Was_different_before_copy =false;
    bool Was_same_after_copy =false;

    bool memory_freed_after_move_assign=false;
    bool Was_different_before_move =false;
    bool Was_same_after_move =false;

global_loading_counter_test_lock.lock();



    {
        typename TestFixture::TestClass thisLane({1,0,2,3,4,5,6,7});
        typename TestFixture::TestClass DifferentLane({1,0,2});

        Was_different_before_copy = (thisLane!=DifferentLane);

        //Test copy assignment, on top of SOMETHING WHICH ALREADY EXISTS
        thisLane=DifferentLane;
        Was_same_after_copy = thisLane==DifferentLane;
    }

    loading_counter_lock.lock();
    memory_freed_after_copy_assign= loading_counter==0;
    loading_counter_lock.unlock();
    //Same thing with move assignment,
    {
        //Create this thing
        typename TestFixture::TestClass thisLane({1,0,2,3,4,5,6,7});
        //And the data we will move
        typename TestFixture::TestClass thatLane({1,0,2});
        {
            //But move the data out of this copy
            typename TestFixture::TestClass DifferentLane(thatLane);

            Was_different_before_move=(thisLane!=DifferentLane);
            thisLane=std::move(DifferentLane);
            Was_same_after_move = (thisLane==thatLane);

        }
        //The thing we moved out of just called its destructor, verify that we still exist by comparing to the copy
        ASSERT_EQ(thisLane,thatLane);

    }

    //Memory could both be left unfreed (if we did not free the already loaded thisLane) or double-freed (if DifferentLane freed its data after the move assignment)

    loading_counter_lock.lock();
    memory_freed_after_move_assign=loading_counter==0;
    loading_counter_lock.unlock();

global_loading_counter_test_lock.unlock();


    ASSERT_EQ(memory_freed_after_copy_assign, true);
    ASSERT_EQ(Was_different_before_copy     , true);
    ASSERT_EQ(Was_same_after_copy           , true);

    ASSERT_EQ(memory_freed_after_move_assign, true);
    ASSERT_EQ(Was_different_before_move     , true);
    ASSERT_EQ(Was_same_after_move           , true);


}


int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
