#pragma once

namespace bittorrent {
namespace ui {

template <typename T>
struct Point {
    Point() : x{0}, y{0} {}
    Point(T x, T y) : x{x}, y{y} {}

    bool operator==(const Point& other) const {
        return x == other.x && y == other.y;
    }

    bool operator!=(const Point& other) const {
        return !(*this == other);
    }

    T x, y;
};

}}
