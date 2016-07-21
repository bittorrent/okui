#pragma once

#include "onair/okui/config.h"

#include "scraps/utility.h"

#include <cmath>

/**
* See easings.net and gizma.com/easing for examples/math
*
* b: start value
* c: change in value
* s: percent complete (0-1)
*/
namespace onair {
namespace okui {
namespace interpolation {

template <typename T>
constexpr T Linear(const T& b, const T& c, double s) {
    return b + c*s;
}

struct Sine {
    template <typename T>
    static T EaseIn(const T& b, const T& c, double s) {
        return -c * cos(s * (scraps::kPi/2)) + c + b;
    }

    template <typename T>
    static T EaseOut(const T& b, const T& c, double s) {
        return c * sin(s * (scraps::kPi/2)) + b;
    }

    template <typename T>
    static T EaseInOut(const T& b, const T& c, double s) {
        return -c/2 * (cos(scraps::kPi * s) - 1) + b;
    }
};

struct Quadratic {
    template <typename T>
    static constexpr T EaseIn(const T& b, const T& c, double s) {
        return c*s*s + b;
    }

    template <typename T>
    static constexpr T EaseOut(const T& b, const T& c, double s) {
        return -c * s*(s-2) + b;
    }

    template <typename T>
    static constexpr T EaseInOut(const T& b, const T& c, double s) {
        s *= 2;
        if (s < 1)
            return c/2 * s*s + b;
        --s;
        return -c/2 * (s*(s-2) - 1) + b;
    }
};

struct Cubic {
    template <typename T>
    static constexpr T EaseIn(const T& b, const T& c, double s) {
        return c * s*s*s + b;
    }

    template <typename T>
    static constexpr T EaseOut(const T& b, const T& c, double s) {
        --s;
        return c * (s*s*s + 1) + b;
    }

    template <typename T>
    static constexpr T EaseInOut(const T& b, const T& c, double s) {
        s *= 2;
        if (s < 1)
            return c/2 * s*s*s + b;
        s -= 2;
        return c/2 * (s*s*s + 2) + b;
    }
};

struct Quartic {
    template <typename T>
    static constexpr T EaseIn(const T& b, const T& c, double s) {
        return c * s*s*s*s + b;
    }

    template <typename T>
    static constexpr T EaseOut(const T& b, const T& c, double s) {
        --s;
        return -c * (s*s*s*s - 1) + b;
    }

    template <typename T>
    static constexpr T EaseInOut(const T& b, const T& c, double s) {
        s *= 2;
        if (s < 1)
            return c/2 * s*s*s*s + b;
        s -= 2;
        return -c/2 * (s*s*s*s - 2) + b;
    }
};

struct Quintic {
    template <typename T>
    static constexpr T EaseIn(const T& b, const T& c, double s) {
        return c * s*s*s*s*s + b;
    }

    template <typename T>
    static constexpr T EaseOut(const T& b, const T& c, double s) {
        --s;
        return c*(s*s*s*s*s + 1) + b;
    }

    template <typename T>
    static constexpr T EaseInOut(const T& b, const T& c, double s) {
        s *= 2;
        if (s < 1)
            return c/2 * s*s*s*s*s + b;
        s -= 2;
        return c/2 * (s*s*s*s*s + 2) + b;
    }
};

struct Exponential {
    template <typename T>
    static T EaseIn(const T& b, const T& c, double s) {
        return c * pow(2, 10 * (s - 1)) + b;
    }

    template <typename T>
    static T EaseOut(const T& b, const T& c, double s) {
        return c * (-pow(2, -10 * s) + 1) + b;
    }

    template <typename T>
    static T EaseInOut(const T& b, const T& c, double s) {
        s *= 2;
        if (s < 1)
            return c/2 * pow(2, 10 * (s - 1)) + b;
        --s;
        return c/2 * (-pow(2, -10 * s) + 2) + b;
    }
};

struct Circular {
    template <typename T>
    static T EaseIn(const T& b, const T& c, double s) {
        return -c * (sqrt(1 - s*s) - 1) + b;
    }

    template <typename T>
    static T EaseOut(const T& b, const T& c, double s) {
        --s;
        return c * sqrt(1 - s*s) + b;
    }

    template <typename T>
    static T EaseInOut(const T& b, const T& c, double s) {
        s *= 2;
        if (s < 1)
            return -c/2 * (sqrt(1 - s*s) - 1) + b;
        s -= 2;
        return c/2 * (sqrt(1 - s*s) + 1) + b;
    }
};

struct Back {
    template <typename T>
    static constexpr T EaseIn(const T& b, const T& c, double s) {
        constexpr auto f = 1.70158;
        return c * s*s * ((f+1)*s - f) + b;
    }

    template <typename T>
    static constexpr T EaseOut(const T& b, const T& c, double s) {
        constexpr auto f = 1.70158;
        --s;
        return c * (s*s * ((f+1)*s + f) + 1) + b;
    }

    template <typename T>
    static constexpr T EaseInOut(const T& b, const T& c, double s) {
        constexpr auto f = 1.70158 * 1.525;
        s *= 2;
        if (s < 1)
            return c/2 * (s*s * ((f+1)*s - f)) + b;
        s -= 2;
        return c/2 * (s*s * ((f+1)*s + f) + 2) + b;
    }
};

}}} // onair::okui::interpolation
