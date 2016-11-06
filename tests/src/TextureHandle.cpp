#include <okui/FileTexture.h>
#include <okui/TextureHandle.h>

#include <gtest/gtest.h>

using namespace okui;

TEST(TextureHandle, callback) {
    TextureHandle handle;

    bool one = false;
    handle.onLoad([&] { one = true; });

    handle.invokeLoadCallbacks();
    EXPECT_TRUE(one);
}

TEST(TextureHandle, multipleHandles) {
    auto texture = std::make_shared<FileTexture>();
    TextureHandle handle{texture};

    bool one = false;
    handle.onLoad([&] { one = true; });

    auto second = handle.newHandle();
    bool two = false;
    second.onLoad([&] { two = true; });

    handle.invokeLoadCallbacks();
    EXPECT_TRUE(one);
    EXPECT_TRUE(two);

    EXPECT_EQ(handle.texture(), second.texture());
}

TEST(TextureHandle, expiredHandle) {
    auto handle = std::make_unique<TextureHandle>();

    bool one = false;
    handle->onLoad([&] { one = true; });

    auto second = handle->newHandle();
    bool two = false;
    second.onLoad([&] { two = true; });

    handle.reset();

    second.invokeLoadCallbacks();
    EXPECT_FALSE(one);
    EXPECT_TRUE(two);
}
