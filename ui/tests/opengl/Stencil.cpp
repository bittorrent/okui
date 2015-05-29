#include "gtest/gtest.h"

#include "../RenderOnce.h"
#include "bittorrent/ui/Rectangle.h"
#include "bittorrent/ui/opengl/Framebuffer.h"
#include "bittorrent/ui/opengl/Stencil.h"
#include "bittorrent/ui/shapes/Rectangle.h"

using namespace bittorrent;
using namespace bittorrent::ui;
using namespace bittorrent::ui::opengl;

struct Pixel {
    Pixel() {}
    Pixel(uint8_t r, uint8_t g, uint8_t b, uint8_t a) : r(r), g(g), b(b), a(a) {}
    uint8_t r{0}, g{0}, b{0}, a{255};
    bool operator==(const Pixel& other) const {
        return r == other.r && g == other.g && b == other.b && a == other.a;
    }
};

struct TestFramebuffer {
    TestFramebuffer() {
        auto width = 400;
        auto height = 640;
        colorAttachment = framebuffer.addColorAttachment(width, height);
        framebuffer.addDepthStencilAttachment(width, height);
        EXPECT_TRUE(framebuffer.isComplete());
        framebuffer.bind();
        glViewport(0, 0, width, height);
        glClearColor(0.0, 0.0, 0.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    }
    
    GLuint texture() const { return colorAttachment->texture(); }
    int width() const { return colorAttachment->width(); }
    int height() const { return colorAttachment->height(); }

    void iteratePixels(std::function<void(int, int, const Pixel&)> f) {
        glFinish();
        
        std::vector<Pixel> pixels;
        pixels.resize(width() * height());            
        
        glBindTexture(GL_TEXTURE_2D, texture());
        glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
        
        for (int y = 0; y < height(); ++y) {
            for (int x = 0; x < width(); ++x) {
                f(x, y, pixels[x + width() * (height() - y)]);
            }
        }
    }
    
    Framebuffer framebuffer;
    Framebuffer::Attachment* colorAttachment = nullptr;
};

TEST(Stencil, normal) {
    RenderOnce([&] (View* view) {
        TestFramebuffer framebuffer;

        auto shader = view->colorShader();
        shader->setColor(1.0, 1.0, 1.0, 1.0);
        shader->setTransformation(AffineTransformation{-1, 1, 0, 0, 2.0 / framebuffer.width(), -2.0 / framebuffer.height()});

        Stencil stencil(view->window());
        auto a = Rectangle<double>(10, 10, framebuffer.width() - 20, framebuffer.height());
        shapes::Rectangle(a).draw(shader);
        shader->flush();
        stencil.activate();
        auto b = Rectangle<double>(0, 0, framebuffer.width(), framebuffer.height() - 100);
        shapes::Rectangle(b).draw(shader);

        shader->flush();

        framebuffer.iteratePixels([&](int x, int y, const Pixel& pixel) {
            if (fabs(a.distance(x, y)) < 0.5 || fabs(b.distance(x, y)) < 0.5) {
                // let the edge pixels go either way
                return;
            }
            EXPECT_EQ(pixel, (b.contains(x, y) && !a.contains(x, y)) ? Pixel(255, 255, 255, 255) : Pixel(0, 0, 0, 255));
        });
    });
}

TEST(Stencil, inversion) {
    RenderOnce([&] (View* view) {return;
        TestFramebuffer framebuffer;

        auto shader = view->colorShader();
        shader->setColor(1.0, 1.0, 1.0, 1.0);
        shader->setTransformation(AffineTransformation{-1, 1, 0, 0, 2.0 / framebuffer.width(), -2.0 / framebuffer.height()});

        Stencil stencil(view->window());
        auto a = Rectangle<double>(10, 10, framebuffer.width() - 20, framebuffer.height());
        shapes::Rectangle(a).draw(shader);
        shader->flush();
        stencil.activate(true);
        auto b = Rectangle<double>(0, 0, framebuffer.width(), framebuffer.height() - 100);
        shapes::Rectangle(b).draw(shader);

        shader->flush();

        auto intersection = a.intersection(b);
        framebuffer.iteratePixels([&](int x, int y, const Pixel& pixel) {
            if (fabs(intersection.distance(x, y)) < 0.5) {
                // let the edge pixels go either way
                return;
            }
            EXPECT_EQ(pixel, intersection.contains(x, y) ? Pixel(255, 255, 255, 255) : Pixel(0, 0, 0, 255));
        });
    });
}
