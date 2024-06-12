#include<iostream>
#include"Hellhole.hpp"
#include"Node.hpp"

using std::cout, std::cin, std::endl;

class DUM
{
    int dumdum;
public:
    DUM(int dum):dumdum(dum){}

    void test() const {cout<<dumdum<<endl;}
};

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        cout<<"Need: "<<argv[0]<<" input_city_file output_folder"<<endl;
        return 1;
    }

    //STUB
    return 0;
}
