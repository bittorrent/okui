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
