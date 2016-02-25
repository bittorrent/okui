#include "onair/okui/views/TextView.h"
#include "onair/okui/shapes/Rectangle.h"
#include "onair/okui/Window.h"

namespace {
    /*
    Copyright (c) 2008-2010 Bjoern Hoehrmann <bjoern@hoehrmann.de>
    See http://bjoern.hoehrmann.de/utf-8/decoder/dfa/ for details.

    Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
    associated documentation files (the "Software"), to deal in the Software without restriction,
    including without limitation the rights to use, copy, modify, merge, publish, distribute,
    sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all copies or
    substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
    NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
    DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
    */

    const uint32_t kUTF8Accept = 0;
    // const uint32_t kUTF8Reject = 12;

    const uint8_t utf8d[] = {
         0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
         0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
         0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
         0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
         1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,  9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
         7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,  7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
         8,8,2,2,2,2,2,2,2,2,2,2,2,2,2,2,  2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
        10,3,3,3,3,3,3,3,3,3,3,3,3,4,3,3, 11,6,6,6,5,8,8,8,8,8,8,8,8,8,8,8,

         0,12,24,36,60,96,84,12,12,12,48,72, 12,12,12,12,12,12,12,12,12,12,12,12,
        12, 0,12,12,12,12,12, 0,12, 0,12,12, 12,24,12,12,12,12,12,24,12,24,12,12,
        12,12,12,12,12,12,12,24,12,12,12,12, 12,24,12,12,12,12,12,12,12,24,12,12,
        12,12,12,12,12,12,12,36,12,36,12,12, 12,36,12,12,12,12,12,36,12,36,12,12,
        12,36,12,12,12,12,12,12,12,12,12,12,
    };

    uint32_t inline UTF8Decode(uint32_t* state, uint32_t* codep, uint32_t byte) {
        uint32_t type = utf8d[byte];
        *codep = (*state != kUTF8Accept) ? (byte & 0x3fu) | (*codep << 6) : (0xff >> type) & (byte);
        *state = utf8d[256 + *state + type];
        return *state;
    }
}

#include <cmath>

