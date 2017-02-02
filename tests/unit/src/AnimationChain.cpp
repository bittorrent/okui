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
