#pragma once

#include "bittorrent/ui/config.h"

#include "bittorrent/ui/BitmapFont.h"
#include "bittorrent/ui/View.h"

namespace bittorrent {
namespace ui {
namespace views {

class TextView : public View {
public:
	enum HorizontalAlignment {
		kHorizontalAlignmentLeft,
		kHorizontalAlignmentCenter,
		kHorizontalAlignmentRight,
	};

	enum VerticalAlignment {
		kVerticalAlignmentBottom,
		kVerticalAlignmentCenter,
		kVerticalAlignmentTop,
	};

	void setAlignment(HorizontalAlignment horizontal, VerticalAlignment vertical = kVerticalAlignmentTop);
	void setFont(shared_ptr<BitmapFont> font, double size) { _font = font; _fontSize = size; }
	void setText(const char* text);
	void setTextColor(double r, double g, double b, double a = 1.0);

	virtual void render() override;

private:
	HorizontalAlignment _horizontalAlignment = kHorizontalAlignmentLeft;
	VerticalAlignment _verticalAlignment = kVerticalAlignmentTop;
	shared_ptr<BitmapFont> _font;
	double _fontSize;
	std::basic_string<BitmapFont::GlyphId> _text;
	double _textColorR{0.0}, _textColorG{0.0}, _textColorB{0.0}, _textColorA{1.0};
	
	void _renderBitmapText(shaders::DistanceFieldShader* shader);
};

}}}