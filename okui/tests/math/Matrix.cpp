#include "gtest/gtest.h"

#include "onair/okui/math/Matrix.h"

using namespace onair;
using namespace onair::okui;
using namespace onair::okui::math;

TEST(Matrix, translation) {
    auto matrix = Matrix<int, 4, 4>::Translation(2, 3, 4);
    
    EXPECT_EQ(matrix(0, 0), 1);
    EXPECT_EQ(matrix(0, 1), 0);
    EXPECT_EQ(matrix(0, 2), 0);
    EXPECT_EQ(matrix(0, 3), 2);
    EXPECT_EQ(matrix(1, 0), 0);
    EXPECT_EQ(matrix(1, 1), 1);
    EXPECT_EQ(matrix(1, 2), 0);
    EXPECT_EQ(matrix(1, 3), 3);
    EXPECT_EQ(matrix(2, 0), 0);
    EXPECT_EQ(matrix(2, 1), 0);
    EXPECT_EQ(matrix(2, 2), 1);
    EXPECT_EQ(matrix(2, 3), 4);
    EXPECT_EQ(matrix(3, 0), 0);
    EXPECT_EQ(matrix(3, 1), 0);
    EXPECT_EQ(matrix(3, 2), 0);
    EXPECT_EQ(matrix(3, 3), 1);
}

TEST(Matrix, construction) {
    Matrix<int, 3, 3> matrix{
        2, -1, 1, 
        0, -2, 1, 
        1, -2, 0,
    };

    EXPECT_EQ(matrix(0, 0), 2);
    EXPECT_EQ(matrix(0, 1), -1);
    EXPECT_EQ(matrix(0, 2), 1);
    EXPECT_EQ(matrix(1, 0), 0);
    EXPECT_EQ(matrix(1, 1), -2);
    EXPECT_EQ(matrix(1, 2), 1);
    EXPECT_EQ(matrix(2, 0), 1);
    EXPECT_EQ(matrix(2, 1), -2);
    EXPECT_EQ(matrix(2, 2), 0);
}

TEST(Matrix, multiplication) {
    Matrix<int, 3, 3> a{
        2, -1, 1, 
        0, -2, 1, 
        1, -2, 0,
    };

    Matrix<int, 3, 3> b{
        -2,  3,  5, 
        -1, -3,  9, 
         5,  6, -7,
    };

    Matrix<int, 3, 3> expected{
        2, 15,  -6, 
        7, 12, -25, 
        0,  9, -13,
    };

    EXPECT_EQ(a * b, expected);
}