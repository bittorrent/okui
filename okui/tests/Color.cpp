#include "gtest/gtest.h"

#include "onair/okui/Color.h"

using namespace onair;
using namespace onair::okui;

TEST(Color, construction) {
    EXPECT_TRUE(Color(1.0) == Color(1.0, 1.0, 1.0, 1.0));
    EXPECT_TRUE(Color(1.0, 0.5) == Color(1.0, 1.0, 1.0, 0.5));
    EXPECT_TRUE(Color(2.0, 3.0, 4.0) == Color(2.0, 3.0, 4.0, 1.0));
}

TEST(Color, multiplication) {
    EXPECT_EQ(Color(1.0, 2.0, 3.0, 4.0) * 2.0, Color(2.0, 4.0, 6.0, 8.0));
}

TEST(Color, addition) {
    EXPECT_EQ(Color(1.0, 2.0, 3.0, 4.0) + Color(2.0, 3.0, 4.0, 5.0), Color(3.0, 5.0, 7.0, 9.0));
}

TEST(Color, interpolation) {
    EXPECT_EQ(Color(1.0, 1.0, 1.0, 1.0).interpolate(Color(2.0, 3.0, 4.0, 5.0), 0.25), Color(1.25, 1.5, 1.75, 2.0));
}

TEST(Color, comparison) {
    EXPECT_FALSE(Color(1.0, 2.0, 3.0, 4.0) == Color(2.0, 3.0, 4.0, 5.0));
    EXPECT_TRUE(Color(1.0, 2.0, 3.0, 4.0) == Color(1.0, 2.0, 3.0, 4.0));
    EXPECT_FALSE(Color(0.0, 2.0, 3.0, 4.0) == Color(1.0, 2.0, 3.0, 4.0));
    EXPECT_FALSE(Color(1.0, 0.0, 3.0, 4.0) == Color(1.0, 2.0, 3.0, 4.0));
    EXPECT_FALSE(Color(1.0, 2.0, 0.0, 4.0) == Color(1.0, 2.0, 3.0, 4.0));
    EXPECT_FALSE(Color(1.0, 2.0, 3.0, 0.0) == Color(1.0, 2.0, 3.0, 4.0));
}

TEST(Color, literals) {
    EXPECT_EQ(Color(1.0, 127.0 / 255.0, 1.0, 0.0), 0xFF7FFF00_rgba);
    EXPECT_EQ(Color(1.0, 127.0 / 255.0, 0.0, 1.0), 0xFF7F00_rgb);
    EXPECT_EQ(Color(0.0), 0x000000FF_rgba);
    EXPECT_EQ(Color(0.0), 0x000000_rgb);
}
