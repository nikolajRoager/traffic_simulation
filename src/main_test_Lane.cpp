#include<iostream>
#include"Lane.hpp"

//Simply demonstrate that the lane container is working, with an example of how to use it, this is not the test, that lives in the test/ directory, it is more an example of how to use it, for later copy-pasting

using std::cout, std::cin, std::endl;

int main()
{
    {

    //MNO l  LK r QP rl HI lr A rrwc
    Lane<char> testLane={'M','N' ,'O', 'L','K','Q','P','H','I','A'};//,'I'};

    //for (double d : testLane)
    //{
    //    std::cout<<d<<std::endl;
    //}
    testLane.DEBUG_print_tree();

    std::cout<<loading_counter<<std::endl;
    }

    std::cout<<loading_counter<<std::endl;

    return 0;
}
