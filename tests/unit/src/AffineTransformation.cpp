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
#include <okui/AffineTransformation.h>

#include <gtest/gtest.h>

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
