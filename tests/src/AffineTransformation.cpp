#include "gtest/gtest.h"

#include "okui/AffineTransformation.h"

using namespace okui;

TEST(AffineTransformation, translation) {
    double x, y;
    AffineTransformation::Translation(2.0, -5.0).transform(1.0, 1.0, &x, &y);
    EXPECT_FLOAT_EQ(x,  3.0);
    EXPECT_FLOAT_EQ(y, -4.0);
}

TEST(AffineTransformation, rotation) {
    double x, y;
    AffineTransformation::Rotation(M_PI).transform(1.0, 1.0, &x, &y);
    EXPECT_FLOAT_EQ(x, -1.0);
    EXPECT_FLOAT_EQ(y, -1.0);
}

TEST(AffineTransformation, scale) {
    double x, y;
    AffineTransformation::Scale(2.0, -4.0).transform(1.0, 1.0, &x, &y);
    EXPECT_FLOAT_EQ(x,  2.0);
    EXPECT_FLOAT_EQ(y, -4.0);
}

TEST(AffineTransformation, combination) {
    double x, y;

    // start with a translation
    AffineTransformation(0.0, 0.0, 3.0, 1.0).transform(1.0, 0.0, &x, &y);
    EXPECT_FLOAT_EQ(x, 4.0);
    EXPECT_FLOAT_EQ(y, 1.0);

    // add scaling
    AffineTransformation(0.0, 0.0, 3.0, 1.0, 2.0, 4.0).transform(1.0, 0.0, &x, &y);
    EXPECT_FLOAT_EQ(x, 8.0);
    EXPECT_FLOAT_EQ(y, 4.0);

    // add rotation
    AffineTransformation(0.0, 0.0, 3.0, 1.0, 2.0, 4.0, M_PI / 2).transform(1.0, 0.0, &x, &y);
    EXPECT_FLOAT_EQ(x, -4.0);
    EXPECT_FLOAT_EQ(y,  8.0);

    // add another translation
    AffineTransformation(-1.0, 3.0, 3.0, 1.0, 2.0, 4.0, M_PI / 2).transform(1.0, 0.0, &x, &y);
    EXPECT_FLOAT_EQ(x, -5.0);
    EXPECT_FLOAT_EQ(y, 11.0);
}
