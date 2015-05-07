#pragma once

namespace bittorrent {
namespace ui {

struct Color {
    Color() = default;
    explicit Color(double brightness, double alpha = 1.0) : r{brightness}, g{brightness}, b{brightness}, a{alpha} {}
    Color(double red, double green, double blue, double alpha = 1.0) : r{red}, g{green}, b{blue}, a{alpha} {}

    Color operator*(double multiplier) const {
        return Color(r * multiplier, g * multiplier, b * multiplier, a * multiplier);
    }

    Color operator+(const Color& other) const {
        return Color(r + other.r, g + other.g, b + other.b, a + other.a);
    }

    Color interpolate(const Color& other, double factor) const {
        return *this * (1.0 - factor) + other * factor;
    }

    double r{1.0}, g{1.0}, b{1.0}, a{1.0};
};

}}