namespace onair {
namespace okui {
namespace views {

void TextView::setAlignment(HorizontalAlignment horizontal, VerticalAlignment vertical) {
    _horizontalAlignment = horizontal;
    _verticalAlignment = vertical;
    invalidateRenderCache();
}

void TextView::setFont(std::shared_ptr<BitmapFont> font, double size) {
    _font = font;
    _fontSize = size;
    _bitmapFontTexture.clear();
    _bitmapFontMetadata.clear();
    _computeLines();
}

void TextView::setText(const char* text) {
    _glyphs.clear();

    uint32_t codePoint;
    uint32_t state = 0;

    for (auto ptr = text; *ptr; ++ptr) {
        if (UTF8Decode(&state, &codePoint, static_cast<unsigned char>(*ptr))) {
            continue;
        }
        _glyphs += *ptr;
    }

    _computeLines();
}

void TextView::setTextColor(const Color& color) {
    _textColor = color;
    invalidateRenderCache();
}

void TextView::setBitmapFont(const char* texture, const char* metadata, double size) {
    _bitmapFontTexture = texture;
    _bitmapFontMetadata = metadata;
    _bitmapFontSize = size;
    _font = nullptr;
    if (window()) {
        setFont(window()->loadBitmapFontResource(_bitmapFontTexture.c_str(), _bitmapFontMetadata.c_str()), _bitmapFontSize);
    }
}

void TextView::render(const RenderTarget* renderTarget, const Rectangle<int>& area) {
    if (!_font) { return; }

    auto distanceFieldShader = this->distanceFieldShader();
    
    auto glyphId = BitmapFont::GlyphId{0x4f}; // 'O'

    double fontScale   = _fontSize / _font->size(),
           glyphWidth  = _font->width(&glyphId, 1) * fontScale,
           glyphHeight = _font->capHeight() * fontScale,
           clipWidth   = 0,
           clipHeight  = 0;
    renderTransformation().transform(glyphWidth, glyphHeight, &clipWidth, &clipHeight);

    distanceFieldShader->enableSupersampling(true);
    distanceFieldShader->setEdge(_weight == Weight::kHeavy ? 0.45 : 0.5);

    distanceFieldShader->setColor(_textColor);
    _renderBitmapText(distanceFieldShader);

    distanceFieldShader->flush();
}

void TextView::layout() {
    _computeLines();
}

void TextView::windowChanged() {
    if (!_font && !_bitmapFontTexture.empty() && window()) {
        setFont(window()->loadBitmapFontResource(_bitmapFontTexture.c_str(), _bitmapFontMetadata.c_str()), _bitmapFontSize);
    }
}

Point<int> TextView::lineColumnPosition(size_t lineNum, size_t col) const {
    if (!_font) { return{0, 0}; }

    auto fontScale = _fontScale();
    auto lineSpacing = _font->lineSpacing() * fontScale;

    auto& line = _lines[lineNum];
    col = std::min(col, line.size());

    return {static_cast<int>(round(_calcXOffset(line) + (_font->width(line.data(), col) * fontScale))),
            static_cast<int>(round(_calcYOffset() + (lineNum * lineSpacing)))};
}

std::pair<size_t, size_t> TextView::lineColumnAtPosition(int mouseX, int mouseY) const {
    if (!_font || _lines.empty()) { return{0, 0}; }

    auto fontScale   = _fontScale();
    auto lineSpacing = _font->lineSpacing() * fontScale;

    auto lineNum = std::min<size_t>(_lines.size()-1, std::max(0, static_cast<int>((mouseY-_calcYOffset()) / lineSpacing)));
    auto& line = _lines[lineNum];

    double x = _calcXOffset(line);

    for (size_t i = 0; i < line.size(); ++i) {
        if (i > 0) {
            x += _font->kerning(line[i - 1], line[i]) + _letterSpacing;
        }
        auto glyph = _font->glyph(line[i]);
        if (glyph) {
            x += glyph->xAdvance * fontScale;
        }

        if (mouseX < x) {
            return {lineNum, i};
        }
    }

    return {lineNum, line.size()};
}

double TextView::lineHeight() const {
    if (!_font) { return 0; }
    return _font->lineSpacing() * _fontScale();
}

void TextView::_computeLines() {
    _textWidth = 0.0;
    _lines.clear();

    if (!_font) { return; }

    auto fontScale = _fontSize / _font->size();

    auto width = bounds().width;
    std::basic_string<BitmapFont::GlyphId> line;

    static constexpr std::array<BitmapFont::GlyphId, 3> ellipses{{ 0x2e, 0x2e, 0x2e }};
    auto ellipsesWidth = _font->width(ellipses.data(), ellipses.size()) * fontScale;
    bool skipUntilNewLine = false;

    for (auto& glyph : _glyphs) {
        if (glyph == '\n') {
            _lines.emplace_back(std::move(line));
            line.clear();
            skipUntilNewLine = false;
            continue;
        } else if (skipUntilNewLine) {
            continue;
        }

        line.push_back(glyph);

        auto lineWidth = _font->width(line.data(), line.size()) * fontScale;
        if (lineWidth > width) {
            if (_overflowBehavior == OverflowBehavior::kEllipses) {
                // rewind, insert the ellipses, and skip until a new line
                do {
                    line.pop_back();
                } while (!line.empty() && _font->width(line.data(), line.size()) * fontScale + ellipsesWidth > width);
                line.insert(line.end(), ellipses.begin(), ellipses.end());
                skipUntilNewLine = true;
            } else if (_overflowBehavior == OverflowBehavior::kWrap) {
                // try to break
                for (size_t i = line.size(); i > 0; --i) {
                    auto& candidate = line[i - 1];
                    auto glyph = _font->glyph(candidate);
                    if (!glyph->width) {
                        // whitespace
                        if (i > 0) {
                            _lines.emplace_back(&line[0], i - 1);
                        }
                        if (i < line.size()) {
                            line = line.substr(i);
                        } else {
                            line.clear();
                        }
                        break;
                    }
                }
            }
        }
    }

    if (!line.empty()) {
        _lines.emplace_back(std::move(line));
    }

    for (auto& line : _lines) {
        auto letterSpacing = line.empty() ? 0 : (line.size()-1) * _letterSpacing;
        _textWidth = std::max(_textWidth, (_font->width(line.data(), line.size()) + letterSpacing) * fontScale);
    }

    invalidateRenderCache();
}

void TextView::_renderBitmapText(shaders::DistanceFieldShader* shader) {
    // TODO: this could be much more optimized (e.g. via display list or vbo)

    auto& texture = _font->texture();
    if (!texture->isLoaded()) { return; }

    auto fontScale = _fontScale();
    auto lineSpacing = _font->lineSpacing() * fontScale;
    auto y = _calcYOffset();

    for (auto& line : _lines) {
        double x = _calcXOffset(line);

        for (size_t i = 0; i < line.size(); ++i) {
            if (i > 0) {
                x += _font->kerning(line[i - 1], line[i]) + _letterSpacing;
            }
            auto glyph = _font->glyph(line[i]);
            if (glyph) {
                Rectangle<double> glyphBounds(x + glyph->xOffset * fontScale, y + glyph->yOffset * fontScale, glyph->width * fontScale, glyph->height * fontScale);
                auto textureScale = glyphBounds.width / glyph->textureWidth;
                shader->setTexture(texture->id(), glyphBounds.x - glyph->textureX * textureScale, glyphBounds.y - glyph->textureY * textureScale, texture->width() * textureScale, texture->height() * textureScale);
                shapes::Rectangle(glyphBounds.x, glyphBounds.y, glyphBounds.width, glyphBounds.height).draw(shader);
                x += glyph->xAdvance * fontScale;
            }
        }

        y += lineSpacing;
    }
}

double TextView::_calcXOffset(const std::basic_string<BitmapFont::GlyphId>& line) const {
    if (!_font) { return 0; }

    auto textWidth = _font->width(line.data(), line.size()) * _fontScale();

    double x = 0.0;
    if (_horizontalAlignment == HorizontalAlignment::kCenter) {
        x = (bounds().width - textWidth) * 0.5;
    } else if (_horizontalAlignment == HorizontalAlignment::kRight) {
        x = bounds().width - textWidth;
    }

    return x;
}

double TextView::_calcYOffset() const {
    if (!_font) { return 0; }

    auto lineSpacing = _font->lineSpacing() * _fontScale();
    auto textHeight  = lineSpacing * std::min<size_t>(_lines.size(), 1);

    auto y = 0.0;
    if (_verticalAlignment == VerticalAlignment::kCenter) {
        y = (bounds().height - textHeight) * 0.5;
    } else if (_verticalAlignment == VerticalAlignment::kBottom) {
        y = bounds().height - textHeight;
    }

    return y;
}

double TextView::_fontScale() const {
    if (_overflowBehavior == OverflowBehavior::kShrink && _textWidth > bounds().width) {
        return _fontSize / _font->size() * bounds().width / _textWidth;
    }
    return _fontSize / _font->size();
}

}}}
