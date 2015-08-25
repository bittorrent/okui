#include "onair/okui/BitmapFont.h"

#include <cstdlib>
#include <cstring>

namespace onair {
namespace okui {

BitmapFont::BitmapFont(TextureHandle&& texture, const std::string& metadata) : _texture(std::move(texture)) {
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
        _size = sLineParameter("size", line);
        _padding = sLineParameter("padding", line);
    } else if (!strncmp(line, "common ", 7)) {
        _lineSpacing = sLineParameter("lineHeight", line);
        _base        = _lineSpacing - sLineParameter("base", line);
        _scaleW      = sLineParameter("scaleW", line);
        _scaleH      = sLineParameter("scaleH", line);
    } else if (!strncmp(line, "char ", 5)) {
        auto id = sLineParameter("id", line);
        auto& glyph    = _glyphs[id];

        auto textureScale = _texture->width() / _scaleW;

        glyph.width         = sLineParameter("width", line);
        glyph.height        = sLineParameter("height", line);
        glyph.textureWidth  = glyph.width * textureScale;
        glyph.textureHeight = glyph.height * textureScale;
        glyph.textureX      = sLineParameter("x", line) * textureScale;
        glyph.textureY      = sLineParameter("y", line) * textureScale;
        glyph.xOffset       = sLineParameter("xoffset", line);
        glyph.yOffset       = sLineParameter("yoffset", line);
        glyph.xAdvance      = sLineParameter("xadvance", line);

        if (strchr("ABCDEFGHIKLMNOPRSTUVWXYZ", id)) {
            _capHeight = std::max(glyph.height - _padding * 2, _capHeight);
        }
    } else if (!strncmp(line, "kerning ", 8)) {
        _kernings[sLineParameter("first", line)][sLineParameter("second", line)] = sLineParameter("amount", line);
    }
}

double BitmapFont::sLineParameter(const char* name, const char* line) {
    auto parameter = strstr(line, name);
	if (!parameter) { return 0.0; }
    auto nameLength = strlen(name);
    if (parameter[nameLength] != '=') { return 0.0; }
    return std::strtod(parameter + nameLength + 1, nullptr);
}

}}
