#include "bittorrent/ui/views/TextView.h"

#include "bittorrent/ui/shapes/Rectangle.h"

namespace bittorrent {
namespace ui {
namespace views {

void TextView::setAlignment(HorizontalAlignment horizontal, VerticalAlignment vertical) {
    _horizontalAlignment = horizontal;
    _verticalAlignment = vertical;
}

void TextView::setFont(std::shared_ptr<BitmapFont> font, double size) {
    _font = font;
    _fontSize = size;
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

void TextView::_computeLines() {
    _textWidth = 0.0;
    _lines.clear();

    if (!_font) { return; }

    auto fontScale = _fontSize / _font->size();
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

    for (auto& line : _lines) {
        _textWidth = std::max(_textWidth, _font->width(line.data(), line.size()) * fontScale);
    }
}

void TextView::_renderBitmapText(shaders::DistanceFieldShader* shader) {
    // TODO: this could be much more optimized (e.g. via display list or vbo)

    auto fontScale = _fontSize / _font->size();
    auto texture = _font->texture();

    auto lineSpacing = _font->lineSpacing() * fontScale;
    auto textHeight = _lines.empty() ? 0.0 : (lineSpacing * _lines.size() - (lineSpacing - _font->capHeight() * fontScale));

    auto y = 0.0;
    if (_verticalAlignment == VerticalAlignment::kCenter) {
        y = (bounds().height - textHeight) * 0.5;
    } else if (_verticalAlignment == VerticalAlignment::kTop) {
        y = bounds().height - textHeight;
    }

    y += (lineSpacing * (_lines.size() - 1)) - _font->base() * fontScale;

    for (auto& line : _lines) {
        auto textWidth = _font->width(line.data(), line.size()) * fontScale;

        double x = 0.0;
        if (_horizontalAlignment == HorizontalAlignment::kCenter) {
            x = (bounds().width - textWidth) * 0.5;
        } else if (_horizontalAlignment == HorizontalAlignment::kRight) {
            x = bounds().width - textWidth;
        }

        for (size_t i = 0; i < line.size(); ++i) {
            if (i > 0) {
                x += _font->kerning(line[i - 1], line[i]);
            }
            auto glyph = _font->glyph(line[i]);
            if (glyph) {
                ui::Rectangle<double> glyphBounds(x + glyph->xOffset * fontScale, y + glyph->yOffset * fontScale, glyph->width * fontScale, glyph->height * fontScale);
                auto textureScale = glyphBounds.width / glyph->textureWidth;
                shader->setTexture(texture->id(), glyphBounds.x - glyph->textureX * textureScale, glyphBounds.y - glyph->textureY * textureScale, texture->width() * textureScale, texture->height() * textureScale);
                ui::shapes::Rectangle(glyphBounds.x, glyphBounds.y, glyphBounds.width, glyphBounds.height).draw(shader);
                x += glyph->xAdvance * fontScale;
            }
        }

        y -= lineSpacing;
    }
}

}}}
