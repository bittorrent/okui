#include "gtest/gtest.h"

#include "onair/okui/config.h"
#include "onair/okui/Animation.h"

using namespace onair;
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
    
    C::set(11_s);

    Animation<int, C> animation(3);
    
    EXPECT_EQ(animation.current(), 3);
    
    C::set(20_s);
    animation.target(10, 7_s);
    EXPECT_EQ(animation.target(), 10);
    
    C::set(20_s);
    EXPECT_EQ(animation.current(), 3);
    
    C::set(21_s);
    EXPECT_EQ(animation.current(), 4);

    C::set(25_s);
    EXPECT_EQ(animation.current(), 8);

    C::set(27_s);
    EXPECT_EQ(animation.current(), 10);

    C::set(30_s);
    EXPECT_EQ(animation.current(), 10);
    
    animation.reset(15);
    
    EXPECT_EQ(animation.current(), 15);
    C::set(100_s);
    EXPECT_EQ(animation.current(), 15);
}

TEST(Animation, chain) {
    using C = Clock<std::chrono::seconds>;
    
    C::set(11_s);

    AnimationChain<int, C> animation(3);
    
    EXPECT_EQ(animation.current(), 3);
    
    C::set(20_s);
    animation.target(10, 7_s, 5, 3_s, 100, 10_s);
    
    C::set(20_s);
    EXPECT_EQ(animation.current(), 3);
    
    C::set(21_s);
    EXPECT_EQ(animation.current(), 4);

    C::set(25_s);
    EXPECT_EQ(animation.current(), 8);

    C::set(27_s);
    EXPECT_EQ(animation.current(), 10);

    C::set(30_s);
    EXPECT_EQ(animation.current(), 5);

    C::set(32_s);
    EXPECT_EQ(animation.current(), 24);

    C::set(40_s);
    EXPECT_EQ(animation.current(), 100);

    C::set(42_s);
    EXPECT_EQ(animation.current(), 100);

    animation.reset(15);
    
    EXPECT_EQ(animation.current(), 15);
    C::set(100_s);
    EXPECT_EQ(animation.current(), 15);
}

TEST(Animation, interpolation) {
    using C = Clock<std::chrono::seconds>;
    
    C::set(11_s);

    Animation<double, C> animation(3.0);
    
    EXPECT_FLOAT_EQ(animation.current(), 3.0);
    
    C::set(20_s);
    animation.target(10.0, 7_s, Interpolation::EaseInExpo);
    EXPECT_FLOAT_EQ(animation.target(), 10);
    
    C::set(20_s);
    EXPECT_NEAR(animation.current(), 3.0, 0.01);
    
    C::set(21_s);
    EXPECT_NEAR(animation.current(), 3.0, 0.1);

    C::set(24_s);
    EXPECT_NEAR(animation.current(), 3.3, 0.1);

    C::set(25_s);
    EXPECT_NEAR(animation.current(), 4.0, 0.1);

    C::set(26_s);
    EXPECT_NEAR(animation.current(), 5.6, 0.1);

    C::set(27_s);
    EXPECT_NEAR(animation.current(), 10.0, 0.01);    

    C::set(200_s);
    EXPECT_NEAR(animation.current(), 10.0, 0.01);    

    animation.target(10.0, 7_s, Interpolation::EaseOutExpo); // XXX: build fix for unused function

    // TODO: test values for EaseOutExpo
}
