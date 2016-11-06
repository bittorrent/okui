#pragma once

#include <okui/config.h>

#include <okui/Color.h>
#include <okui/BitmapFont.h>
#include <okui/View.h>

#include <stdts/string_view.h>

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
        Style& setEllipsesEnabled(bool enabled)                { _ellipsesEnabled = enabled; return *this; }

        const std::string& fontTexture() const                 { return _texture; }
        const std::string& fontMetadata() const                { return _metadata; }
        double textSize() const                                { return _textSize; }
        const Color& textColor() const                         { return _textColor; }
        double letterSpacing() const                           { return _letterSpacing; }
        HorizontalAlignment horizontalAlignment() const        { return _horizontalAlignment; }
        VerticalAlignment verticalAlignment() const            { return _verticalAlignment; }
        OverflowBehavior overflowBehavior() const              { return _overflowBehavior; }
        double weight() const                                  { return _weight; }
        bool ellipsesEnabled() const                           { return _ellipsesEnabled; }

    private:
        std::string         _texture;
        std::string         _metadata;
        double              _textSize = 12;
        Color               _textColor = Color::kBlack;
        double              _letterSpacing = 0.0;
        HorizontalAlignment _horizontalAlignment = HorizontalAlignment::kLeft;
        VerticalAlignment   _verticalAlignment = VerticalAlignment::kCenter;
        OverflowBehavior    _overflowBehavior = OverflowBehavior::kWrap;
        double              _weight = 100.0;
        bool                _ellipsesEnabled = true;
    };

    const Style& style() const { return _style; }
    double lineHeight() const;
    size_t lineCount() const { return _lines.size(); }
    size_t lineCountForWidth(double width) const { return _computeLines(width, std::numeric_limits<double>::infinity()).size(); }
    double textWidth() const { return _textWidth; }
    double textHeight() const { return lineHeight() * std::max<double>(lineCount(), 1); }
    double textHeightForWidth(double width) const { return lineHeight() * std::max<double>(lineCountForWidth(width), 1); }
    Point<int> lineColumnPosition(size_t line, size_t column) const;
    std::pair<size_t, size_t> lineColumnAtPosition(int x, int y) const;

    void setText(stdts::string_view text);
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
    void setEllipsesEnabled(bool enabled);

    virtual void render(const RenderTarget* renderTarget, const Rectangle<int>& area) override;
    virtual void layout() override;
    virtual void windowChanged() override;

private:
    std::vector<std::basic_string<BitmapFont::GlyphId>> _computeLines(double width, double height) const;
    void _updateLines();
    void _renderBitmapText(shaders::DistanceFieldShader* shader);
    double _calcXOffset(const std::basic_string<BitmapFont::GlyphId>& line) const;
    double _calcYOffset() const;
    double _fontScale() const;
    double _lineWidth(stdts::basic_string_view<BitmapFont::GlyphId> line, double scale) const;

    Style                                               _style;
    std::shared_ptr<BitmapFont>                         _font;
    std::basic_string<BitmapFont::GlyphId>              _glyphs;
    std::string                                         _text;
    std::vector<std::basic_string<BitmapFont::GlyphId>> _lines;
    double                                              _textWidth = 0;
};

} } // namespace okui::views
