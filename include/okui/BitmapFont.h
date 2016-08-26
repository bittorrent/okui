#pragma once

#include "okui/config.h"

#include "okui/TextureHandle.h"

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
