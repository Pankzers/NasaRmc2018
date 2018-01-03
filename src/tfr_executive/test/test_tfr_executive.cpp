#include <gtest/gtest.h>
#include "executive_helper.h"

TEST(Executive, ExecutiveHelper)
{
    // Google tests provide no handlers for exceptions
    try
    {
        tfr_executive::ExecutiveHelper helper;
        EXPECT_STREQ("Executive System Online", helper.GetEcho().c_str());
    }
    catch (std::exception e)
    {
        // An unhandled exception was thrown; fail the test
        ASSERT_TRUE(false);
    }
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}