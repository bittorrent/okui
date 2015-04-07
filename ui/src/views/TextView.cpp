#ifdef BT_USE_UI_VIEWS

#include "bittorrent/ui/views/TextView.h"

#include "bittorrent/ui/shapes/Rectangle.h"

namespace bittorrent {
namespace ui {
namespace views {

void TextView::setAlignment(HorizontalAlignment horizontal, VerticalAlignment vertical) {
    _horizontalAlignment = horizontal;
    _verticalAlignment = vertical;
}

void TextView::setText(const char* text) {
	_text.clear();
	while (*text) {
		_text += *text;
		++text;
	}
}

void TextView::setTextColor(double r, double g, double b, double a) {
    _textColorR = r;
    _textColorG = g;
    _textColorB = b;
    _textColorA = a;
}

void TextView::render() {
	auto distanceFieldShader = this->distanceFieldShader();

	distanceFieldShader->setColor(_textColorR, _textColorG, _textColorB, _textColorA);
	_renderBitmapText(distanceFieldShader);

	distanceFieldShader->flush();
}

void TextView::_renderBitmapText(shaders::DistanceFieldShader* shader) {
	// TODO: this could be much more optimized (e.g. via display list or vbo)
	
	auto fontScale = _fontSize / _font->size();
	auto textWidth = _font->width(_text.data(), _text.size()) * fontScale;
	auto texture = _font->texture();

	double x = 0.0;
	if (_horizontalAlignment == kHorizontalAlignmentCenter) {
		x = (bounds().width - textWidth) * 0.5;
	} else if (_horizontalAlignment == kHorizontalAlignmentRight) {
		x = bounds().width - textWidth;
	}
	
	double y = _font->base() * fontScale;
	if (_verticalAlignment == kVerticalAlignmentCenter) {
		y += (bounds().height - _font->lineHeight() * fontScale) * 0.5;
	} else if (_verticalAlignment == kVerticalAlignmentTop) {
		y += bounds().height - _font->lineHeight() * fontScale;
	}

	for (size_t i = 0; i < _text.size(); ++i) {
		if (i > 0) {
			x += _font->kerning(_text[i - 1], _text[i]);
		}
		auto glyph = _font->glyph(_text[i]);
		ui::Rectangle<double> glyphBounds(x + glyph->xOffset * fontScale, y + glyph->yOffset * fontScale, glyph->width * fontScale, glyph->height * fontScale);
		auto textureScale = glyphBounds.width / glyph->textureWidth;
		shader->setTexture(texture->id(), glyphBounds.x - glyph->textureX * textureScale, glyphBounds.y - glyph->textureY * textureScale, texture->width() * textureScale, texture->height() * textureScale);
		ui::shapes::Rectangle(glyphBounds.x, glyphBounds.y, glyphBounds.width, glyphBounds.height).draw(shader);
		x += glyph->xAdvance * fontScale;
	}	
}

}}}

#endif