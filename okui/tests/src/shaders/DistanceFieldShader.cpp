#include "gtest/gtest.h"

#include "../RenderOnce.h"
#include "../TestFramebuffer.h"

#if ONAIR_OKUI_HAS_NATIVE_APPLICATION && !OPENGL_ES // TODO: fix for OpenGL ES

using namespace scraps;
using namespace onair::okui;


TEST(DistanceFieldShader, edgeZero) {
    std::shared_ptr<Texture> texture;

    RenderOnce([&](View* view) {
        texture = view->loadTextureResource("PlayIcon.png");
        EXPECT_NE(texture, nullptr);
        EXPECT_FALSE(texture->isLoaded());
    },
    [&](View* view) { // edge 0
        EXPECT_TRUE(texture->isLoaded());

        TestFramebuffer framebuffer(320, 200);

        auto shader = view->distanceFieldShader();
        shader->setTransformation(framebuffer.transformation());
        shader->setEdge(0);

        shader->drawScaledFill(*texture, 0, 0, 128, 128);

        shader->flush();

        framebuffer.finish();

        // top left
        auto pixel = framebuffer.getPixel(19, 19);
        EXPECT_EQ(pixel, Color(0, 0, 0, 1));
        pixel = framebuffer.getPixel(20, 20);
        EXPECT_GT(pixel.r, 0.5);
        EXPECT_LT(pixel.r, 1);
        pixel = framebuffer.getPixel(21, 21);
        EXPECT_EQ(pixel, Color(1, 1, 1, 1));
        pixel = framebuffer.getPixel(28, 28);
        EXPECT_EQ(pixel, Color(1, 1, 1, 1));

        // bottom right
        pixel = framebuffer.getPixel(69, 89);
        EXPECT_EQ(pixel, Color(1, 1, 1, 1));
        pixel = framebuffer.getPixel(70, 90);
        EXPECT_GT(pixel.r, 0);
        pixel = framebuffer.getPixel(72, 92);
        EXPECT_EQ(pixel, Color(0, 0, 0, 1));
    });
}

TEST(DistanceFieldShader, edgeHalf) {
    std::shared_ptr<Texture> texture;

    RenderOnce([&](View* view) {
        texture = view->loadTextureResource("PlayIcon.png");
        EXPECT_NE(texture, nullptr);
        EXPECT_FALSE(texture->isLoaded());
    },
    [&](View* view) { // edge 0.5
        EXPECT_TRUE(texture->isLoaded());

        TestFramebuffer framebuffer(320, 200);

        auto shader = view->distanceFieldShader();
        shader->setTransformation(framebuffer.transformation());
        shader->setEdge(0.5);

        shader->drawScaledFill(*texture, 0, 0, 128, 128);

        shader->flush();

        framebuffer.finish();

        // top left
        auto pixel = framebuffer.getPixel(26, 26);
        EXPECT_EQ(pixel, Color(0, 0, 0, 1));
        pixel = framebuffer.getPixel(28, 28);
        EXPECT_GT(pixel.r, 0.5);
        EXPECT_LT(pixel.r, 1);
        pixel = framebuffer.getPixel(29, 29);
        EXPECT_EQ(pixel, Color(1, 1, 1, 1));

        // bottom right
        pixel = framebuffer.getPixel(61, 81);
        EXPECT_EQ(pixel, Color(1, 1, 1, 1));
        pixel = framebuffer.getPixel(63, 83);
        EXPECT_GT(pixel.r, 0);
        EXPECT_LT(pixel.r, 1);
        pixel = framebuffer.getPixel(64, 84);
        EXPECT_EQ(pixel, Color(0, 0, 0, 1));
    });
}

#endif // ONAIR_OKUI_HAS_NATIVE_APPLICATION
