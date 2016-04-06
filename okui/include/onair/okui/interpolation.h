#pragma once

#include "onair/okui/config.h"

#include "scraps/utility.h"

#include <cmath>

/**
* See easings.net and gizma.com/easing for examples/math
*
* t: elapsed time
* b: start value
* c: change in value
* d: duration
*/
namespace onair {
namespace okui {
namespace interpolation {

template <class T>
constexpr T Linear(double t, const T& b, const T& c, double d) {
    return c * t/d + b;
}

struct Sine {
    template <class T>
    static T EaseIn(double t, const T& b, const T& c, double d) {
        return -c * cos(t/d * (scraps::kPi/2)) + c + b;
    }

    template <class T>
    static T EaseOut(double t, const T& b, const T& c, double d) {
        return c * sin(t/d * (scraps::kPi/2)) + b;
    }

    template <class T>
    static T EaseInOut(double t, const T& b, const T& c, double d) {
        return -c/2 * (cos(scraps::kPi * t/d) - 1) + b;
    }
};

struct Quadratic {
    template <class T>
    static constexpr T EaseIn(double t, const T& b, const T& c, double d) {
        t /= d;
        return c*t*t + b;
    }

    template <class T>
    static constexpr T EaseOut(double t, const T& b, const T& c, double d) {
        t /= d;
        return -c * t*(t-2) + b;
    }

    template <class T>
    static constexpr T EaseInOut(double t, const T& b, const T& c, double d) {
        t /= d/2;
        if (t < 1) {
            return c/2 * t*t + b;
        }
        t -= 1;
        return -c/2 * (t*(t-2) - 1) + b;
    }
};

struct Cubic {
    template <class T>
    static constexpr T EaseIn(double t, const T& b, const T& c, double d) {
        t /= d;
        return c * t*t*t + b;
    }

    template <class T>
    static constexpr T EaseOut(double t, const T& b, const T& c, double d) {
        t = t/d - 1;
        return c * (t*t*t + 1) + b;
    }

    template <class T>
    static constexpr T EaseInOut(double t, const T& b, const T& c, double d) {
        t /= d/2;
        if (t < 1) {
            return c/2 * t*t*t + b;
        }
        t -= 2;
        return c/2 * (t*t*t + 2) + b;
    }
};

struct Quartic {
    template <class T>
    static constexpr T EaseIn(double t, const T& b, const T& c, double d) {
        t /= d;
        return c * t*t*t*t + b;
    }

    template <class T>
    static constexpr T EaseOut(double t, const T& b, const T& c, double d) {
        t = t/d - 1;
        return -c * (t*t*t*t - 1) + b;
    }

    template <class T>
    static constexpr T EaseInOut(double t, const T& b, const T& c, double d) {
        t /= d/2;
        if (t < 1) {
            return c/2 * t*t*t*t + b;
        }
        t -= 2;
        return -c/2 * (t*t*t*t - 2) + b;
    }
};

struct Quintic {
    template <class T>
    static constexpr T EaseIn(double t, const T& b, const T& c, double d) {
        t /= d;
        return c * t*t*t*t*t + b;
    }

    template <class T>
    static constexpr T EaseOut(double t, const T& b, const T& c, double d) {
        t = t/d - 1;
        return c*(t*t*t*t*t + 1) + b;
    }

    template <class T>
    static constexpr T EaseInOut(double t, const T& b, const T& c, double d) {
        t /= d/2;
        if (t < 1) {
            return c/2 * t*t*t*t*t + b;
        }
        t -= 2;
        return c/2 * (t*t*t*t*t + 2) + b;
    }
};

struct Exponential {
    template <class T>
    static T EaseIn(double t, const T& b, const T& c, double d) {
        return c * pow(2, 10 * (t/d - 1)) + b;
    }

    template <class T>
    static T EaseOut(double t, const T& b, const T& c, double d) {
        return c * (-pow(2, -10 * t/d) + 1) + b;
    }

    template <class T>
    static T EaseInOut(double t, const T& b, const T& c, double d) {
        t /= d/2;
        if (t < 1) {
            return c/2 * pow(2, 10 * (t - 1)) + b;
        }
        t -= 1;
        return c/2 * (-pow(2, -10 * t) + 2) + b;
    }
};

struct Circular {
    template <class T>
    static T EaseIn(double t, const T& b, const T& c, double d) {
        t /= d;
        return -c * (sqrt(1 - t*t) - 1) + b;
    }

    template <class T>
    static T EaseOut(double t, const T& b, const T& c, double d) {
        t = t/d - 1;
        return c * sqrt(1 - t*t) + b;
    }

    template <class T>
    static T EaseInOut(double t, const T& b, const T& c, double d) {
        t /= d/2;
        if (t < 1) {
            return -c/2 * (sqrt(1 - t*t) - 1) + b;
        }
        t -= 2;
        return c/2 * (sqrt(1 - t*t) + 1) + b;
    }
};

struct Back {
    template <class T>
    static constexpr T EaseIn(double t, const T& b, const T& c, double d) {
        t /= d;
        double s = 1.70158;
        return c * t*t * ((s+1)*t - s) + b;
    }

    template <class T>
    static constexpr T EaseOut(double t, const T& b, const T& c, double d) {
        t = t/d - 1;
        double s = 1.70158;
        return c * (t*t * ((s+1)*t + s) + 1) + b;
    }

    template <class T>
    static constexpr T EaseInOut(double t, const T& b, const T& c, double d) {
        t /= d/2;
        double s = 1.70158 * 1.525;
        if (t < 1) {
            return c/2 * (t*t * ((s+1)*t - s)) + b;
        }
        t-=2;
        return c/2 * (t*t * ((s+1)*t + s) + 2) + b;
    }
};

}}} // onair::okui::interpolation
