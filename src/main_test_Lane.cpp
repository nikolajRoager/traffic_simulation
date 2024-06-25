#include<iostream>

#define DEBUG_PRINT_ON_INSERT
#define DEBUG_loading_counter
#include"Lane.hpp"

//Simply demonstrate that the lane container is working, with an example of how to use it, this is not the test, that lives in the test/ directory, it is more an example of how to use it, for later copy-pasting

using std::cout, std::cin, std::endl;

int main()
{
    {
        Lane<char> testLane={'M','N' ,'O', 'L','K','Q','P','H','I','A'};

        for (const char& d : testLane)
        {
            std::cout<<d<<std::endl;
        }
        testLane.DEBUG_print_tree();

        std::cout<<loading_counter<<std::endl;



        //MNO l  LK r QP rl HI lr A rrwc
        Lane<char> testLane_copy(testLane);

        testLane_copy.DEBUG_print_tree();

        std::cout<<loading_counter<<std::endl;

        std::cout<<(testLane==testLane_copy ? "same" : "not")<<std::endl;
    }

    std::cout<<loading_counter<<std::endl;

    return 0;
}
