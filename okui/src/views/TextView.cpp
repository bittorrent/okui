#include "onair/okui/views/TextView.h"
#include "onair/okui/shapes/Rectangle.h"
#include "onair/okui/Window.h"

#include <cmath>

namespace onair {
namespace okui {
namespace views {

void TextView::setAlignment(HorizontalAlignment horizontal, VerticalAlignment vertical) {
    _horizontalAlignment = horizontal;
    _verticalAlignment = vertical;
}

void TextView::setFont(std::shared_ptr<BitmapFont> font, double size) {
    _font = font;
    _fontSize = size;
    _bitmapFontTexture.clear();
    _bitmapFontMetadata.clear();
    _computeLines();
}

void TextView::setText(const char* text) {
    _text.clear();
    while (*text) {
        _text += *text;
        ++text;
    }
    _computeLines();
}

void TextView::setTextColor(double r, double g, double b, double a) {
    _textColorR = r;
    _textColorG = g;
    _textColorB = b;
    _textColorA = a;
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

void TextView::render() {
    if (!_font) { return; }

    auto distanceFieldShader = this->distanceFieldShader();

    if (_weight == Weight::kHeavy) {
        distanceFieldShader->setEdge(0.45);
    }

    distanceFieldShader->setColor(_textColorR, _textColorG, _textColorB, _textColorA);
    _renderBitmapText(distanceFieldShader);

    distanceFieldShader->flush();
    distanceFieldShader->setEdge(0.5);
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

    auto fontScale = _fontSize / _font->size();
    auto lineSpacing = _font->lineSpacing() * fontScale;

    auto& line = _lines[lineNum];
    col = std::min(col, line.size());

    return {static_cast<int>(round(_calcXOffset(line) + (_font->width(line.data(), col) * fontScale))),
            static_cast<int>(round(_calcYOffset() + (lineNum * lineSpacing)))};
}

std::pair<size_t, size_t> TextView::lineColumnAtPosition(int mouseX, int mouseY) const {
    if (!_font || _lines.empty()) { return{0, 0}; }

    auto fontScale   = _fontSize / _font->size();
    auto lineSpacing = _font->lineSpacing() * fontScale;

    auto lineNum = std::min<size_t>(_lines.size()-1, std::max(0, static_cast<int>((mouseY-_calcYOffset()) / lineSpacing)));
    auto& line = _lines[lineNum];

    double x = _calcXOffset(line);

    for (size_t i = 0; i < line.size(); ++i) {
        if (i > 0) {
            x += _font->kerning(line[i - 1], line[i]);
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

    auto fontScale = _fontSize / _font->size();
    return _font->lineSpacing() * fontScale;
}

void TextView::_computeLines() {
    _textWidth = 0.0;
    _lines.clear();

    if (!_font) { return; }

    auto fontScale = _fontSize / _font->size();

    if (!_multiLine) {
        _lines.push_back(_text);
    } else {
        auto width = bounds().width;
        std::basic_string<BitmapFont::GlyphId> line;

        for (auto& glyph : _text) {
            if (glyph == '\n') {
                _lines.emplace_back(std::move(line));
                line.clear();
                continue;
            }
            line.push_back(glyph);
            auto lineWidth = _font->width(line.data(), line.size()) * fontScale;
            if (lineWidth > width) {
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

        if (!line.empty()) {
            _lines.emplace_back(std::move(line));
        }
    }

    for (auto& line : _lines) {
        _textWidth = std::max(_textWidth, _font->width(line.data(), line.size()) * fontScale);
    }
}

void TextView::_renderBitmapText(shaders::DistanceFieldShader* shader) {
    // TODO: this could be much more optimized (e.g. via display list or vbo)
    auto fontScale = _fontSize / _font->size();
    auto texture = _font->texture();
    auto lineSpacing = _font->lineSpacing() * fontScale;
    auto y = _calcYOffset();

    for (auto& line : _lines) {
        double x = _calcXOffset(line);

        for (size_t i = 0; i < line.size(); ++i) {
            if (i > 0) {
                x += _font->kerning(line[i - 1], line[i]);
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

    auto fontScale = _fontSize / _font->size();
    auto textWidth = _font->width(line.data(), line.size()) * fontScale;

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

    auto fontScale   = _fontSize / _font->size();
    auto lineSpacing = _font->lineSpacing() * fontScale;
    auto textHeight  = lineSpacing * std::min<size_t>(_lines.size(), 1);

    auto y = 0.0;
    if (_verticalAlignment == VerticalAlignment::kCenter) {
        y = (bounds().height - textHeight) * 0.5;
    } else if (_verticalAlignment == VerticalAlignment::kBottom) {
        y = bounds().height - textHeight;
    }

    return y;
}

}}}
