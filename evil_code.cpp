#include<set>
#include<iostream>
#include<string>

//A car which will be sorted based on position
class Car
{
private:
    std::string name;
    double distance;

public:
    Car(double d,const std::string& n="null"): distance(d), name(n){}

    bool operator<(const Car& that) const {return distance<that.distance;}

    std::string toString()const{ return name+" ("+std::to_string(distance)+")";}

    void drive(double d){distance+=d;}
};

int main()
{

    std::set<Car> Lane;

    //The cars get inserted based on distance
    Lane.insert(Car(1.0,"Trabant"));
    Lane.insert(Car(0.0,"Volvo"));
    Lane.insert(Car(3.0,"Fiat"));
    Lane.insert(Car(2.0,"Volkswagen"));

    std::cout<<"Cars before driving\n";
    for (const Car& C : Lane)
        std::cout<<C.toString()<<' ';
    std::cout<<"\n\n";

    //Find the car just before 3.5 km
    const auto lb_before = --Lane.upper_bound(3.5);//Uses Car(3.5,"null") to construct a temporary car for comparison

    std::cout<<"The car just before 3.5 km is "<<lb_before->toString()<<std::endl;

    std::cout<<"\n\n";
    //The cars drive 2 km, after this is done the order is the same
    for (const Car& C : Lane)
        const_cast<Car&>(C).drive(2.0);


    //Before driving, this would end between Volkswagen and Fiat
    Lane.insert(Car(2.5,"Ford"));

    std::cout<<"Cars after driving and inserting\n";
    for (const Car& C : Lane)
        std::cout<<C.toString()<<' ';
    std::cout<<"\n\n";
    //Find the car just before 3.5 km
    const auto lb_after = --Lane.upper_bound(3.5);//Uses Car(3.5,"null") to construct a temporary car for comparison

    std::cout<<"The car just before 3.5 km is "<<lb_after->toString()<<std::endl;

    std::cout<<"\n\n";

    return 0;
}
