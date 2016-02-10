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

    Color interpolate(const Color& other, double factor) const {
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

}}
