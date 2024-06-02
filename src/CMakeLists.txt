# src/CMakeLists.txt
# ANY CLASS LIBRARIES CAN BE ADDED LIKE THIS
#add_library(Class class.cpp)

# Define the executable
add_executable(trafficSimulation main.cpp)

# Include directories
target_include_directories(trafficSimulation PRIVATE ../include)

# Link libraries
#target_link_libraries(trafficSimulation Class)
#target_include_directories(Class PRIVATE ../include)
