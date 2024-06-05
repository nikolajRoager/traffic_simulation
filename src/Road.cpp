#include "Road.hpp"
#include "json/json.h"
#include<string>

using std::string;


Road::Road(Json::Value& object){


    //Not required, and given a default value, no throw guaranteed

    no_overtake= object.get("no_overtake",true).asBool();
    one_way= object.get("one_way",false).asBool();
    lanes  = object.get("lanes",1).asInt();

    string type_name=object["type"].asString();
    //int first   = object["first"].asInt();
    //int second  = object["second"].asInt();

}
