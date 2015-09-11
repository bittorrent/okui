#pragma once

#include "onair/okui/Point.h"

#include <cmath>

namespace onair {
namespace okui {

template <typename T>
struct Rectangle {
    Rectangle();
    Rectangle(T x, T y, T width, T height);
    Rectangle(Point<T> position, T width, T height);
    Rectangle(Point<T> position, math::Vector<T, 2> size);

    template <typename U>
    explicit Rectangle(const Rectangle<U>& other)
        : Rectangle(other.x, other.y, other.width, other.height) {}

    Point<T> position() const { return Point<T>{x, y}; }
    math::Vector<T, 2> size() const { return math::Vector<T, 2>{width, height}; }

    T minX() const { return x; }
    T maxX() const { return x + width; }
    T minY() const { return y; }
    T maxY() const { return y + height; }

    Rectangle<T> withPosition(T x, T y) const { return {x, y, width, height}; }
    Rectangle<T> withSize(T width, T height) const { return {x, y, width, height}; }

    /**
    * Tests if the rectangle contains the given point.
    *
    * The rectangle's dimensons are considered non-inclusive. For example, a 1x1 rectangle
    * at 0,0 does NOT contain the point at 1,1.
    */
    bool contains(T x, T y) const;
    bool contains(const Point<T>& p) const { return contains(p.x, p.y); }

    /**
    * Returns the distance from the given point to the edge of the rectangle.
    *
    * The returned distance is negative if the point is inside the rectangle.
    */
    double distance(T x, T y) const;

    bool intersects(const Rectangle& other) const;
    Rectangle intersection(const Rectangle& other) const;

    /**
    * Returns the area that another rectangle of the given aspect ratio (width / height) would
    * need to occupy to fill this rectangle via scaling only.
    */
    Rectangle<double> scaledFill(double aspectRatio) const;

    /**
    * Returns the area that another rectangle of the given aspect ratio (width / height) would
    * need to occupy to fit (but not necessarily fill) this rectangle via scaling only.
    */
    Rectangle<double> scaledFit(double aspectRatio) const;

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

template <typename T>
Rectangle<T>::Rectangle()
    : x{0}
    , y{0}
    , width{0}
    , height{0}
{}

template <typename T>
Rectangle<T>::Rectangle(T x, T y, T width, T height)
    : x{x}
    , y{y}
    , width{width}
    , height{height}
{}

template <typename T>
Rectangle<T>::Rectangle(Point<T> pos, T width, T height)
    : x{pos.x}
    , y{pos.y}
    , width{width}
    , height{height}
{}

template <typename T>
Rectangle<T>::Rectangle(Point<T> pos, math::Vector<T, 2> size)
    : x{pos.x}
    , y{pos.y}
    , width{size.x}
    , height{size.y}
{}

template <typename T>
bool Rectangle<T>::contains(T x, T y) const {
    return x >= minX() && x < maxX() && y >= minY() && y < maxY();
}

template <typename T>
double Rectangle<T>::distance(T x, T y) const {
    if (contains(x, y)) {
        return -std::min({x - minX(), maxX() - x, y - minY(), maxY() - y});
    }
    auto xDistance = x < minX() ? (minX() - x) : x >= maxX() ? (x - maxX()) : 0;
    auto yDistance = y < minY() ? (minY() - y) : y >= maxY() ? (y - maxY()) : 0;
    return sqrt(xDistance * xDistance + yDistance * yDistance);
}

template <typename T>
bool Rectangle<T>::intersects(const Rectangle& other) const {
    return minX() < other.maxX() && maxX() > other.minX() && minY() < maxY() && maxY() > other.minY();
}

template <typename T>
Rectangle<T> Rectangle<T>::intersection(const Rectangle& other) const {
    auto min = Point<T>{std::max(minX(), other.minX()), std::max(minY(), other.minY())},
         max = Point<T>{std::min(maxX(), other.maxX()), std::min(maxY(), other.maxY())};

    if (max.x <= min.x || max.y <= min.y) { return Rectangle<T>(); }

    return Rectangle<T>{min.x, min.y, max.x - min.x, max.y - min.y};
}

template <typename T>
Rectangle<double> Rectangle<T>::scaledFill(double aspectRatio) const {
    auto scaledHeight = std::max((double)width / aspectRatio, (double)height);
    auto scaledWidth = scaledHeight * aspectRatio;
    return Rectangle<double>(x - 0.5 * (scaledWidth - width), y - 0.5 * (scaledHeight - height), scaledWidth, scaledHeight);
}

template <typename T>
Rectangle<double> Rectangle<T>::scaledFit(double aspectRatio) const {
    auto scaledHeight = std::min((double)width / aspectRatio, (double)height);
    auto scaledWidth = scaledHeight * aspectRatio;
    return Rectangle<double>(x - 0.5 * (scaledWidth - width), y - 0.5 * (scaledHeight - height), scaledWidth, scaledHeight);
}

}}
