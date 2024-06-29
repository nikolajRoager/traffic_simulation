#include <gtest/gtest.h>
#include<iostream>
#include<vector>
#include<algorithm>


//For a VERBOSE test, useful only for manually verifying that things look sane
//#define DEBUG_PRINT_ON_INSERT
//#define DEBUG_PRINT_ENABLE


//The memory monitoring version will be tested in a separate testing program as that must be STRICTLY SINGLE THREADED
//#define DEBUG_loading_counter

#undef DEBUG_PRINT_ON_INSERT
#include"Lane.hpp"



#define tolerance 1e-8


class DoubleConstructible{
    private:
        double val;

    public:
    DoubleConstructible(double _val=0){
        val=_val;
    }

    DoubleConstructible(const DoubleConstructible& that){
        val=that.val;
    }

    DoubleConstructible(DoubleConstructible&& that){
        val=that.val;
    }

    DoubleConstructible& operator=(DoubleConstructible&& that){
        val=that.val;
        return *this;
    }

    DoubleConstructible& operator=(const DoubleConstructible& that){
        val=that.val;
        return *this;
    }

    bool operator==(const DoubleConstructible& that) const
    {
        return that.val==val;
    }

    bool operator<(const DoubleConstructible& that) const
    {
        return val<that.val;
    }

    bool operator==(const double& that) const
    {
        return val==that;
    }

    bool operator<(const double& that) const
    {
        return val<that;
    }

    ~DoubleConstructible(){
    }

    //For checking the -> operator on the iterator
    double getValue() const {return val;}
};


//We are going to repeat this for all of these fundamental types, which have < operators with themself and double, and can be made from unsigned integers.
typedef ::testing::Types<unsigned char, char, int, long, unsigned int, unsigned long, size_t, float, double,DoubleConstructible> fundamentalTypes;

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


TYPED_TEST(LaneFundamentalTypeTest , test_sorting_and_insertion_order) {

    typename TestFixture::TestVector    Sorted({0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32});

    typename TestFixture::TestVector shuffled0({31,10,18,6,11,14,17,20,25,21,3,2,27,9,4,16,32,23,26,8,0,19,28,12,13,1,29,22,5,24,30,15,7});

    typename TestFixture::TestVector shuffled1({17,30,21,0,8,15,32,22,27,13,28,14,6,1,31,4,29,3,25,12,10,11,7,5,19,24,18,20,2,26,16,23,9});



    typename TestFixture::TestClass inserted_from_sorted(Sorted.begin(),Sorted.end());
    typename TestFixture::TestClass inserted_from_shuffled0(shuffled0.begin(),shuffled0.end());
    typename TestFixture::TestClass inserted_from_shuffled1(shuffled1.begin(),shuffled1.end());


    //Verify that this is sorted
    ASSERT_EQ(inserted_from_sorted,inserted_from_shuffled0);
    ASSERT_EQ(inserted_from_sorted,inserted_from_shuffled1);

    ASSERT_TRUE(std::is_sorted(   inserted_from_sorted.begin(),   inserted_from_sorted.end()));
    ASSERT_TRUE(std::is_sorted(inserted_from_shuffled0.begin(),inserted_from_shuffled0.end()));
    ASSERT_TRUE(std::is_sorted(inserted_from_shuffled1.begin(),inserted_from_shuffled1.end()));
}

TYPED_TEST(LaneFundamentalTypeTest , copy_and_move_assignments) {

    {
        typename TestFixture::TestClass thisLane({1,0,2,3,4,5,6,7});
        typename TestFixture::TestClass DifferentLane({1,0,2});

        ASSERT_NE(thisLane,DifferentLane);

        //Test copy assignment, on top of SOMETHING WHICH ALREADY EXISTS
        thisLane=DifferentLane;
        ASSERT_EQ(thisLane,DifferentLane);
    }

    //Same thing with move assignment,
    {
        //Create this thing
        typename TestFixture::TestClass thisLane({1,0,2,3,4,5,6,7});
        //And the data we will move
        typename TestFixture::TestClass thatLane({1,0,2});
        {
            //But move the data out of this copy
            typename TestFixture::TestClass DifferentLane(thatLane);

            ASSERT_NE(thisLane,DifferentLane);
            thisLane=std::move(DifferentLane);
            ASSERT_NE(thisLane,DifferentLane);

        }
        //The thing we moved out of just called its destructor, verify that we still exist by comparing to the copy
        ASSERT_EQ(thisLane,thatLane);

    }
}

