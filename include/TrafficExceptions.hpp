#include<exception>
#include<string>
#include<vector>

//A collection of errors, related to the traffic simulation







class vehicle_past_update_exception: public std::exception{
private:
    //Declare the message here, as a C++ string, that way the c_str() will live as long as the object itself
    std::string msg;
public:
    vehicle_past_update_exception(int vehicleID, double requested_time, double next_update) noexcept : msg("Vehicle ID "+std::to_string(vehicleID)+" asked to update until "+std::to_string(requested_time)+", but has updated queued for "+std::to_string(next_update)){}


    virtual char const* what() const noexcept
    {
        //This pointer lives as long as the exception object itself
        return msg.c_str();
    };
};



class road_address_exception: public std::exception
{
private:
    //Declare the message here, as a C++ string, that way the c_str() will live as long as the object itself
    std::string msg;
public:
    road_address_exception(int roadID, int maxRoads, int nodeID) noexcept : msg("Asked for local Road "+std::to_string(roadID)+" out of "+std::to_string(maxRoads)+" at Node "+std::to_string(nodeID)){}

    //This is called when we asked for a roadID not in the legal road IDs
    road_address_exception(int roadID, std::vector<int> legalRoadID, int nodeID)noexcept : msg("Asked for global Road ID "+std::to_string(roadID)+" at Node "+std::to_string(nodeID)+" valid roads are:")
    {
        for (int i : legalRoadID)
            msg += std::to_string(i);
    }

    virtual char const* what() const noexcept
    {
        //This pointer lives as long as the exception object itself
        return msg.c_str();
    };
};



//More generic error with a more general message
class city_loading_exception : public std::exception
{
    std::string msg;
public:
    city_loading_exception(std::string _msg) : msg(_msg){}
    virtual char const* what() const noexcept
    {
        return exception::what();
    };
};
