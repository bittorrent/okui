#pragma once

namespace bittorrent {
namespace ui {

template <typename T>
struct Point {
    Point() : x{0}, y{0} {}
    Point(T x, T y) : x{x}, y{y} {}

    T x, y;
};

}}
