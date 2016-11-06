#include <okui/math/Vector.h>

#include <gtest/gtest.h>

using namespace okui;
using namespace okui::math;

TEST(Vector, twoComponent) {
    Vector<int8_t, 2> v;

    EXPECT_EQ(v.x, 0);
    EXPECT_EQ(v.y, 0);

    v.x = 5;
    v.y = -1;

    EXPECT_EQ(v.x, 5);
    EXPECT_EQ(v.y, -1);

    EXPECT_EQ(v[0], 5);
    EXPECT_EQ(v[1], -1);
}

TEST(Vector, scalar) {
    Vector<int, 3> v{1, 2, 3};
    v *= 2;

    EXPECT_EQ(v[0], 2);
    EXPECT_EQ(v[1], 4);
    EXPECT_EQ(v[2], 6);
}

TEST(Vector, scalarDivision) {
    Vector<int, 3> v{4, 6, 8};
    v /= 2;

    EXPECT_EQ(v[0], 2);
    EXPECT_EQ(v[1], 3);
    EXPECT_EQ(v[2], 4);
}

TEST(Vector, vectorAddition) {
    Vector<int, 3> a{4, 6, -1};
    Vector<int, 3> b{-1, 3, 2};
    auto c = a + b;

    EXPECT_EQ(c.x, 3);
    EXPECT_EQ(c.y, 9);
    EXPECT_EQ(c.z, 1);
}

TEST(Vector, magnitude) {
    Vector<double, 3> a{4.0, 6.0, -5.0};

    EXPECT_FLOAT_EQ(77.0, a.magnitudeSquared());
    EXPECT_FLOAT_EQ(8.774964387392123, a.magnitude());
}

TEST(Vector, normalize) {
    Vector<double, 3> a{4.0, 6.0, -5.0};

    EXPECT_FLOAT_EQ(8.774964387392123, a.magnitude());
    EXPECT_FLOAT_EQ(1.0, a.normalized().magnitude());
}
