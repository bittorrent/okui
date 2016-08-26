#include "gtest/gtest.h"

#include "okui/Rectangle.h"
#include "okui/interpolation.h"

using namespace okui;

TEST(Rectangle, construction) {
    Rectangle<int> r(1, 2, 3, 4);
    EXPECT_EQ(r.x, 1);
    EXPECT_EQ(r.y, 2);
    EXPECT_EQ(r.width, 3);
    EXPECT_EQ(r.height, 4);
}

TEST(Rectangle, defaultConstruction) {
    Rectangle<int> r;
    EXPECT_EQ(r.x, 0);
    EXPECT_EQ(r.y, 0);
    EXPECT_EQ(r.width, 0);
    EXPECT_EQ(r.height, 0);
}

TEST(Rectangle, position) {
    Rectangle<int> r(1, 2, 3, 4);
    EXPECT_EQ(r.position().x, 1);
    EXPECT_EQ(r.position().y, 2);
}

TEST(Rectangle, size) {
    Rectangle<int> r(1, 2, 3, 4);
    EXPECT_EQ(r.size().x, 3);
    EXPECT_EQ(r.size().y, 4);
}

TEST(Rectangle, contains) {
    Rectangle<int> r(1, 2, 3, 4);

    EXPECT_FALSE(r.contains(5, 10));
    EXPECT_TRUE(r.contains(1, 2));
    EXPECT_FALSE(r.contains(1, 10));
    EXPECT_TRUE(r.contains(2, 3));
    EXPECT_FALSE(r.contains(4, 6));
    EXPECT_FALSE(r.contains(4, 3));
    EXPECT_FALSE(r.contains(2, 6));
}

TEST(Rectangle, pointDistance) {
    Rectangle<int> r(1, 2, 3, 4);

    EXPECT_NEAR(r.distance(0, 0), sqrt(5.0), 0.01);
    EXPECT_NEAR(r.distance(7, 2), 3.0, 0.01);

    EXPECT_NEAR(r.distance(2, 2), 0.0, 0.01);

    EXPECT_NEAR(r.distance(2, 4), -1.0, 0.01);
}

TEST(Rectangle, rectangleDistance) {
    EXPECT_EQ(Rectangle<int>(10, 10, 100, 200).distance(Rectangle<int>(10, 10 + 275, 100, 100)), 75);
    EXPECT_EQ(Rectangle<int>(10, 10, 100, 200).distance(Rectangle<int>(10 + 300, 10, 100, 100)), 200);
    EXPECT_EQ(Rectangle<int>(10, 10, 100, 200).distance(Rectangle<int>(10 + 300, 10 + 275, 100, 100)), sqrt(75*75 + 200*200));
}

TEST(Rectangle, difference) {
    auto a = Rectangle<int>(10, 10, 100, 200);
    auto b = Rectangle<int>(0, 40, 100, 100);
    auto diff = a - b;
    bool contained = false;
    for (auto& r : diff) {
        EXPECT_FALSE(r.contains(10, 50));
        contained |= r.contains(10, 30);
    }
    EXPECT_TRUE(contained);
}

TEST(Rectangle, intersection) {
    {
        Rectangle<int> a(1, 2, 3, 4);
        Rectangle<int> b(2, 1, 3, 4);

        EXPECT_TRUE(a.intersects(b));
    }

    {
        Rectangle<int> a(1, 2, 3, 4);
        Rectangle<int> b(0, 1, 3, 4);

        EXPECT_TRUE(a.intersects(b));
    }

    {
        Rectangle<int> a(1, 2, 3, 4);
        Rectangle<int> b(0, 0, 1, 2);

        EXPECT_FALSE(a.intersects(b));
        EXPECT_EQ(a.intersection(b), Rectangle<int>());
    }

    {
        Rectangle<int> a(-1, -2, 3, 4);
        Rectangle<int> b(0, 0, 1, 2);

        EXPECT_TRUE(a.intersects(b));
    }

    {
        Rectangle<int> a(-1, -2, 3, 4);
        Rectangle<int> b(10, 10, 1, 2);

        EXPECT_FALSE(a.intersects(b));
        EXPECT_EQ(a.intersection(b), Rectangle<int>());
    }

    {
        Rectangle<int> a(1, 2, 3, 4);
        Rectangle<int> b(2, 1, 3, 4);

        EXPECT_EQ(a.intersection(b), Rectangle<int>(2, 2, 2, 3));
    }

    {
        Rectangle<int> a(270, 760, 500, 0);
        Rectangle<int> b(270, 760, 2000, 600);

        EXPECT_FALSE(a.intersects(b));
        EXPECT_EQ(a.intersection(b), Rectangle<int>());
    }

    {
        Rectangle<int> a(770, 0, 1130, 500);
        Rectangle<int> b(270, 0, 2230, 1400);

        EXPECT_EQ(a.intersection(b), Rectangle<int>(770, 0, 1130, 500));
    }
}

TEST(Rectangle, interpolation) {
    EXPECT_EQ(Rectangle<int>(10, 10, 100, 200).interpolate(Rectangle<int>(-110, -310, 50, 300), 0.0, interpolation::Linear<int>), Rectangle<int>(10, 10, 100, 200));
    EXPECT_EQ(Rectangle<int>(10, 10, 100, 200).interpolate(Rectangle<int>(-110, -310, 50, 300), 1.0, interpolation::Linear<int>), Rectangle<int>(-110, -310, 50, 300));
    EXPECT_EQ(Rectangle<int>(10, 10, 100, 200).interpolate(Rectangle<int>(-110, -310, 50, 300), 0.7, interpolation::Linear<int>), Rectangle<int>(-74, -214, 65, 270));
}

TEST(Rectangle, scaledFill) {
    EXPECT_EQ(Rectangle<int>(10, 10, 100, 200).scaledFill(2.0), Rectangle<double>(10-150, 10, 400, 200));
}

TEST(Rectangle, scaledFit) {
    EXPECT_EQ(Rectangle<int>(10, 10, 100, 200).scaledFit(2.0), Rectangle<double>(10, 10+75, 100, 50));
}
