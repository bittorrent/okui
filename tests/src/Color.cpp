#include "gtest/gtest.h"

#include "okui/Color.h"

using namespace okui;

// construction
static_assert(RGBAF(1.0, 1.0, 1.0, 1.0) == RGBA(255, 255, 255, 255), "test failed");
static_assert(RGB(0, 0, 0) == RGBAF(0, 0, 0, 1), "test failed");
static_assert(RGBF(2.0, 3.0, 4.0) == RGBAF(2.0, 3.0, 4.0, 1.0), "test failed");

// multiplication
static_assert(RGBAF(1.0, 2.0, 3.0, 4.0) * 2.0 == RGBAF(2.0, 4.0, 6.0, 8.0), "test failed");
static_assert(2.0 * RGBAF(1.0, 2.0, 3.0, 4.0) == RGBAF(2.0, 4.0, 6.0, 8.0), "test failed");
static_assert((RGBAF(1.0, 2.0, 3.0, 4.0) *= 2.0) == RGBAF(2.0, 4.0, 6.0, 8.0), "test failed");

// division
static_assert(RGBAF(1.0, 2.0, 3.0, 4.0) / 2.0 == RGBAF(0.5, 1.0, 1.5, 2.0), "test failed");
static_assert((RGBAF(1.0, 2.0, 3.0, 4.0) /= 2.0) == RGBAF(0.5, 1.0, 1.5, 2.0), "test failed");

// addition
static_assert(RGBAF(1.0, 2.0, 3.0, 4.0) + RGBAF(2.0, 3.0, 4.0, 5.0) == RGBAF(3.0, 5.0, 7.0, 9.0), "test failed");
static_assert((RGBAF(1.0, 2.0, 3.0, 4.0) += RGBAF(2.0, 3.0, 4.0, 5.0)) == RGBAF(3.0, 5.0, 7.0, 9.0), "test failed");

// subtraction
static_assert(RGBAF(1.0, 2.0, 3.0, 4.0) - RGBAF(2.0, 3.0, 4.0, 5.0) == RGBAF(-1.0, -1.0, -1.0, -1.0), "test failed");
static_assert((RGBAF(1.0, 2.0, 3.0, 4.0) -= RGBAF(2.0, 3.0, 4.0, 5.0)) == RGBAF(-1.0, -1.0, -1.0, -1.0), "test failed");

// comparison
static_assert(RGBAF(1.0, 2.0, 3.0, 4.0) != RGBAF(2.0, 3.0, 4.0, 5.0), "test failed");
static_assert(RGBAF(1.0, 2.0, 3.0, 4.0) == RGBAF(1.0, 2.0, 3.0, 4.0), "test failed");
static_assert(RGBAF(0.0, 2.0, 3.0, 4.0) != RGBAF(1.0, 2.0, 3.0, 4.0), "test failed");
static_assert(RGBAF(1.0, 0.0, 3.0, 4.0) != RGBAF(1.0, 2.0, 3.0, 4.0), "test failed");
static_assert(RGBAF(1.0, 2.0, 0.0, 4.0) != RGBAF(1.0, 2.0, 3.0, 4.0), "test failed");
static_assert(RGBAF(1.0, 2.0, 3.0, 0.0) != RGBAF(1.0, 2.0, 3.0, 4.0), "test failed");

// literals
static_assert(RGBAF(1.0, 127.0 / 255.0, 1.0, 0.0) == 0xFF7FFF00_rgba, "test failed");
static_assert(RGBAF(1.0, 127.0 / 255.0, 0.0, 1.0) == 0xFF7F00_rgb, "test failed");
static_assert(RGB(0, 0, 0) == 0x000000FF_rgba, "test failed");
static_assert(RGB(0, 0, 0) == 0x000000_rgb, "test failed");