//This class keeps a reference to a memory counter, stored here, anything copied from the instances we send in will update the memory counter in this function
class C{
    private:
        int& MemoryCounter;
        double val;

    public:
    C(int& MC, double _val): MemoryCounter(MC){
        val=_val;
        ++MemoryCounter;
    }

    C(const C& that): MemoryCounter(that.MemoryCounter){
        val=that.val;
        ++MemoryCounter;
    }

    C(C&& that): MemoryCounter(that.MemoryCounter){
        val=that.val;
        ++MemoryCounter;
    }

    C& operator=(C&& that){
        val=that.val;
        MemoryCounter=that.MemoryCounter;
        return *this;
    }

    C& operator=(const C& that){
        val=that.val;
        MemoryCounter=that.MemoryCounter;
        return *this;
    }

    bool operator==(const C& that) const
    {
        return that.val==val;
    }

    bool operator<(const C& that) const
    {
        return val<that.val;
    }

    bool operator==(const double& that) const
    {
        return val==that;
    }

    bool operator<(const double& that) const
    {
        return val<that;
    }

    ~C(){
        --MemoryCounter;
    }

    //For checking the -> operator on the iterator
    double getValue() const {return val;}
};


//We are going to repeat some saving and loading tests with this custom class, and use it to track that allocation and freeing is balanced
TEST(CustomClassLaneTest,testLaneMemory_constructors)
{

    int Mem = 0;



    {
        Lane<C> thisLane({C(Mem,1),C(Mem,0),C(Mem,2),C(Mem,3),C(Mem,4),C(Mem,5),C(Mem,6),C(Mem,7)});
        Lane<C> DifferentLane({C(Mem,1),C(Mem,0),C(Mem,2)});

        ASSERT_NE(thisLane,DifferentLane);
        ASSERT_EQ(Mem,11);
    }//Destructor called, mem should be 0

    ASSERT_EQ(Mem,0);



}
TEST(CustomClassLaneTest,testLaneMemory_copyConstructor)
{

    int Mem = 0;

    //These brackets forces the destructors to be called, before we do the final memory check
    {
        Lane<C> thisLane({C(Mem,1),C(Mem,0),C(Mem,2),C(Mem,3),C(Mem,4),C(Mem,5),C(Mem,6),C(Mem,7)});
        Lane<C> DifferentLane({C(Mem,1),C(Mem,0),C(Mem,2)});

        ASSERT_EQ(Mem,11);

        ASSERT_NE(thisLane,DifferentLane);

        //Test copy assignment, on top of SOMETHING WHICH ALREADY EXISTS
        thisLane=DifferentLane;

        //That which was there is now gone, but now a copy of the 3 things in DifferentLane exist both here and there
        ASSERT_EQ(Mem,6);


        ASSERT_EQ(thisLane,DifferentLane);
    }
    ASSERT_EQ(Mem,0);
}

/*


//Same thing with move assignment,
TEST(CustomClassLaneTest,testLaneMemory_moveConstructor)
{

    int Mem = 0;
    {
        Lane<C> thisLane({C(Mem,1),C(Mem,0),C(Mem,2),C(Mem,3),C(Mem,4),C(Mem,5),C(Mem,6),C(Mem,7)});
        Lane<C> thatLane({C(Mem,1),C(Mem,0),C(Mem,2)});

        {
            //But move the data out of this copy
            Lane<C> DifferentLane(thatLane);

            ASSERT_NE(thisLane,DifferentLane);
            thisLane=std::move(DifferentLane);
            ASSERT_NE(thisLane,DifferentLane);

            //Move should have dealocated that stored in thisLane, and DifferentLanes content now live there (and only there)
            ASSERT_EQ(Mem,6);
        }
        //The thing we moved out of just called its destructor, verify that we still exist by comparing to the copy
        ASSERT_EQ(thisLane,thatLane);
        ASSERT_EQ(Mem,10);

    }
    ASSERT_EQ(Mem,0);
}
*/
