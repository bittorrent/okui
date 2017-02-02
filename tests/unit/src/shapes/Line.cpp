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
#include "../RenderOnce.h"
#include "../TestFramebuffer.h"

#include <okui/shapes/Line.h>

#include <gtest/gtest.h>

#if ONAIR_OKUI_HAS_NATIVE_APPLICATION && !OPENGL_ES // TODO: fix for OpenGL ES

using namespace okui;

TEST(LineShape, normal) {
    RenderOnce([&] (View* view) {
        TestFramebuffer framebuffer(320, 200);

        auto shader = view->colorShader();
        shader->setColor(Color::kWhite);
        shader->setTransformation(framebuffer.transformation());

        Point<double> p1(12.0, 20.0);
        Point<double> p2(123.0, 66.0);

        shapes::Line(p1.x, p1.y, p2.x, p2.y, 5).draw(shader);
        shader->flush();

        framebuffer.finish();

        framebuffer.iteratePixels([&](int x, int y, Color pixel) {
            static constexpr auto aa = 1.0; // antialiasing buffer
            auto p = Point<double>((double)x, (double)y);
            auto t = (p - p1).dot(p2 - p1) / (p2 - p1).magnitudeSquared();

            if (t < 0) {        // before the start
                if ((p1-p).magnitude() < aa) {
                    EXPECT_EQ(pixel, Color::kBlack);
                }
            } else if (t > 1) { // after the end
                if ((p-p2).magnitude() < aa) {
                    EXPECT_EQ(pixel, Color::kBlack);
                }
            } else {
                auto proj = p1 + (p2-p1) * t;
                auto distance = (proj-p).magnitude();
                if (distance < (2.5-aa) && (p-p2).magnitude() < aa && (p1-p).magnitude() < aa) {
                    EXPECT_EQ(pixel, Color::kWhite);
                }
            }
        });
    });
}

#endif // ONAIR_OKUI_HAS_NATIVE_APPLICATION
