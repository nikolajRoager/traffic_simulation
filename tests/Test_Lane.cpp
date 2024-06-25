#include <gtest/gtest.h>
#include<iostream>
#include<vector>


//For a VERBOSE test, useful only for manually verifying that things look sane
//#define DEBUG_PRINT_ON_INSERT
//#define DEBUG_PRINT_ENABLE

//Activate a memory counter, titled loading_counter, which increments when an internal node is loaded, and decrements when it is deleted
#define DEBUG_loading_counter
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

}

TYPED_TEST(LaneFundamentalTypeTest , testIterator_constructors_assignments_and_swaps) {
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
}

TYPED_TEST(LaneFundamentalTypeTest , testIterator_traverse_and_dereference) {
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
}


//With the iterator working for fundamental types (save fo the -> operator, which doesn't make sense for fundamental types), we can move on to testing the Lane itself

