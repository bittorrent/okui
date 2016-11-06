#include "../RenderOnce.h"
#include "../TestFramebuffer.h"

#include <okui/shapes/Rectangle.h>

#include <gtest/gtest.h>

#if ONAIR_OKUI_HAS_NATIVE_APPLICATION && !OPENGL_ES // TODO: fix for OpenGL ES

using namespace okui;

TEST(RectangleShape, normal) {
    RenderOnce([&] (View* view) {
        TestFramebuffer framebuffer(320, 200);

        auto shader = view->colorShader();
        shader->setColor(Color::kWhite);
        shader->setTransformation(framebuffer.transformation());

        auto rect = Rectangle<double>(2, 13, 11, 123);
        shapes::Rectangle(rect).draw(shader);
        shader->flush();

        framebuffer.finish();

        framebuffer.iteratePixels([&](int x, int y, Color pixel) {
            EXPECT_EQ(pixel, rect.contains(x, y) ? Color::kWhite : Color::kBlack);
        });
    });
}

TEST(RectangleShape, round) {
    RenderOnce([&] (View* view) {
        TestFramebuffer framebuffer(320,200);

        auto shader = view->colorShader();
        shader->setColor(Color::kWhite);
        shader->setTransformation(framebuffer.transformation());

        auto rect = Rectangle<double>(2, 13, 11, 123);
        shapes::Rectangle(rect).round(3).draw(shader);
        shader->flush();

        framebuffer.finish();

        //  Check that the corners aren't written to
        EXPECT_EQ(framebuffer.getPixel(rect.minX(), rect.minY()), Color::kBlack);
        EXPECT_EQ(framebuffer.getPixel(rect.maxX(), rect.minY()), Color::kBlack);
        EXPECT_EQ(framebuffer.getPixel(rect.minX(), rect.maxY()), Color::kBlack);
        EXPECT_EQ(framebuffer.getPixel(rect.maxX(), rect.maxY()), Color::kBlack);

        //  Check that the main area is written to
        auto rect1 = Rectangle<double>(2, 13+3, 11, 123-6);
        auto rect2 = Rectangle<double>(2+3, 13, 11-6, 123);
        framebuffer.iteratePixels([&](int x, int y, const Color& pixel) {
            if (rect1.contains(x, y) || rect2.contains(x, y)) {
                EXPECT_EQ(pixel, Color::kWhite);
            }
        });
    });
}

TEST(RectangleShape, rotate) {
    RenderOnce([&] (View* view) {
        TestFramebuffer framebuffer(320, 200);

        auto shader = view->colorShader();
        shader->setColor(Color::kWhite);
        shader->setTransformation(framebuffer.transformation());

        auto rect = Rectangle<double>(20, 3, 15, 10);
        shapes::Rectangle(rect).rotate(M_PI/2, 20, 3).draw(shader);
        shader->flush();

        framebuffer.finish();

        auto rotatedRect = Rectangle<double>(10, 3, 10, 15);

        framebuffer.iteratePixels([&](int x, int y, Color pixel) {
            EXPECT_EQ(pixel, rotatedRect.contains(x, y) ? Color::kWhite : Color::kBlack);
        });
    });
}

#endif // ONAIR_OKUI_HAS_NATIVE_APPLICATION
