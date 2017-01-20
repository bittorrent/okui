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
#include <okui/BitmapFont.h>

#include <cstdlib>
#include <cstring>

namespace okui {

namespace {

double LineParameter(const char* name, const char* line) {
    auto parameter = strstr(line, name);
    if (!parameter) { return 0.0; }
    auto nameLength = strlen(name);
    if (parameter[nameLength] != '=') { return 0.0; }
    return std::strtod(parameter + nameLength + 1, nullptr);
}

}

BitmapFont::BitmapFont(TextureHandle texture, const std::string& metadata)
    : _texture{std::move(texture)}
{
    _parseMetadata(metadata.c_str());
}

const BitmapFont::Glyph* BitmapFont::glyph(GlyphId id) const {
    auto it = _glyphs.find(id);
    return it == _glyphs.end() ? nullptr : &it->second;
}

double BitmapFont::kerning(GlyphId first, GlyphId second) const {
    auto it = _kernings.find(first);
    if (it == _kernings.end()) { return 0.0; }

    auto it2 = it->second.find(second);
    return it2 == it->second.end() ? 0.0 : it2->second;
}

double BitmapFont::width(const GlyphId* glyphs, size_t count) const {
    double width = 0.0;
    for (size_t i = 0; i < count; ++i) {
        auto g = glyph(glyphs[i]);
        if (!g) { continue; }
        if (i > 0) {
            width += kerning(glyphs[i - 1], glyphs[i]);
        }
        width += g->xAdvance;
    }
    return width;
}

void BitmapFont::_parseMetadata(const char* metadata) {
    auto line = metadata;
    while (true) {
        auto newline = strchr(line, '\n');
        _parseMetadataLine(line);
        if (newline) {
            line = newline + 1;
        } else {
            break;
        }
    }
}

void BitmapFont::_parseMetadataLine(const char* line) {
    if (!strncmp(line, "info ", 5)) {
        _size = LineParameter("size", line);
        _padding = LineParameter("padding", line);
    } else if (!strncmp(line, "common ", 7)) {
        _lineSpacing = LineParameter("lineHeight", line);
        _base        = _lineSpacing - LineParameter("base", line);
        _scaleW      = LineParameter("scaleW", line);
        _scaleH      = LineParameter("scaleH", line);
    } else if (!strncmp(line, "char ", 5)) {
        auto id = LineParameter("id", line);
        auto& glyph = _glyphs[id];

        auto textureScale = _texture->width() / _scaleW;

        glyph.width         = LineParameter("width", line);
        glyph.height        = LineParameter("height", line);
        glyph.textureWidth  = glyph.width * textureScale;
        glyph.textureHeight = glyph.height * textureScale;
        glyph.textureX      = LineParameter("x", line) * textureScale;
        glyph.textureY      = LineParameter("y", line) * textureScale;
        glyph.xOffset       = LineParameter("xoffset", line);
        glyph.yOffset       = LineParameter("yoffset", line);
        glyph.xAdvance      = LineParameter("xadvance", line);

        if (strchr("ABCDEFGHIKLMNOPRSTUVWXYZ", id)) {
            _capHeight = std::max(glyph.height - _padding * 2, _capHeight);
        }
    } else if (!strncmp(line, "kerning ", 8)) {
        _kernings[LineParameter("first", line)][LineParameter("second", line)] = LineParameter("amount", line);
    }
}

} // namespace okui
