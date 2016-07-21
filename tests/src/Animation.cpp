#include "gtest/gtest.h"

#include "TestClock.h"

#include "onair/okui/Animation.h"

using namespace onair::okui;

TEST(Animation, basics) {
    TestClock::set(11s);

    Animation<int, TestClock> animation(3);

    EXPECT_EQ(animation.current(), 3);

    TestClock::set(20s);
    animation.target(10, 7s);
    EXPECT_EQ(animation.target(), 10);

    TestClock::set(20s);
    EXPECT_EQ(animation.current(), 3);

    TestClock::set(21s);
    EXPECT_EQ(animation.current(), 4);

    TestClock::set(25s);
    EXPECT_EQ(animation.current(), 8);

    TestClock::set(27s);
    EXPECT_EQ(animation.current(), 10);

    TestClock::set(30s);
    EXPECT_EQ(animation.current(), 10);

    animation.reset(15);

    EXPECT_EQ(animation.current(), 15);
    TestClock::set(100s);
    EXPECT_EQ(animation.current(), 15);
}

TEST(Animation, interpolation) {
    TestClock::set(11s);

    Animation<double, TestClock> animation(3.0);

    EXPECT_FLOAT_EQ(animation.current(), 3.0);

    TestClock::set(20s);
    animation.target(10.0, 7s, interpolation::Exponential::EaseIn);
    EXPECT_FLOAT_EQ(animation.target(), 10);

    TestClock::set(20s);
    EXPECT_NEAR(animation.current(), 3.0, 0.01);

    TestClock::set(21s);
    EXPECT_NEAR(animation.current(), 3.0, 0.1);

    TestClock::set(24s);
    EXPECT_NEAR(animation.current(), 3.3, 0.1);

    TestClock::set(25s);
    EXPECT_NEAR(animation.current(), 4.0, 0.1);

    TestClock::set(26s);
    EXPECT_NEAR(animation.current(), 5.6, 0.1);

    TestClock::set(27s);
    EXPECT_NEAR(animation.current(), 10.0, 0.01);

    TestClock::set(200s);
    EXPECT_NEAR(animation.current(), 10.0, 0.01);

    animation.target(10.0, 7s, interpolation::Exponential::EaseOut);

    // TODO: test values for EaseOutExpo
}
