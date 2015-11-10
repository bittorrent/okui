#include "gtest/gtest.h"

#include "../RenderOnce.h"
#include "../TestFramebuffer.h"

#include "onair/okui/shapes/Circle.h"

using namespace onair;
using namespace onair::okui;

#if ONAIR_OKUI_HAS_NATIVE_APPLICATION

TEST(CircleShape, normal) {
    RenderOnce([&] (View* view) {
        TestFramebuffer framebuffer(320, 200);

        auto shader = view->colorShader();
        shader->setColor(1, 1, 1, 1);
        shader->setTransformation(framebuffer.transformation());

        Point<double> pos(12.0, 20.0);

        shapes::Circle(pos.x, pos.y, 7).draw(shader);
        shader->flush();

        framebuffer.finish();

        framebuffer.iteratePixels([&](int x, int y, Color pixel) {
            static constexpr auto aa = 1.5; // antialiasing epsilon
            auto distance = (Point<double>(static_cast<double>(x), static_cast<double>(y)) - pos).magnitude();

            if (distance < (7-aa)) {
                EXPECT_EQ(pixel, Color(1, 1, 1, 1));
            } else if (distance > (7+aa)) {
                EXPECT_EQ(pixel, Color(0, 0, 0, 1));
            }
        });
    });
}

#endif // ONAIR_OKUI_HAS_NATIVE_APPLICATION
