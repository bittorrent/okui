#pragma once
#include "bittorrent/ui/Point.h"

namespace bittorrent {
namespace ui {

template<typename T>
struct Rectangle {
    Rectangle();
    Rectangle(T x, T y, T width, T height);
    Rectangle(const Point<T>& topLeft, const Point<T>& bottomRight);

    T left() const               { return x; }
    T right() const              { return x + width; }
    T top() const                { return y; }
    T bottom() const             { return y + height; }
    Point<T> topLeft() const     { return Point<T>{left(), top()}; }
    Point<T> topRight() const    { return Point<T>{right(), top()}; }
    Point<T> bottomLeft() const  { return Point<T>{left(), bottom()}; }
    Point<T> bottomRight() const { return Point<T>{right(), bottom()}; }

    bool contains(const Point<T>& p) const;
    bool intersects(const Rectangle& other) const;
    Rectangle intersection(const Rectangle& other) const;

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
Rectangle<T>::Rectangle(const Point<T>& topLeft, const Point<T>& bottomRight)
    : x{topLeft.x}
    , y{topLeft.y}
    , width{bottomRight.x-topLeft.x}
    , height{bottomRight.y-topLeft.y}
{}

template<typename T>
bool Rectangle<T>::contains(const Point<T>& p) const {
    return p.x >= x && p.x < right() &&
           p.y >= y && p.y < bottom();
}

template<typename T>
bool Rectangle<T>::intersects(const Rectangle& other) const {
    return left() < other.right() && right() > other.left() &&
           bottom() > other.top() && top() < other.bottom();
}

template<typename T>
Rectangle<T> Rectangle<T>::intersection(const Rectangle& other) const {
    auto topLeft     = Point<T>{std::max(left(), other.left()), std::max(top(), other.top())},
         bottomRight = Point<T>{std::min(right(), other.right()), std::min(bottom(), other.bottom())};

    if (bottomRight.x < topLeft.x) { topLeft.x = bottomRight.x; }
    if (bottomRight.y < topLeft.y) { topLeft.y = bottomRight.y; }

    return Rectangle<T>{topLeft, bottomRight};
}

}}
