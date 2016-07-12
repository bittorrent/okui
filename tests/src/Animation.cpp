#include "gtest/gtest.h"

#include "onair/okui/config.h"
#include "onair/okui/Animation.h"

using namespace onair::okui;

template <typename T>
struct Clock {
    using time_point = T;
    using duration = T;

    static void set(time_point t) { Clock::t = t; }
    static time_point now() { return t; }

    static time_point t;
};

template <typename T>
typename Clock<T>::time_point Clock<T>::t;

TEST(Animation, basics) {
    using C = Clock<std::chrono::seconds>;

    C::set(11s);

    Animation<int, C> animation(3);

    EXPECT_EQ(animation.current(), 3);

    C::set(20s);
    animation.target(10, 7s);
    EXPECT_EQ(animation.target(), 10);

    C::set(20s);
    EXPECT_EQ(animation.current(), 3);

    C::set(21s);
    EXPECT_EQ(animation.current(), 4);

    C::set(25s);
    EXPECT_EQ(animation.current(), 8);

    C::set(27s);
    EXPECT_EQ(animation.current(), 10);

    C::set(30s);
    EXPECT_EQ(animation.current(), 10);

    animation.reset(15);

    EXPECT_EQ(animation.current(), 15);
    C::set(100s);
    EXPECT_EQ(animation.current(), 15);
}

TEST(Animation, chain) {
    using C = Clock<std::chrono::seconds>;

    C::set(11s);

    AnimationChain<int, C> animation(3);

    EXPECT_EQ(animation.current(), 3);

    C::set(20s);
    animation.target(10, 7s, 5, 3s, 100, 10s);

    C::set(20s);
    EXPECT_EQ(animation.current(), 3);

    C::set(21s);
    EXPECT_EQ(animation.current(), 4);

    C::set(25s);
    EXPECT_EQ(animation.current(), 8);

    C::set(27s);
    EXPECT_EQ(animation.current(), 10);

    C::set(30s);
    EXPECT_EQ(animation.current(), 5);

    C::set(32s);
    EXPECT_EQ(animation.current(), 24);

    C::set(40s);
    EXPECT_EQ(animation.current(), 100);

    C::set(42s);
    EXPECT_EQ(animation.current(), 100);

    animation.reset(15);

    EXPECT_EQ(animation.current(), 15);
    C::set(100s);
    EXPECT_EQ(animation.current(), 15);
}

TEST(Animation, interpolation) {
    using C = Clock<std::chrono::seconds>;

    C::set(11s);

    Animation<double, C> animation(3.0);

    EXPECT_FLOAT_EQ(animation.current(), 3.0);

    C::set(20s);
    animation.target(10.0, 7s, interpolation::Exponential::EaseIn);
    EXPECT_FLOAT_EQ(animation.target(), 10);

    C::set(20s);
    EXPECT_NEAR(animation.current(), 3.0, 0.01);

    C::set(21s);
    EXPECT_NEAR(animation.current(), 3.0, 0.1);

    C::set(24s);
    EXPECT_NEAR(animation.current(), 3.3, 0.1);

    C::set(25s);
    EXPECT_NEAR(animation.current(), 4.0, 0.1);

    C::set(26s);
    EXPECT_NEAR(animation.current(), 5.6, 0.1);

    C::set(27s);
    EXPECT_NEAR(animation.current(), 10.0, 0.01);

    C::set(200s);
    EXPECT_NEAR(animation.current(), 10.0, 0.01);

    animation.target(10.0, 7s, interpolation::Exponential::EaseOut);

    // TODO: test values for EaseOutExpo
}
