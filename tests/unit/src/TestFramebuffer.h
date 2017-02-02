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
#pragma once

#include <okui/config.h>

#include <okui/opengl/Framebuffer.h>
#include <okui/Color.h>
#include <okui/AffineTransformation.h>

#include <gtest/gtest.h>

#include <vector>

#if !OPENGL_ES // TODO: fix for OpenGL ES

using namespace okui;

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

    okui::Color getPixel(size_t x, size_t y) {
        auto* p = &pixels[((colorAttachment->height()-y-1)*colorAttachment->width() + x)*4];
        return RGBA(p[0], p[1], p[2], p[3]);
    }

    template<typename F> // F policy: void(int x, int y, Color pixel)
    void iteratePixels(F f) {
        for (int y = 0; y < colorAttachment->height(); ++y) {
            for (int x = 0; x < colorAttachment->width(); ++x) {
                f(x, y, getPixel(x, y));
            }
        }
    }

    okui::AffineTransformation transformation() const {
        return {-1, 1, 0, 0, 2.0 / colorAttachment->width(), -2.0 / colorAttachment->height()};
    }

    okui::opengl::Framebuffer framebuffer;
    okui::opengl::Framebuffer::Attachment* colorAttachment = nullptr;
    std::vector<uint8_t> pixels;
};

#endif
