#pragma once
#include "bittorrent/ui/Point.h"

namespace bittorrent {
namespace ui {

template<typename T>
struct Rectangle {
    Rectangle();
    
    /**
    * Constructs a rectangle with the given origin and dimensions.
    */
    Rectangle(T x, T y, T width, T height);

    Point<T> origin() const { return Point<T>{x, y}; }
    Point<T> size() const { return Point<T>{width, height}; }

    T minX() const { return x; }
    T maxX() const { return x + width; }
    T minY() const { return y; }
    T maxY() const { return y + height; }

    /**
    * Tests if the rectangle contains the given point. 
    *
    * The rectangle's dimensons are considered non-inclusive. For example, a 1x1 rectangle 
    * at 0,0 does NOT contain the point at 1,1.
    */
    bool contains(T x, T y) const;
    bool contains(const Point<T>& p) const { return contains(p.x, p.y); }

    bool intersects(const Rectangle& other) const;
    Rectangle intersection(const Rectangle& other) const;

    bool operator==(const Rectangle& other) const {
        return x == other.x && y == other.y && width == other.width && height == other.height;
    }

    bool operator!=(const Rectangle& other) const {
        return !(*this == other);
    }

    T x;
    T y;
    T width;
    T height;
};

template<typename T>
Rectangle<T>::Rectangle()
    : x{0}
    , y{0}
    , width{0}
    , height{0}
{}

template<typename T>
Rectangle<T>::Rectangle(T x, T y, T width, T height)
    : x{x}
    , y{y}
    , width{width}
    , height{height}
{}

template<typename T>
bool Rectangle<T>::contains(T x, T y) const {
    return x >= this->x && x < this->x + width &&
           y >= this->y && y < this->y + height;
}

template<typename T>
bool Rectangle<T>::intersects(const Rectangle& other) const {
    return minX() < other.maxX() && maxX() > other.minX() && minY() < maxY() && maxY() > other.minY();
}

template<typename T>
Rectangle<T> Rectangle<T>::intersection(const Rectangle& other) const {
    auto min = Point<T>{std::max(minX(), other.minX()), std::max(minY(), other.minY())},
         max = Point<T>{std::min(maxX(), other.maxX()), std::min(maxY(), other.maxY())};

    if (max.x <= min.x || max.y <= min.y) { return Rectangle<T>(); }

    return Rectangle<T>{min.x, min.y, max.x - min.x, max.y - min.y};
}

}}
