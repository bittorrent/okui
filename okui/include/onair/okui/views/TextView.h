#pragma once

#include "onair/okui/config.h"

#include "onair/okui/BitmapFont.h"
#include "onair/okui/View.h"

namespace onair {
namespace okui {
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
    void setText(const std::string& text) { setText(text.c_str()); }
    void setTextColor(double r, double g, double b, double a = 1.0);

    /**
    * @param texture the name of the texture resource to use
    * @param metadata the name of the metadata resource to use
    */
    void setBitmapFont(const char* texture, const char* metadata, double size);

    void setMultiLine(bool enabled) { _multiLine = enabled;  _computeLines(); }
    bool multiLine() const          { return _multiLine; }

    double textWidth() const { return _textWidth; }

    enum class Weight {
        kRegular,
        kHeavy,
    };

    void setWeight(Weight weight) { _weight = weight; }

    Point<int> lineColumnPosition(size_t line, size_t column) const;
    std::pair<size_t, size_t> lineColumnAtPosition(int x, int y) const;

    double lineHeight() const;

    virtual void render() override;
    virtual void layout() override;
    virtual void windowChanged() override;

private:
    std::shared_ptr<BitmapFont> _font;
    double _fontSize;
    std::string _bitmapFontTexture;
    std::string _bitmapFontMetadata;
    double _bitmapFontSize = 0.0;

    HorizontalAlignment _horizontalAlignment = HorizontalAlignment::kLeft;
    VerticalAlignment _verticalAlignment = VerticalAlignment::kCenter;
    Weight _weight = Weight::kRegular;
    std::basic_string<BitmapFont::GlyphId> _text;
    std::vector<std::basic_string<BitmapFont::GlyphId>> _lines;
    double _textWidth;
    double _textColorR{0.0}, _textColorG{0.0}, _textColorB{0.0}, _textColorA{1.0};
    bool _multiLine = true;

    void _computeLines();
    void _renderBitmapText(shaders::DistanceFieldShader* shader);
    double _calcXOffset(const std::basic_string<BitmapFont::GlyphId>& line) const;
    double _calcYOffset() const;
};

}}}
