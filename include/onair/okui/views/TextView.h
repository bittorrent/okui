#pragma once

#include "onair/okui/config.h"

#include "onair/okui/Color.h"
#include "onair/okui/BitmapFont.h"
#include "onair/okui/View.h"

namespace onair {
namespace okui {
namespace views {

class TextView : public View {
public:
    class Style {
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

        enum class OverflowBehavior {
            kNone,
            kWrap,
            kEllipses,
            kShrink,
        };

        Style& font(std::string texture, std::string metadata) { _texture = std::move(texture); _metadata = std::move(metadata); return *this; }
        Style& textSize(double size)                           { _textSize = size; return *this; }
        Style& textColor(const Color& color)                   { _textColor = color; return *this; }
        Style& letterSpacing(double letterSpacing)             { _letterSpacing = letterSpacing; return *this; }
        Style& letterSpacingFromTracking(double tracking)      { _letterSpacing = _textSize * tracking / 1000; return *this; }
        Style& alignment(HorizontalAlignment alignment)        { _horizontalAlignment = alignment; return *this; }
        Style& alignment(VerticalAlignment alignment)          { _verticalAlignment = alignment; return *this; }
        Style& alignment(HorizontalAlignment horizontal, VerticalAlignment vertical)
                                                               { _horizontalAlignment = horizontal; _verticalAlignment = vertical; return *this; }
        Style& overflowBehavior(OverflowBehavior overflow)     { _overflowBehavior = overflow; return *this; }
        Style& weight(double weight)                           { _weight = weight; return *this; }

        const std::string& fontTexture() const                 { return _texture; }
        const std::string& fontMetadata() const                { return _metadata; }
        double textSize() const                                { return _textSize; }
        const Color& textColor() const                         { return _textColor; }
        double letterSpacing() const                           { return _letterSpacing; }
        HorizontalAlignment horizontalAlignment() const        { return _horizontalAlignment; }
        VerticalAlignment verticalAlignment() const            { return _verticalAlignment; }
        OverflowBehavior overflowBehavior() const              { return _overflowBehavior; }
        double weight() const                                  { return _weight; }

    private:
        std::string         _texture;
        std::string         _metadata;
        double              _textSize = 12;
        Color               _textColor = 0x000000_rgb;
        double              _letterSpacing = 0.0;
        HorizontalAlignment _horizontalAlignment = HorizontalAlignment::kLeft;
        VerticalAlignment   _verticalAlignment = VerticalAlignment::kCenter;
        OverflowBehavior    _overflowBehavior = OverflowBehavior::kEllipses;
        double              _weight = 100.0;
    };

    const Style& style() const { return _style; }
    double lineHeight() const;
    size_t numLines() const { return _lines.size(); }
    double textWidth() const { return _textWidth; }
    double textHeight() const { return lineHeight() * std::max<double>(numLines(), 1); }
    Point<int> lineColumnPosition(size_t line, size_t column) const;
    std::pair<size_t, size_t> lineColumnAtPosition(int x, int y) const;

    void setText(const char* text);
    void setText(const std::string& text) { setText(text.c_str()); }
    void setStyle(Style style);
    void setFont(std::string texture, std::string metadata);
    void setTextSize(double size);
    void setTextColor(const Color& color);
    void setLetterSpacing(double letterSpacing);
    void setLetterSpacingFromTracking(double tracking);
    void setAlignment(Style::HorizontalAlignment alignment);
    void setAlignment(Style::VerticalAlignment alignment);
    void setAlignment(Style::HorizontalAlignment horizontal, Style::VerticalAlignment vertical);
    void setOverflowBehavior(Style::OverflowBehavior overflowBehavior);
    void setWeight(double weight);

    virtual void render(const RenderTarget* renderTarget, const Rectangle<int>& area) override;
    virtual void layout() override;
    virtual void windowChanged() override;

private:
    void _computeLines();
    void _renderBitmapText(shaders::DistanceFieldShader* shader);
    double _calcXOffset(const std::basic_string<BitmapFont::GlyphId>& line) const;
    double _calcYOffset() const;
    double _fontScale() const;
    double _lineWidth(const std::basic_string<BitmapFont::GlyphId>& line, double scale) const;

    Style                                               _style;
    std::shared_ptr<BitmapFont>                         _font;
    std::basic_string<BitmapFont::GlyphId>              _glyphs;
    std::vector<std::basic_string<BitmapFont::GlyphId>> _lines;
    double                                              _textWidth = 0;
};

}}}
