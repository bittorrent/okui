#pragma once

#include "bittorrent/ui/Texture.h"

#include <string>

namespace bittorrent {
namespace ui {

class BitmapFont {
public:
    BitmapFont(shared_ptr<Texture> texture, const std::string& metadata);

    shared_ptr<Texture> texture() const { return _texture; }

    double size() const { return _size; }
    double padding() const { return _padding; }
    double lineHeight() const { return _lineHeight; }
    double base() const { return _base; }

    using GlyphId = uint16_t;

    struct Glyph {
        double textureX, textureY;
        double textureWidth, textureHeight;
        double width, height;
        double xOffset, yOffset;
        double xAdvance;
    };

    const Glyph* glyph(GlyphId id) const;
    double kerning(GlyphId first, GlyphId second) const;
    
    double width(const GlyphId* glyphs, size_t count) const;

private:
    shared_ptr<Texture> _texture;

    double _size = 0.0;
    double _padding = 0.0;
    double _lineHeight = 0.0;
    double _base = 0.0;
    double _scaleW = 1.0;
    double _scaleH = 1.0;

    std::unordered_map<GlyphId, Glyph> _glyphs;
    std::unordered_map<GlyphId, std::unordered_map<GlyphId, double>> _kernings;

    void _parseMetadata(const char* metadata);
    void _parseMetadataLine(const char* line);
    
    static double sLineParameter(const char* name, const char* line);
};

}}