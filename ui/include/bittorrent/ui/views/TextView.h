#pragma once

#include "bittorrent/ui/config.h"

#include "bittorrent/ui/BitmapFont.h"
#include "bittorrent/ui/View.h"

namespace bittorrent {
namespace ui {
namespace views {

class TextView : public View {
public:
	enum class HorizontalAlignment {
		kLeft,
		kCenter,
		kRight,
	};

	enum class VerticalAlignment {
		kBottom,
		kCenter,
		kTop,
	};

	void setAlignment(HorizontalAlignment horizontal, VerticalAlignment vertical = VerticalAlignment::kTop);
	void setFont(std::shared_ptr<BitmapFont> font, double size);
	void setText(const char* text);
	void setTextColor(double r, double g, double b, double a = 1.0);
	
	double textWidth() const { return _textWidth; }

	enum class Weight {
	    kRegular,
	    kHeavy,
	};

	void setWeight(Weight weight) { _weight = weight; }

	virtual void render() override;
	virtual void layout() override;

private:
	HorizontalAlignment _horizontalAlignment = HorizontalAlignment::kLeft;
	VerticalAlignment _verticalAlignment = VerticalAlignment::kTop;
	Weight _weight = Weight::kRegular;
	std::shared_ptr<BitmapFont> _font;
	double _fontSize;
	std::basic_string<BitmapFont::GlyphId> _text;
	std::vector<std::basic_string<BitmapFont::GlyphId>> _lines;
	double _textWidth;
	double _textColorR{0.0}, _textColorG{0.0}, _textColorB{0.0}, _textColorA{1.0};
	
	void _computeLines();
	void _renderBitmapText(shaders::DistanceFieldShader* shader);
};

}}}
