#pragma once

#include <okui/config.h>

#include <scraps/utility.h>

#include <cstdint>

namespace okui {

class Color {
public:
    constexpr Color() = default;

    friend constexpr Color RGB(uint8_t r, uint8_t g, uint8_t b) noexcept;
    friend constexpr Color RGBA(uint8_t r, uint8_t g, uint8_t b, uint8_t a) noexcept;
    friend constexpr Color RGBF(double r, double g, double b) noexcept;
    friend constexpr Color RGBAF(double r, double g, double b, double a) noexcept;

    constexpr uint8_t red() const noexcept                              { return static_cast<uint8_t>(r * 255.0); }
    constexpr double  redF() const noexcept                             { return r; }
    constexpr uint8_t green() const noexcept                            { return static_cast<uint8_t>(g * 255.0); }
    constexpr double  greenF() const noexcept                           { return g; }
    constexpr uint8_t blue() const noexcept                             { return static_cast<uint8_t>(b * 255.0); }
    constexpr double  blueF() const noexcept                            { return b; }
    constexpr uint8_t alpha() const noexcept                            { return static_cast<uint8_t>(a * 255.0); }
    constexpr double  alphaF() const noexcept                           { return a; }

    constexpr void setRed(uint8_t r) noexcept                           { this->r = r/255.0; }
    constexpr void setRedF(double r) noexcept                           { this->r = r; }
    constexpr void setGreen(uint8_t g) noexcept                         { this->g = g/255.0; }
    constexpr void setGreenF(double g) noexcept                         { this->g = g; }
    constexpr void setBlue(uint8_t b) noexcept                          { this->b = b/255.0; }
    constexpr void setBlueF(double b) noexcept                          { this->b = b; }
    constexpr void setAlpha(uint8_t a) noexcept                         { this->a = a/255.0; }
    constexpr void setAlphaF(double a) noexcept                         { this->a = a; }

    constexpr Color withAlpha(uint8_t a) const noexcept                 { return {r, g, b, a/255.0}; }
    constexpr Color withAlphaF(double a) const noexcept                 { return {r, g, b, a}; }

    constexpr Color& operator+=(const Color& c) noexcept                { return *this = *this + c; }
    constexpr Color& operator-=(const Color& c) noexcept                { return *this = *this - c; }
    constexpr Color& operator*=(double s) noexcept                      { return *this = *this * s; }
    constexpr Color& operator/=(double s) noexcept                      { return *this = *this / s; }

    constexpr Color operator+(const Color& c) const noexcept            { return {r + c.r, g + c.g, b + c.b, a + c.a}; }
    constexpr Color operator-(const Color& c) const noexcept            { return {r - c.r, g - c.g, b - c.b, a - c.a}; }
    constexpr Color operator*(double s) const noexcept                  { return {r * s, g * s, b * s, a * s}; }
    friend constexpr Color operator*(double s, const Color& c) noexcept { return c * s; }
    constexpr Color operator/(double s) const noexcept                  { return {r / s, g / s, b / s, a / s}; }

    constexpr bool operator==(const Color& c) const noexcept            { return r == c.r && g == c.g && b == c.b && a == c.a; }
    constexpr bool operator!=(const Color& c) const noexcept            { return !(*this == c); }

    static const Color kWhite;
    static const Color kSilver;
    static const Color kGray;
    static const Color kBlack;
    static const Color kTransparentBlack;
    static const Color kRed;
    static const Color kMaroon;
    static const Color kYellow;
    static const Color kOlive;
    static const Color kLime;
    static const Color kGreen;
    static const Color kAqua;
    static const Color kTeal;
    static const Color kBlue;
    static const Color kNavy;
    static const Color kFuchsia;
    static const Color kPurple;

private:
    constexpr Color(double r, double g, double b, double a = 1) noexcept : r{r}, g{g}, b{b}, a{a} {}

    double r{1.0}, g{1.0}, b{1.0}, a{1.0};
};

constexpr Color RGB(uint8_t r, uint8_t g, uint8_t b) noexcept             { return {r/255.0, g/255.0, b/255.0}; }
constexpr Color RGBA(uint8_t r, uint8_t g, uint8_t b, uint8_t a) noexcept { return {r/255.0, g/255.0, b/255.0, a/255.0}; }
constexpr Color RGBF(double r, double g, double b) noexcept               { return {r, g, b}; }
constexpr Color RGBAF(double r, double g, double b, double a) noexcept    { return {r, g, b, a}; }

namespace detail {

template <char Char>
struct HexConversion {
    static_assert((Char >= '0' && Char <= '9') || (Char >= 'a' && Char <= 'f') || (Char >= 'A' && Char <= 'F'), "non-hex character");
    static constexpr uint8_t value = scraps::HexToDec(Char);
};

template <char Zero, char X, char RHi, char RLo, char GHi, char GLo, char BHi, char BLo, char AHi, char ALo>
struct ColorFactory {
    static_assert(Zero == '0' && X == 'x', "color literal must be prefixed with 0x");
    constexpr Color operator()() {
        return RGBA(
            HexConversion<RHi>::value << 4 | HexConversion<RLo>::value,
            HexConversion<GHi>::value << 4 | HexConversion<GLo>::value,
            HexConversion<BHi>::value << 4 | HexConversion<BLo>::value,
            HexConversion<AHi>::value << 4 | HexConversion<ALo>::value
        );
    }
};

} // namespace detail

inline namespace literals {

template <char... Chars>
constexpr Color operator""_rgb() {
    return detail::ColorFactory<Chars..., 'f', 'f'>()();
}

template <char... Chars>
constexpr Color operator""_rgba() {
    return detail::ColorFactory<Chars...>()();
}

} // namespace literals

} // namespace okui
