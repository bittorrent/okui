#include "gtest/gtest.h"

#include "onair/okui/Color.h"

using namespace onair::okui;

// construction
static_assert(Color(1.0) == Color(1.0, 1.0, 1.0, 1.0), "test failed");
static_assert(Color(1.0, 0.5) == Color(1.0, 1.0, 1.0, 0.5), "test failed");
static_assert(Color(2.0, 3.0, 4.0) == Color(2.0, 3.0, 4.0, 1.0), "test failed");

// multiplication
static_assert(Color(1.0, 2.0, 3.0, 4.0) * 2.0 == Color(2.0, 4.0, 6.0, 8.0), "test failed");
static_assert(2.0 * Color(1.0, 2.0, 3.0, 4.0) == Color(2.0, 4.0, 6.0, 8.0), "test failed");
static_assert((Color(1.0, 2.0, 3.0, 4.0) *= 2.0) == Color(2.0, 4.0, 6.0, 8.0), "test failed");

// division
static_assert(Color(1.0, 2.0, 3.0, 4.0) / 2.0 == Color(0.5, 1.0, 1.5, 2.0), "test failed");
static_assert((Color(1.0, 2.0, 3.0, 4.0) /= 2.0) == Color(0.5, 1.0, 1.5, 2.0), "test failed");

// addition
static_assert(Color(1.0, 2.0, 3.0, 4.0) + Color(2.0, 3.0, 4.0, 5.0) == Color(3.0, 5.0, 7.0, 9.0), "test failed");
static_assert((Color(1.0, 2.0, 3.0, 4.0) += Color(2.0, 3.0, 4.0, 5.0)) == Color(3.0, 5.0, 7.0, 9.0), "test failed");

// subtraction
static_assert(Color(1.0, 2.0, 3.0, 4.0) - Color(2.0, 3.0, 4.0, 5.0) == Color(-1.0, -1.0, -1.0, -1.0), "test failed");
static_assert((Color(1.0, 2.0, 3.0, 4.0) -= Color(2.0, 3.0, 4.0, 5.0)) == Color(-1.0, -1.0, -1.0, -1.0), "test failed");

// comparison
static_assert(Color(1.0, 2.0, 3.0, 4.0) != Color(2.0, 3.0, 4.0, 5.0), "test failed");
static_assert(Color(1.0, 2.0, 3.0, 4.0) == Color(1.0, 2.0, 3.0, 4.0), "test failed");
static_assert(Color(0.0, 2.0, 3.0, 4.0) != Color(1.0, 2.0, 3.0, 4.0), "test failed");
static_assert(Color(1.0, 0.0, 3.0, 4.0) != Color(1.0, 2.0, 3.0, 4.0), "test failed");
static_assert(Color(1.0, 2.0, 0.0, 4.0) != Color(1.0, 2.0, 3.0, 4.0), "test failed");
static_assert(Color(1.0, 2.0, 3.0, 0.0) != Color(1.0, 2.0, 3.0, 4.0), "test failed");

// literals
static_assert(Color(1.0, 127.0 / 255.0, 1.0, 0.0) == 0xFF7FFF00_rgba, "test failed");
static_assert(Color(1.0, 127.0 / 255.0, 0.0, 1.0) == 0xFF7F00_rgb, "test failed");
static_assert(Color(0.0) == 0x000000FF_rgba, "test failed");
static_assert(Color(0.0) == 0x000000_rgb, "test failed");
