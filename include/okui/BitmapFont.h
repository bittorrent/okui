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

#include <okui/TextureHandle.h>

#include <string>
#include <unordered_map>

namespace okui {

class BitmapFont {
public:
    struct Glyph {
        double textureX, textureY;
        double textureWidth, textureHeight;
        double width, height;
        double xOffset, yOffset;
        double xAdvance;
    };

    using GlyphId = uint16_t;

    BitmapFont(TextureHandle texture, const std::string& metadata);

    TextureHandle& texture()   { return _texture; }

    double size() const        { return _size; }
    double padding() const     { return _padding; }
    double lineSpacing() const { return _lineSpacing; }
    double capHeight() const   { return _capHeight; }
    double base() const        { return _base; }
    const Glyph* glyph(GlyphId id) const;
    double kerning(GlyphId first, GlyphId second) const;
    double width(const GlyphId* glyphs, size_t count) const;

private:
    void _parseMetadata(const char* metadata);
    void _parseMetadataLine(const char* line);

    TextureHandle _texture;

    double _size = 0.0;
    double _padding = 0.0;
    double _lineSpacing = 0.0;
    double _capHeight = 0.0;
    double _base = 0.0;
    double _scaleW = 1.0;
    double _scaleH = 1.0;

    std::unordered_map<GlyphId, Glyph> _glyphs;
    std::unordered_map<GlyphId, std::unordered_map<GlyphId, double>> _kernings;
};

} // namespace okui
