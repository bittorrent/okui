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

    constexpr uint32_t kUTF8Accept = 0;
    // const uint32_t kUTF8Reject = 12;

    constexpr uint8_t utf8d[] = {
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

    constexpr uint32_t UTF8Decode(uint32_t* state, uint32_t* codep, uint32_t byte) {
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

double TextView::lineHeight() const {
    return _font ? _font->lineSpacing() * _fontScale() : 0;
}

Point<int> TextView::lineColumnPosition(size_t lineNum, size_t col) const {
    if (!_font) { return{0, 0}; }

    auto fontScale = _fontScale();
    auto lineSpacing = _font->lineSpacing() * fontScale;

    auto& line = _lines[lineNum];
    col = std::min(col, line.size());

    return {static_cast<int>(round(_calcXOffset(line) + _lineWidth(line, fontScale))),
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
            x += _font->kerning(line[i - 1], line[i]) * fontScale + _style.letterSpacing();
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

void TextView::setStyle(Style style) {
    _style = std::move(style);
    _computeLines();
}

void TextView::setFont(std::string texture, std::string metadata) {
    _style.font(std::move(texture), std::move(metadata));
    _font = nullptr;
    _textWidth = 0;

    if (window()) {
        _font = window()->loadBitmapFontResource(_style.fontTexture().c_str(), _style.fontMetadata().c_str());
        _computeLines();
    }
}

void TextView::setTextSize(double size) {
    _style.textSize(size);
    _computeLines();
}

void TextView::setLetterSpacing(double letterSpacing) {
    _style.letterSpacing(letterSpacing);
    _computeLines();
}

void TextView::setLetterSpacingFromTracking(double tracking) {
    _style.letterSpacingFromTracking(tracking);
    _computeLines();
}

void TextView::setAlignment(Style::HorizontalAlignment alignment) {
    _style.alignment(alignment);
    invalidateRenderCache();
}

void TextView::setAlignment(Style::VerticalAlignment alignment) {
    _style.alignment(alignment);
    invalidateRenderCache();
}

void TextView::setAlignment(Style::HorizontalAlignment horizontal, Style::VerticalAlignment vertical) {
    _style.alignment(horizontal, vertical);
    invalidateRenderCache();
}

void TextView::setOverflowBehavior(Style::OverflowBehavior overflowBehavior) {
    _style.overflowBehavior(overflowBehavior);
    _computeLines();
}

void TextView::setWeight(double weight) {
    _style.weight(weight);
    invalidateRenderCache();
}

void TextView::setTextColor(const Color& color) {
    _style.textColor(color);
    invalidateRenderCache();
}

void TextView::render(const RenderTarget* renderTarget, const Rectangle<int>& area) {
    if (!_font) { return; }

    auto distanceFieldShader = this->distanceFieldShader();

    auto glyphId = BitmapFont::GlyphId{0x4f}; // 'O'

    double fontScale   = _fontScale(),
           glyphWidth  = _font->width(&glyphId, 1) * fontScale,
           glyphHeight = _font->capHeight() * fontScale,
           clipWidth   = 0,
           clipHeight  = 0;
    renderTransformation().transform(glyphWidth, glyphHeight, &clipWidth, &clipHeight);

    distanceFieldShader->enableSupersampling(true);
    distanceFieldShader->setEdge(std::min(std::max(1.0 - _style.weight() / 200.0, 0.0), 1.0));

    distanceFieldShader->setColor(_style.textColor());
    _renderBitmapText(distanceFieldShader);

    distanceFieldShader->flush();
}

void TextView::layout() {
    _computeLines();
}

void TextView::windowChanged() {
    if (window() && !_style.fontTexture().empty() && !_style.fontMetadata().empty()) {
        _font = window()->loadBitmapFontResource(_style.fontTexture().c_str(), _style.fontMetadata().c_str());
        _computeLines();
    }
}

void TextView::_computeLines() {
    _textWidth = 0.0;
    _lines.clear();

    if (!_font) { return; }

    auto fontScale = _style.textSize() / _font->size();

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

        if (_lineWidth(line, fontScale) > width) {
            if (_style.overflowBehavior() == Style::OverflowBehavior::kEllipses) {
                // rewind, insert the ellipses, and skip until a new line
                do {
                    line.pop_back();
                } while (!line.empty() && _font->width(line.data(), line.size()) * fontScale + ellipsesWidth > width);
                line.insert(line.end(), ellipses.begin(), ellipses.end());
                skipUntilNewLine = true;
            } else if (_style.overflowBehavior() == Style::OverflowBehavior::kWrap) {
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
        auto letterSpacing = line.empty() ? 0 : ((line.size() - 1) * _style.letterSpacing());
        _textWidth = std::max(_textWidth, _font->width(line.data(), line.size()) * fontScale + letterSpacing);
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
                x += _font->kerning(line[i - 1], line[i]) * fontScale + _style.letterSpacing();
            }
            auto glyph = _font->glyph(line[i]);
            if (glyph) {
                Rectangle<double> glyphBounds(x + glyph->xOffset * fontScale, y + glyph->yOffset * fontScale, glyph->width * fontScale, glyph->height * fontScale);
                auto textureScale = glyphBounds.width / glyph->textureWidth;
                shader->setTexture(*texture, glyphBounds.x - glyph->textureX * textureScale, glyphBounds.y - glyph->textureY * textureScale, texture->width() * textureScale, texture->height() * textureScale);
                shapes::Rectangle(glyphBounds.x, glyphBounds.y, glyphBounds.width, glyphBounds.height).draw(shader);
                x += glyph->xAdvance * fontScale;
            }
        }

        y += lineSpacing;
    }
}

double TextView::_calcXOffset(const std::basic_string<BitmapFont::GlyphId>& line) const {
    if (!_font) { return 0; }

    auto textWidth = _lineWidth(line, _fontScale());

    double x = 0.0;
    if (_style.horizontalAlignment() == Style::HorizontalAlignment::kCenter) {
        x = (bounds().width - textWidth) * 0.5;
    } else if (_style.horizontalAlignment() == Style::HorizontalAlignment::kRight) {
        x = bounds().width - textWidth;
    }

    return x;
}

double TextView::_calcYOffset() const {
    if (!_font) { return 0; }

    auto lineSpacing = _font->lineSpacing() * _fontScale();
    auto textHeight  = lineSpacing * std::min<size_t>(_lines.size(), 1);

    auto y = 0.0;
    if (_style.verticalAlignment() == Style::VerticalAlignment::kCenter) {
        y = (bounds().height - textHeight) * 0.5;
    } else if (_style.verticalAlignment() == Style::VerticalAlignment::kBottom) {
        y = bounds().height - textHeight;
    }

    return y;
}

double TextView::_fontScale() const {
    if (_style.overflowBehavior() == Style::OverflowBehavior::kShrink && _textWidth > bounds().width) {
        return _style.textSize() / _font->size() * bounds().width / _textWidth;
    }
    return _style.textSize() / _font->size();
}

double TextView::_lineWidth(const std::basic_string<BitmapFont::GlyphId>& line, double fontScale) const {
    auto letterSpacing = line.empty() ? 0 : (line.size()-1) * _style.letterSpacing();
    return _font->width(line.data(), line.size()) * fontScale + letterSpacing;
}

}}}
