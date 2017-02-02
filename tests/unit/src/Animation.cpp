/**
* Copyright 2017 BitTorrent Inc.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*    http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/
#include "TestClock.h"

#include <okui/Animation.h>

#include <gtest/gtest.h>

using namespace okui;

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

    // this tests a specific potential crash bug we had where calling current() after a reset()
    // could attempt to use a null interpolator if clock::now() was before the time_point default
    // construction (epoch of the clock)
    TestClock::set(-1s);
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
