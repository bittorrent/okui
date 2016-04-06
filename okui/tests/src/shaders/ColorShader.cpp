#include "gtest/gtest.h"

#include "../RenderOnce.h"
#include "../TestFramebuffer.h"

#include "onair/okui/shapes/Rectangle.h"

#if ONAIR_OKUI_HAS_NATIVE_APPLICATION && !OPENGL_ES // TODO: fix for OpenGL ES

using namespace scraps;
using namespace onair::okui;

TEST(ColorShader, basic) {
    RenderOnce([&] (View* view) {
        TestFramebuffer framebuffer(320, 200);

        auto shader = view->colorShader();
        shader->setColor(1, 1, 1, 1);
        shader->setTransformation(framebuffer.transformation());

        auto rect = Rectangle<double>(2, 13, 11, 123);
        shapes::Rectangle(rect).draw(shader);
        shader->flush();

        framebuffer.finish();

        framebuffer.iteratePixels([&](int x, int y, Color pixel) {
        	EXPECT_EQ(pixel, rect.contains(x, y) ? Color(1, 1, 1, 1) : Color(0, 0, 0, 1));
        });
    });
}

TEST(ColorShader, gradient) {
    RenderOnce([&] (View* view) {
        TestFramebuffer framebuffer(320, 200);

        auto shader = view->colorShader();
        shader->setTransformation(framebuffer.transformation());
        shader->setColorA(2, 13, 1, 0, 0, 1);
        shader->setColorB(13, 136, 0, 0, 1, 1);

        auto rect = Rectangle<double>(2, 13, 11, 123);
        shapes::Rectangle(rect).draw(shader);
        shader->flush();

        framebuffer.finish();

        auto pixel = framebuffer.getPixel(2, 13);
        EXPECT_NEAR(pixel.r, 1, 0.01);
        EXPECT_EQ(pixel.g, 0);
        EXPECT_NEAR(pixel.b, 0, 0.01);
        EXPECT_EQ(pixel.a, 1);

        pixel = framebuffer.getPixel(12, 135);
        EXPECT_NEAR(pixel.r, 0, 0.01);
        EXPECT_EQ(pixel.g, 0);
        EXPECT_NEAR(pixel.b, 1, 0.01);
        EXPECT_EQ(pixel.a, 1);
        
        pixel = framebuffer.getPixel(7, 74);
        EXPECT_NEAR(pixel.r, 0.5, 0.01);
        EXPECT_EQ(pixel.g, 0);
        EXPECT_NEAR(pixel.b, 0.5, 0.01);
        EXPECT_EQ(pixel.a, 1);
    });
}

#endif // ONAIR_OKUI_HAS_NATIVE_APPLICATION
