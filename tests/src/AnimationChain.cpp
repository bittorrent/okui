#include "TestClock.h"

#include <okui/AnimationChain.h>

#include <gtest/gtest.h>

using namespace okui;

TEST(AnimationChain, basic) {
    TestClock::set(11s);

    AnimationChain<int, TestClock> animation(3);

    EXPECT_EQ(animation.current(), 3);

    TestClock::set(20s);
    animation.target(10, 7s, 5, 3s, 100, 10s);

    TestClock::set(20s);
    EXPECT_EQ(animation.current(), 3);

    TestClock::set(21s);
    EXPECT_EQ(animation.current(), 4);

    TestClock::set(25s);
    EXPECT_EQ(animation.current(), 8);

    TestClock::set(27s);
    EXPECT_EQ(animation.current(), 10);

    TestClock::set(30s);
    EXPECT_EQ(animation.current(), 5);

    TestClock::set(32s);
    EXPECT_EQ(animation.current(), 24);

    TestClock::set(40s);
    EXPECT_EQ(animation.current(), 100);

    TestClock::set(42s);
    EXPECT_EQ(animation.current(), 100);

    animation.reset(15);

    EXPECT_EQ(animation.current(), 15);
    TestClock::set(100s);
    EXPECT_EQ(animation.current(), 15);
}
