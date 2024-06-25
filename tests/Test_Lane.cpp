#include <gtest/gtest.h>
#include<iostream>
#include<vector>


//For a VERBOSE test, useful only for manually verifying that things look sane
//#define DEBUG_PRINT_ON_INSERT
#define DEBUG_PRINT_ENABLE


//The memory monitoring version will be tested in a separate testing program as that must be STRICTLY SINGLE THREADED
//#define DEBUG_loading_counter
#include"Lane.hpp"



#define tolerance 1e-8



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



TYPED_TEST(LaneFundamentalTypeTest , testIterator_equal) {

//global_loading_counter_test_lock.lock();

    //Make a very simple tree, which will not require any rotations, this is only made to test the iterators
    typename TestFixture::TestClass thisLane({1,0,2});

    //Verify the copy and move assignments work
    auto it = thisLane.begin();
    auto notit = thisLane.end();
    //Does the == operator even work
    ASSERT_EQ(it==it,true);
    ASSERT_EQ(it==notit,false);
    ASSERT_EQ(it!=it,false);
    ASSERT_EQ(it!=notit,true);


    //Just to be clear, the type is what we expected?
    static_assert(std::is_same_v<decltype(it),typename TestFixture::TestIteraor>);

//global_loading_counter_test_lock.unlock();
}

TYPED_TEST(LaneFundamentalTypeTest , testIterator_constructors_assignments_and_swaps) {


//global_loading_counter_test_lock.lock();

    typename TestFixture::TestClass thisLane({1,0,2});

    //default constructor
    typename TestFixture::TestIteraor it;
    ASSERT_EQ(it,thisLane.end());


    it = thisLane.begin();//copy assignment
    auto other_it(it);//Copy constructor

    ASSERT_EQ(it,thisLane.begin());
    ASSERT_EQ(other_it,thisLane.begin());

    //Check that move constructor works
    auto yet_another_it = std::move(it);
    ASSERT_EQ(yet_another_it,thisLane.begin());
    ASSERT_EQ(it,thisLane.end());//end is default (null)

    //Now swap these two different things
    it.swap(yet_another_it);
    ASSERT_EQ(it,thisLane.begin());
    ASSERT_EQ(yet_another_it,thisLane.end());//end is default (null)
//global_loading_counter_test_lock.unlock();
}

TYPED_TEST(LaneFundamentalTypeTest , testIterator_traverse_and_dereference) {

//global_loading_counter_test_lock.lock();

    typename TestFixture::TestClass thisLane({1,0,2});

    //Create beginning iterator, and check the ++ operator
    auto it = thisLane.begin();
    ASSERT_EQ(*it,0);
    ++it;
    ASSERT_EQ(*it,1);
    ++it;
    ASSERT_EQ(*it,2);
    ++it;
    ASSERT_EQ(it,thisLane.end());



    {
        typename TestFixture::TestVector V;

        for (auto& t : thisLane)
            V.push_back(t);

        //This also checks that it is sorted, but most importantly it checks that we could traverse the lane
        ASSERT_EQ(V.size(),3);
        ASSERT_EQ(V[0],0);
        ASSERT_EQ(V[1],1);
        ASSERT_EQ(V[2],2);
    }
    //Same thing, but const
    {
        typename TestFixture::TestVector V;

        for (const auto& t : thisLane)
            V.push_back(t);

        //This also checks that it is sorted, but most importantly it checks that we could traverse the lane
        ASSERT_EQ(V.size(),3);
        ASSERT_EQ(V[0],0);
        ASSERT_EQ(V[1],1);
        ASSERT_EQ(V[2],2);
    }

//global_loading_counter_test_lock.unlock();

}


TYPED_TEST(LaneFundamentalTypeTest , test_size) {
//global_loading_counter_test_lock.lock();

    typename TestFixture::TestClass thisLane({1,0,2,3,4,5,6,7});

    ASSERT_EQ(thisLane.size(),8);
    ASSERT_EQ(thisLane.empty(),false);
    ASSERT_EQ(thisLane.max_size(),std::numeric_limits<size_t>::max());


    typename TestFixture::TestClass default_constructed;

    ASSERT_EQ(default_constructed.size(),0);
    ASSERT_EQ(default_constructed.empty(),true);
    ASSERT_EQ(default_constructed.max_size(),std::numeric_limits<size_t>::max());

}


TYPED_TEST(LaneFundamentalTypeTest , swap_Lane) {
//global_loading_counter_test_lock.lock();

    typename TestFixture::TestClass thisLane({1,0,2,3,4,5,6,7});
    typename TestFixture::TestClass thatLane({1,0,2});

    typename TestFixture::TestClass old_thisLane(thisLane);
    typename TestFixture::TestClass old_thatLane(thatLane);






    //Prior to swapping
    ASSERT_EQ(thisLane,old_thisLane);
    ASSERT_EQ(thatLane,old_thatLane);
    ASSERT_NE(thatLane,thisLane);

    TestFixture::TestClass::swap(thisLane,thatLane);

    ASSERT_EQ(thatLane,old_thisLane);
    ASSERT_EQ(thisLane,old_thatLane);
    ASSERT_NE(thatLane,thisLane);

//global_loading_counter_test_lock.unlock();
}

TYPED_TEST(LaneFundamentalTypeTest , testLane_equal_and_constructors) {
    //Should by all accounts be the same thing
    typename TestFixture::TestClass default_constructed;
    typename TestFixture::TestClass also_default_constructed;


    ASSERT_EQ(also_default_constructed,default_constructed);//Calls Lane<T>::==
    ASSERT_EQ(also_default_constructed!=default_constructed,false);
}


TYPED_TEST(LaneFundamentalTypeTest , test_iterator_construction_size_and_sorting) {

    typename TestFixture::TestVector    Sorted({0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32});

    typename TestFixture::TestVector shuffled0({31,10,18,6,11,14,17,20,25,21,3,2,27,9,4,16,32,23,26,8,0,19,28,12,13,1,29,22,5,24,30,15,7});

    typename TestFixture::TestVector shuffled1({17,30,21,0,8,15,32,22,27,13,28,14,6,1,31,4,29,3,25,12,10,11,7,5,19,24,18,20,2,26,16,23,9});



    typename TestFixture::TestClass inserted_from_sorted(Sorted.begin(),Sorted.end());
    typename TestFixture::TestClass inserted_from_shuffled0(shuffled0.begin(),shuffled0.end());
    typename TestFixture::TestClass inserted_from_shuffled1(shuffled1.begin(),shuffled1.end());


    //Verify that this is sorted
    ASSERT_EQ(inserted_from_sorted,inserted_from_shuffled0);
    ASSERT_EQ(inserted_from_sorted,inserted_from_shuffled1);
}
