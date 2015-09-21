#pragma once
#include "onair/okui/opengl/Framebuffer.h"
#include "onair/okui/Color.h"
#include "onair/okui/AffineTransformation.h"

#include <vector>

struct TestFramebuffer {
    TestFramebuffer(int width, int height) {
        colorAttachment = framebuffer.addColorAttachment(width, height);
        framebuffer.addDepthStencilAttachment(width, height);
        EXPECT_TRUE(framebuffer.isComplete());
        framebuffer.bind();
        glViewport(0, 0, width, height);
        glClearColor(0.0, 0.0, 0.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    }

    void finish() {
        glFinish();

        pixels.resize(colorAttachment->width()*colorAttachment->height()*4);

        glBindTexture(GL_TEXTURE_2D, colorAttachment->texture());
        glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
    }

    onair::okui::Color getPixel(size_t x, size_t y) {
        auto* p = &pixels[((colorAttachment->height()-y-1)*colorAttachment->width() + x)*4];
        return {p[0]/255.0, p[1]/255.0, p[2]/255.0, p[3]/255.0};
    }

    template<typename F> // F policy: void(int x, int y, Color pixel)
    void iteratePixels(F f) {
        for (int y = 0; y < colorAttachment->height(); ++y) {
            for (int x = 0; x < colorAttachment->width(); ++x) {
                f(x, y, getPixel(x, y));
            }
        }
    }

    onair::okui::AffineTransformation transformation() const {
        return {-1, 1, 0, 0, 2.0 / colorAttachment->width(), -2.0 / colorAttachment->height()};
    }

    onair::okui::opengl::Framebuffer framebuffer;
    onair::okui::opengl::Framebuffer::Attachment* colorAttachment = nullptr;
    std::vector<uint8_t> pixels;
};
