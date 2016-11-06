#include "../RenderOnce.h"
#include "../TestFramebuffer.h"

#include <okui/Rectangle.h>
#include <okui/opengl/Framebuffer.h>
#include <okui/opengl/Stencil.h>
#include <okui/shapes/Rectangle.h>

#include <gtest/gtest.h>

using namespace okui;
using namespace okui::opengl;

#if ONAIR_OKUI_HAS_NATIVE_APPLICATION && !OPENGL_ES // TODO: fix for OpenGL ES

constexpr auto width = 320;
constexpr auto height = 200;

TEST(Stencil, normal) {
    RenderOnce([&] (View* view) {
        TestFramebuffer framebuffer(width, height);

        auto shader = view->colorShader();
        shader->setColor(Color::kWhite);
        shader->setTransformation(AffineTransformation{-1, 1, 0, 0, 2.0 / width, -2.0 / height});

        Stencil stencil(view->window());
        auto a = Rectangle<double>(10, 10, width - 20, height);
        shapes::Rectangle(a).draw(shader);
        shader->flush();
        stencil.activate();
        auto b = Rectangle<double>(0, 0, width, height - 100);
        shapes::Rectangle(b).draw(shader);

        shader->flush();

        framebuffer.finish();

        framebuffer.iteratePixels([&](int x, int y, const Color& pixel) {
            if (fabs(a.distance(x, y)) < 0.5 || fabs(b.distance(x, y)) < 0.5) {
                // let the edge pixels go either way
                return;
            }
            EXPECT_EQ(pixel, (b.contains(x, y) && !a.contains(x, y)) ? Color::kWhite : Color::kBlack);
        });
    });
}

TEST(Stencil, inversion) {
    RenderOnce([&] (View* view) {
        TestFramebuffer framebuffer(width, height);

        auto shader = view->colorShader();
        shader->setColor(Color::kWhite);
        shader->setTransformation(AffineTransformation{-1, 1, 0, 0, 2.0 / width, -2.0 / height});

        Stencil stencil(view->window());
        auto a = Rectangle<double>(10, 10, width - 20, height);
        shapes::Rectangle(a).draw(shader);
        shader->flush();
        stencil.activate(true);
        auto b = Rectangle<double>(0, 0, width, height - 100);
        shapes::Rectangle(b).draw(shader);

        shader->flush();

        framebuffer.finish();

        auto intersection = a.intersection(b);
        framebuffer.iteratePixels([&](int x, int y, const Color& pixel) {
            if (fabs(intersection.distance(x, y)) < 0.5) {
                // let the edge pixels go either way
                return;
            }
            EXPECT_EQ(pixel, intersection.contains(x, y) ? Color::kWhite : Color::kBlack);
        });
    });
}

#endif
