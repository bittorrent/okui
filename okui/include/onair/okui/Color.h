#pragma once

#include "onair/okui/config.h"

namespace onair {
namespace okui {

struct Color {
    constexpr Color() = default;
    constexpr explicit Color(double brightness, double alpha = 1.0) : r{brightness}, g{brightness}, b{brightness}, a{alpha} {}
    constexpr Color(double red, double green, double blue, double alpha = 1.0) : r{red}, g{green}, b{blue}, a{alpha} {}

    constexpr Color operator*(double multiplier) const {
        return Color(r * multiplier, g * multiplier, b * multiplier, a * multiplier);
    }

    constexpr Color operator+(const Color& other) const {
        return Color(r + other.r, g + other.g, b + other.b, a + other.a);
    }

    constexpr Color interpolate(const Color& other, double factor) const {
        return *this * (1.0 - factor) + other * factor;
    }

    constexpr bool operator==(const Color& other) const {
        return r == other.r && g == other.g && b == other.b && a == other.a;
    }

    constexpr bool operator!=(const Color& other) const {
        return !(*this == other);
    }

    double r{1.0}, g{1.0}, b{1.0}, a{1.0};
};

namespace detail {

template <typename CharT>
constexpr int8_t HexToDec(CharT c) {
    if (c >= '0' && c <= '9') {
        return c - '0';
    }
    if (c >= 'A' && c <= 'F') {
        return c - 'A' + 10;
    }
    if (c >= 'a' && c <= 'f') {
        return c - 'a' + 10;
    }
    return -1;
}

template <char Char>
struct HexConversion {
    static_assert((Char >= '0' && Char <= '9') || (Char >= 'a' && Char <= 'f') || (Char >= 'A' && Char <= 'F'), "non-hex character");
    static constexpr uint8_t value = HexToDec(Char);
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
