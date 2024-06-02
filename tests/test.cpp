#include <gtest/gtest.h>

/*
TEST(, ) {
    //This very special year had 10 fewer days in October
    WesternDate YearEnd(31,December,1582);
    ASSERT_EQ(YearEnd.DayOfYear(),355)<<" 1582 should ONLY have 355 days (Due to Julian to Gregorian Calender Switch)";
}
*/


int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
