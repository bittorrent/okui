#include "gtest/gtest.h"

#include "../RenderOnce.h"
#include "../TestFramebuffer.h"

#include "onair/okui/shapes/Rectangle.h"

#if ONAIR_OKUI_HAS_NATIVE_APPLICATION && !OPENGL_ES // TODO: fix for OpenGL ES

using namespace onair;
using namespace onair::okui;

TEST(BoxShadowShader, basic) {
    RenderOnce([&] (View* view) {
        static constexpr auto radius = 5.0;
        
        TestFramebuffer framebuffer(320, 200);

        auto shader = view->boxShadowShader();
        shader->setTransformation(framebuffer.transformation());
        shader->setColor(1, 1, 1, 1);
        auto rect = Rectangle<double>(100, 100, 30, 40);
        shader->drawShadow(rect.x, rect.y, rect.width, rect.height, radius);
        shader->flush();

        framebuffer.finish();

        framebuffer.iteratePixels([&](int x, int y, Color pixel) {
            if (rect.contains(x, y)) {
                EXPECT_EQ(pixel, Color(1, 1, 1, 1));
            } else if (rect.distance(x,y) <= 4) {
                EXPECT_NE(pixel, Color(1, 1, 1, 1));
                EXPECT_NE(pixel, Color(0, 0, 0, 1));
            } else if (rect.distance(x,y) <= 6) {
                // ignore
            } else {
                EXPECT_EQ(pixel, Color(0, 0, 0, 1));
            }
        });
    });
}

TEST(BoxShadowShader, inverted) {
    RenderOnce([&] (View* view) {
        static constexpr auto radius = 5.0;
        
        TestFramebuffer framebuffer(320, 200);

        auto shader = view->boxShadowShader();
        shader->setTransformation(framebuffer.transformation());
        shader->setColor(1, 1, 1, 1);
        auto rect = Rectangle<double>(100, 100, 30, 40);
        shader->drawShadow(rect.x, rect.y, rect.width, rect.height, radius, true);
        shader->flush();

        framebuffer.finish();

        rect.x -= radius;
        rect.y -= radius;
        rect.width += radius*2;
        rect.height += radius*2;

        framebuffer.iteratePixels([&](int x, int y, Color pixel) {
            if (!rect.contains(x, y)) {
                EXPECT_EQ(pixel, Color(0, 0, 0, 1));
            } else if (rect.distance(x,y) >= -1) {
                // ignore
            } else if (rect.distance(x,y) >= -4) {
                EXPECT_NE(pixel, Color(1, 1, 1, 1));
                EXPECT_NE(pixel, Color(0, 0, 0, 1));
            } else if (rect.distance(x,y) <= -6) {
                EXPECT_EQ(pixel, Color(0, 0, 0, 1));
            }
        });
    });
}

#endif // ONAIR_OKUI_HAS_NATIVE_APPLICATION
