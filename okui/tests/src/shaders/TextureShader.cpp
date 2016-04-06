#include "gtest/gtest.h"

#include "../RenderOnce.h"
#include "../TestFramebuffer.h"

#include "onair/okui/shapes/Rectangle.h"

#if ONAIR_OKUI_HAS_NATIVE_APPLICATION && !OPENGL_ES // TODO: fix for OpenGL ES

using namespace scraps;
using namespace onair::okui;

namespace {
    // basn2c16 from http://www.schaik.com/pngsuite/pngsuite_bas_png.html
    // 32x32
    static const unsigned char kImageData[] = {
        0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A, 0x00, 0x00, 0x00, 0x0D, 0x49, 0x48, 0x44, 0x52,
        0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x20, 0x10, 0x02, 0x00, 0x00, 0x00, 0xAC, 0x88, 0x31,
        0xE0, 0x00, 0x00, 0x00, 0x04, 0x67, 0x41, 0x4D, 0x41, 0x00, 0x01, 0x86, 0xA0, 0x31, 0xE8, 0x96,
        0x5F, 0x00, 0x00, 0x00, 0xE5, 0x49, 0x44, 0x41, 0x54, 0x78, 0x9C, 0xD5, 0x96, 0xC1, 0x0A, 0x83,
        0x30, 0x10, 0x44, 0xA7, 0xE0, 0x41, 0x7F, 0xCB, 0x7E, 0xB7, 0xFD, 0xAD, 0xF6, 0x96, 0x1E, 0x06,
        0x03, 0x92, 0x86, 0x26, 0x66, 0x93, 0xCC, 0x7A, 0x18, 0x86, 0x45, 0xE4, 0x3D, 0xD6, 0xA0, 0x8F,
        0x10, 0x42, 0x00, 0x3E, 0x2F, 0xE0, 0x9A, 0xEF, 0x64, 0x72, 0x73, 0x7E, 0x18, 0x3D, 0x27, 0x33,
        0x5F, 0xCE, 0xE2, 0xF3, 0x5A, 0x77, 0xB7, 0x02, 0xEB, 0xCE, 0x74, 0x28, 0x70, 0xA2, 0x33, 0x97,
        0xF3, 0xED, 0xF2, 0x70, 0x5D, 0xD1, 0x01, 0x60, 0xF3, 0xB2, 0x81, 0x5F, 0xE8, 0xEC, 0xF2, 0x02,
        0x79, 0x74, 0xA6, 0xB0, 0xC0, 0x3F, 0x74, 0xA6, 0xE4, 0x19, 0x28, 0x43, 0xE7, 0x5C, 0x6C, 0x03,
        0x35, 0xE8, 0xEC, 0x32, 0x02, 0xF5, 0xE8, 0x4C, 0x01, 0x81, 0xBB, 0xE8, 0xCC, 0xA9, 0x67, 0xA0,
        0x0D, 0x9D, 0xF3, 0x49, 0x1B, 0xB0, 0x40, 0x67, 0x1F, 0x2E, 0x60, 0x87, 0xCE, 0x1C, 0x28, 0x60,
        0x8D, 0x1E, 0x05, 0xF8, 0xC7, 0xEE, 0x0F, 0x1D, 0x00, 0xB6, 0x67, 0xE7, 0x0D, 0xF4, 0x44, 0x67,
        0xEF, 0x26, 0xD0, 0x1F, 0xBD, 0x9B, 0xC0, 0x28, 0xF4, 0x28, 0x60, 0xF7, 0x1D, 0x18, 0x8B, 0xCE,
        0xFB, 0x8D, 0x36, 0x30, 0x03, 0x9D, 0xBD, 0x59, 0x60, 0x1E, 0x7A, 0xB3, 0xC0, 0x6C, 0xF4, 0x28,
        0x50, 0x7F, 0x06, 0x34, 0xD0, 0x39, 0xAF, 0xDC, 0x80, 0x12, 0x3A, 0x7B, 0xB1, 0x80, 0x1E, 0x7A,
        0xB1, 0x80, 0x2A, 0x7A, 0x14, 0xC8, 0x9F, 0x01, 0x6D, 0x74, 0xCE, 0x33, 0x1B, 0xF0, 0x80, 0xCE,
        0x9E, 0x08, 0xF8, 0x41, 0x4F, 0x04, 0xBC, 0xA1, 0x33, 0xBF, 0xE6, 0x42, 0xFE, 0x5E, 0x07, 0xBB,
        0xC4, 0xEC, 0x00, 0x00, 0x00, 0x00, 0x49, 0x45, 0x4E, 0x44, 0xAE, 0x42, 0x60, 0x82,
    };
}


