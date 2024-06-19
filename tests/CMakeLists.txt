# tests/CMakeLists.txt

# Locate GTest
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

# Define the test executable
add_executable(Test test.cpp Test_Loading.cpp Test_Vehicle.cpp )

# Include directories
target_include_directories(Test PRIVATE ../include)



# Link to gtest framework
target_link_libraries(Test ${GTEST_LIBRARIES} pthread )

file(GLOB_RECURSE sources
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)


#Link Jsoncpp
target_link_libraries(Test ${JSONCPP_LIBRARIES})

# Link libraries to main program
target_link_libraries(Test RoadVehicle)
target_link_libraries(Test Car)
target_link_libraries(Test Road)
target_link_libraries(Test Node)
target_link_libraries(Test Hellhole)
target_link_libraries(Test CityNetwork)

# Add test
add_test(NAME TestTraffic COMMAND Test)
