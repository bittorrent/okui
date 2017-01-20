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

#include <okui/shapes/Circle.h>

#include <gtest/gtest.h>

using namespace okui;

#if ONAIR_OKUI_HAS_NATIVE_APPLICATION && !OPENGL_ES // TODO: fix for OpenGL ES

TEST(CircleShape, normal) {
    RenderOnce([&] (View* view) {
        TestFramebuffer framebuffer(320, 200);

        auto shader = view->colorShader();
        shader->setColor(Color::kWhite);
        shader->setTransformation(framebuffer.transformation());

        Point<double> pos(12.0, 20.0);

        shapes::Circle(pos.x, pos.y, 7).draw(shader);
        shader->flush();

        framebuffer.finish();

        framebuffer.iteratePixels([&](int x, int y, Color pixel) {
            static constexpr auto aa = 1.5; // antialiasing epsilon
            auto distance = (Point<double>(static_cast<double>(x), static_cast<double>(y)) - pos).magnitude();

            if (distance < (7-aa)) {
                EXPECT_EQ(pixel, Color::kWhite);
            } else if (distance > (7+aa)) {
                EXPECT_EQ(pixel, Color::kBlack);
            }
        });
    });
}

#endif // ONAIR_OKUI_HAS_NATIVE_APPLICATION