TEST(TextureShader, scaledFill) {
    std::shared_ptr<Texture> texture;

    RenderOnce([&](View* view) {
        texture = view->loadTextureFromMemory(std::make_shared<std::string>((const char*)kImageData, sizeof(kImageData)));
        EXPECT_NE(texture, nullptr);
        EXPECT_FALSE(texture->isLoaded());
    },
    [&](View* view) {
        EXPECT_TRUE(texture->isLoaded());

        TestFramebuffer framebuffer(320, 200);

        auto shader = view->textureShader();
        shader->setTransformation(framebuffer.transformation());

        shader->drawScaledFill(*texture, 20, 20, 100, 130);

        shader->flush();

        framebuffer.finish();

        auto pixel = framebuffer.getPixel(19, 19);
        EXPECT_EQ(pixel, Color(0, 0, 0, 1));

        pixel = framebuffer.getPixel(20, 20);
        EXPECT_LT(pixel.r, 1);
        EXPECT_GT(pixel.r, 0.5);
        EXPECT_EQ(pixel.g, 1);
        EXPECT_EQ(pixel.b, 0);

        pixel = framebuffer.getPixel(119, 20);
        EXPECT_LT(pixel.r, 0.5);
        EXPECT_GT(pixel.r, 0);
        EXPECT_EQ(pixel.g, 1);
        EXPECT_EQ(pixel.b, 0);

        pixel = framebuffer.getPixel(20, 149);
        EXPECT_LT(pixel.r, 1);
        EXPECT_GT(pixel.r, 0.5);
        EXPECT_EQ(pixel.g, 0);
        EXPECT_LT(pixel.b, 0.5);
        EXPECT_GT(pixel.b, 0);

        pixel = framebuffer.getPixel(119, 149);
        EXPECT_LT(pixel.r, 0.5);
        EXPECT_GT(pixel.r, 0);
        EXPECT_EQ(pixel.g, 0);
        EXPECT_LT(pixel.b, 1);
        EXPECT_GT(pixel.b, 0.5);

        pixel = framebuffer.getPixel(50, 150);
        EXPECT_EQ(pixel, Color(0, 0, 0, 1));
    });
}

TEST(TextureShader, scaledFit) {
    std::shared_ptr<Texture> texture;

    RenderOnce([&](View* view) {
        texture = view->loadTextureFromMemory(std::make_shared<std::string>((const char*)kImageData, sizeof(kImageData)));
        EXPECT_NE(texture, nullptr);
        EXPECT_FALSE(texture->isLoaded());
    },
    [&](View* view) {
        EXPECT_TRUE(texture->isLoaded());

        TestFramebuffer framebuffer(320, 200);

        auto shader = view->textureShader();
        shader->setTransformation(framebuffer.transformation());

        shader->drawScaledFit(*texture, 20, 20, 100, 130);

        shader->flush();

        framebuffer.finish();

        EXPECT_EQ(framebuffer.getPixel(30, 34), Color(0, 0, 0, 1));
        EXPECT_EQ(framebuffer.getPixel(30, 137), Color(0, 0, 0, 1));
        EXPECT_NE(framebuffer.getPixel(30, 40), Color(0, 0, 0, 1));
        EXPECT_NE(framebuffer.getPixel(30, 125), Color(0, 0, 0, 1));
    });
}

TEST(TextureShader, color) {
    std::shared_ptr<Texture> texture;

    RenderOnce([&](View* view) {
        texture = view->loadTextureFromMemory(std::make_shared<std::string>((const char*)kImageData, sizeof(kImageData)));
        EXPECT_NE(texture, nullptr);
        EXPECT_FALSE(texture->isLoaded());
    },
    [&](View* view) {
        EXPECT_TRUE(texture->isLoaded());

        TestFramebuffer framebuffer(320, 200);

        auto shader = view->textureShader();
        shader->setTransformation(framebuffer.transformation());
        shader->setColor(0.5, 0.25, 0, 1.0);

        shader->drawScaledFit(*texture, 20, 20, 100, 130);

        shader->flush();

        framebuffer.finish();

        auto pixel = framebuffer.getPixel(19, 20);
        EXPECT_EQ(pixel, Color(0, 0, 0, 1));

        pixel = framebuffer.getPixel(22, 40);
        EXPECT_NEAR(pixel.r, 0.5, 0.1);
        EXPECT_NEAR(pixel.g, 0.25, 0.1);
        EXPECT_NEAR(pixel.b, 0, 0.1);
        EXPECT_EQ(pixel.a, 1);

        pixel = framebuffer.getPixel(117, 40);
        EXPECT_NEAR(pixel.r, 0, 0.1);
        EXPECT_NEAR(pixel.g, 0.25, 0.1);
        EXPECT_NEAR(pixel.b, 0, 0.1);
        EXPECT_EQ(pixel.a, 1);

        pixel = framebuffer.getPixel(22, 132);
        EXPECT_NEAR(pixel.r, 0.5, 0.1);
        EXPECT_NEAR(pixel.g, 0, 0.1);
        EXPECT_NEAR(pixel.b, 0, 0.1);
        EXPECT_EQ(pixel.a, 1);

        pixel = framebuffer.getPixel(50, 137);
        EXPECT_EQ(pixel, Color(0, 0, 0, 1));
    });
}

#endif // ONAIR_OKUI_HAS_NATIVE_APPLICATION
