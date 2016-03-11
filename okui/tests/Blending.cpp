#include "gtest/gtest.h"

#include "RenderOnce.h"
#include "TestFramebuffer.h"

#include "onair/okui/blending.h"
#include "onair/okui/shapes/Rectangle.h"

#if ONAIR_OKUI_HAS_NATIVE_APPLICATION && !OPENGL_ES // TODO: fix for OpenGL ES

using namespace onair;
using namespace onair::okui;

TEST(Blending, erasure) {
    RenderOnce([&](View* view) {
        auto width = view->bounds().width;
        auto height = view->bounds().height;

        TestFramebuffer framebuffer(width, height);
        glClearColor(0.0, 0.0, 0.0, 0.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        auto shader = view->colorShader();

        shader->setColor(1.0, 1.0, 0.0);
        shapes::Rectangle((width - height * 0.5) * 0.5, height * 0.25, height * 0.5, height * 0.5).round(height * 0.25).draw(shader);
        shader->flush();

        {
            Blending blending{BlendFunction::kErasure};
            shader->setColor(1.0, 0.0, 0.0, 0.0);
            shapes::Rectangle((width - height * 0.25) * 0.5, height * 0.375, height * 0.25, height * 0.25).round(height * 0.125).draw(shader);
            shader->flush();
        }

        framebuffer.finish();

        EXPECT_EQ(framebuffer.getPixel(width * 0.1, height * 0.5), Color(0.0, 0.0));
        EXPECT_EQ(framebuffer.getPixel(width * 0.5, height * 0.1), Color(0.0, 0.0));
        EXPECT_EQ(framebuffer.getPixel(width * 0.5, height * 0.5), Color(0.0, 1.0, 0.0));
        EXPECT_EQ(framebuffer.getPixel(width * 0.5, height * 0.30), Color(1.0, 1.0, 0.0));
    });
}

TEST(Blending, alphaLock) {
    RenderOnce([&](View* view) {
        auto width = view->bounds().width;
        auto height = view->bounds().height;

        TestFramebuffer framebuffer(width, height);
        glClearColor(0.0, 0.0, 0.0, 0.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        auto shader = view->colorShader();

        shader->setColor(1.0, 0.0, 0.0);
        shapes::Rectangle((width - height * 0.5) * 0.5, height * 0.25, height * 0.5, height * 0.5).round(height * 0.25).draw(shader);
        shader->flush();

        {
            Blending blending{BlendFunction::kAlphaLock};
            shader->setColor(0.0, 1.0, 0.0);
            shapes::Rectangle(0.0, height * 0.4, width, height * 0.2).draw(shader);
            shader->flush();
        }

        framebuffer.finish();

        EXPECT_EQ(framebuffer.getPixel(width * 0.1, height * 0.5), Color(0.0, 0.0));
        EXPECT_EQ(framebuffer.getPixel(width * 0.5, height * 0.1), Color(0.0, 0.0));
        EXPECT_EQ(framebuffer.getPixel(width * 0.5, height * 0.5), Color(0.0, 1.0, 0.0));
    });
}

#endif // ONAIR_OKUI_HAS_NATIVE_APPLICATION
