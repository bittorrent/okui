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

template <typename Condition, char Head, char... Tail>
struct AllOf {
    static constexpr bool value = Condition::template Evaluate<Head>::value && AllOf<Condition, Tail...>::value;
};

template <typename Condition, char Head>
struct AllOf<Condition, Head> {
    static constexpr bool value = Condition::template Evaluate<Head>::value;
};

constexpr char ToUpper(const char c) {
    if (c >= 'a' && c <= 'z') {
        return c - 'a' + 'A';
    }
    return c;
}

struct IsHexDigit {
    template <char Char>
    struct Evaluate {
        static constexpr bool value = (Char >= '0' && Char <= '9') || (ToUpper(Char) >= 'A' && ToUpper(Char) <= 'F');
    };
};

constexpr int HexToDec(char c) {
    if (c >= '0' && c <= '9') {
        return c - '0';
    }
    return ToUpper(c) - 'A' + 10;
}

template <char Pref1, char Pref2, char... Chars>
struct ParseHex {
private:
    static_assert(Pref1 == '0' && (Pref2 == 'x' || Pref2 == 'X'), "0x is a required prefix");
    static_assert(sizeof(uintmax_t) * 2 >= sizeof...(Chars), "ParseHex overflow");
    static_assert(AllOf<IsHexDigit, Chars...>::value, "Chars must be hex digits.");

    static constexpr uintmax_t Parse() {
        uintmax_t ret = 0;

        for (auto&& c : {Chars...}) {
            ret = (ret << 4) + HexToDec(c);
        }

        return ret;
    }
public:
    static constexpr uintmax_t value = Parse();
};

} // namespace detail

template <char... Chars>
constexpr Color operator""_rgba() {
    constexpr auto val = detail::ParseHex<Chars...>::value;

    static_assert(sizeof...(Chars) == 10, "_rgb length out of bounds");
    static_assert(val < 0xFFFFFFFF, "color out of bounds");

    const auto r = ((val & 0xFF000000) >> 24) / 255.0;
    const auto g = ((val & 0x00FF0000) >> 16) / 255.0;
    const auto b = ((val & 0x0000FF00) >> 8 ) / 255.0;
    const auto a = ((val & 0x000000FF)      ) / 255.0;

    return Color{r, g, b, a};
}

template <char... Chars>
constexpr Color operator""_rgb() {
    static_assert(sizeof...(Chars) == 8, "_rgb length out of bounds");

    return operator""_rgba<Chars..., 'F', 'F'>();
}

}} // namespace onair::okui
