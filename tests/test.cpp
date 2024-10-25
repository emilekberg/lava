#include <gtest/gtest.h>
#include <lava/header.h>
TEST(LAVA_ENGINE, test)
{
    auto a = 2;
    EXPECT_EQ(a, 2);
    EXPECT_EQ(lava::hello_world(), 42);
}