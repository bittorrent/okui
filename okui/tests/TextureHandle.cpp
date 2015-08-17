#include "gtest/gtest.h"

#include "onair/okui/PNGTexture.h"
#include "onair/okui/TextureHandle.h"

using namespace onair;
using namespace onair::okui;

TEST(TextureHandle, callback) {
    TextureHandle handle;
        
    bool one = false;
    handle.onLoad([&] { one = true; });
        
    handle.invokeLoadCallbacks();    
    EXPECT_TRUE(one);
}

TEST(TextureHandle, multipleHandles) {
    auto texture = std::make_shared<PNGTexture>();
    TextureHandle handle;

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
    std::unique_ptr<TextureHandle> handle{new TextureHandle()};

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
