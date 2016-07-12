#pragma once

#include "onair/okui/config.h"

#include "scraps/utility.h"

namespace onair {
namespace okui {

struct Color {
    constexpr Color() = default;
    constexpr explicit Color(double brightness, double alpha = 1.0) : r{brightness}, g{brightness}, b{brightness}, a{alpha} {}
    constexpr Color(double red, double green, double blue, double alpha = 1.0) : r{red}, g{green}, b{blue}, a{alpha} {}

    constexpr Color& operator+=(const Color& c)                { return *this = *this + c; }
    constexpr Color& operator-=(const Color& c)                { return *this = *this - c; }
    constexpr Color& operator*=(double s)                      { return *this = *this * s; }
    constexpr Color& operator/=(double s)                      { return *this = *this / s; }

    constexpr Color operator+(const Color& c) const            { return {r + c.r, g + c.g, b + c.b, a + c.a}; }
    constexpr Color operator-(const Color& c) const            { return {r - c.r, g - c.g, b - c.b, a - c.a}; }
    constexpr Color operator*(double s) const                  { return {r * s, g * s, b * s, a * s}; }
    constexpr friend Color operator*(double s, const Color& c) { return c * s; }
    constexpr Color operator/(double s) const                  { return {r / s, g / s, b / s, a / s}; }

    constexpr bool operator==(const Color& c) const            { return r == c.r && g == c.g && b == c.b && a == c.a; }
    constexpr bool operator!=(const Color& c) const            { return !(*this == c); }

    double r{1.0}, g{1.0}, b{1.0}, a{1.0};
};

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
        return Color(
            (HexConversion<RHi>::value << 4 | HexConversion<RLo>::value) / 255.0,
            (HexConversion<GHi>::value << 4 | HexConversion<GLo>::value) / 255.0,
            (HexConversion<BHi>::value << 4 | HexConversion<BLo>::value) / 255.0,
            (HexConversion<AHi>::value << 4 | HexConversion<ALo>::value) / 255.0
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

}} // namespace onair::okui
