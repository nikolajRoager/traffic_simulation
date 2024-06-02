# tests/CMakeLists.txt

# Locate GTest
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

# Define the test executable
add_executable(Test test.cpp)

# Include directories
target_include_directories(Test PRIVATE ../include)

# Link the main project executable or libraries if needed
# target_link_libraries(Test Class)

# Link to gtest framework
target_link_libraries(Test ${GTEST_LIBRARIES} pthread )

# Add test
add_test(NAME TestDate COMMAND Test)
