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
#include "RenderOnce.h"

#include <okui/BitmapFont.h>

#include <gtest/gtest.h>

#include <iostream>

#if ONAIR_OKUI_HAS_NATIVE_APPLICATION

using namespace okui;

void CheckGlyph(const BitmapFont::Glyph& glyph, double x, double y, double width, double height, double xoffset, double yoffset, double xadvance) {
    EXPECT_EQ(glyph.textureX, x);
    EXPECT_EQ(glyph.textureY, y);
    EXPECT_EQ(glyph.width, width);
    EXPECT_EQ(glyph.height, height);
    EXPECT_EQ(glyph.xOffset, xoffset);
    EXPECT_EQ(glyph.yOffset, yoffset);
    EXPECT_EQ(glyph.xAdvance, xadvance);
}

TEST(BitmapFont, parsing) {
    std::shared_ptr<BitmapFont> bmpFont;

    RenderOnce([&](View* view) {
        bmpFont = view->window()->loadBitmapFontResource("Montserrat-regular.png", "Montserrat-regular.fnt");
        view->window()->ensureTextures();
    }, [&](View* view) {
        ASSERT_NE(bmpFont, nullptr);
        CheckGlyph(*bmpFont->glyph(199), 173,   0, 49, 63, -6,  4, 35);
        CheckGlyph(*bmpFont->glyph(193),   0,  66, 53, 61, -8, -5, 36);
        CheckGlyph(*bmpFont->glyph(202), 431,  66, 42, 60, -4, -4, 31);
        CheckGlyph(*bmpFont->glyph(102), 113, 182, 36, 54, -7,  2, 19);
        CheckGlyph(*bmpFont->glyph(95 ), 833, 391, 46, 21, -5, 42, 35);
    });
}

#endif // ONAIR_OKUI_HAS_NATIVE_APPLICATION
