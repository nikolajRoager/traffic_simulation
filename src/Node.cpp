#include"Node.hpp"
#include<cmath>

double Node::getDist(const Node& Other)const
{
    return sqrt(pow(x-Other.x,2)+pow(y-Other.y,2));
}
