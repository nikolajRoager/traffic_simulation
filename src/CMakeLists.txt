# src/CMakeLists.txt
# A few class libraries
add_library(RoadVehicle RoadVehicle.cpp)
add_library(Car Car.cpp)
add_library(Road Road.cpp)
add_library(Node Node.cpp)
add_library(Hellhole Hellhole.cpp)
add_library(CityNetwork CityNetwork.cpp)


# Define the executable
add_executable(test_lane_container main_test_Lane.cpp)
target_include_directories(test_lane_container PRIVATE ../include)


# Define the executable
add_executable(trafficSimulation main.cpp)

# Everyone get your headers from here
target_include_directories(trafficSimulation PRIVATE ../include)
target_include_directories(RoadVehicle PRIVATE ../include)
target_include_directories(Car PRIVATE ../include)
target_include_directories(Road PRIVATE ../include)
target_include_directories(Node PRIVATE ../include)
target_include_directories(Hellhole PRIVATE ../include)
target_include_directories(CityNetwork PRIVATE ../include)

#Link Jsoncpp
target_link_libraries(trafficSimulation ${JSONCPP_LIBRARIES})

# Link libraries to main program
target_link_libraries(trafficSimulation RoadVehicle)
target_link_libraries(trafficSimulation Car)
target_link_libraries(trafficSimulation Road)
target_link_libraries(trafficSimulation Node)
target_link_libraries(trafficSimulation Hellhole)
target_link_libraries(trafficSimulation CityNetwork)

#Link Jsoncpp to the CityNetwork
target_link_libraries(CityNetwork ${JSONCPP_LIBRARIES})


# And link the libraries needed to make the libraries
target_link_libraries(Car RoadVehicle)

target_link_libraries(Hellhole Road)
target_link_libraries(Hellhole Node)

target_link_libraries(CityNetwork Road)
target_link_libraries(CityNetwork Node)
