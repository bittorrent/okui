#pragma once

#include "onair/okui/config.h"

#include "onair/okui/Point.h"

#include <list>

namespace onair {
namespace okui {

class View;

class TouchpadFocus {
public:
    /**
    * Returns the pressure being placed on the view to change focus.
    */
    const Point<double>& pressure() const { return _pressure; }

    /**
    * At this pressure in either direction, the view will attempt to change focus.
    */
    double pressureThreshold() const;

    void update(std::chrono::high_resolution_clock::duration elapsed, View* view, View** newFocus = nullptr);
    bool needsUpdates();

    void touchUp(Point<double> position);
    void touchDown(Point<double> position);
    void touchMovement(Point<double> position, Point<double> distance);

private:
    bool _didMove = false;
    Point<double> _velocity{0.0, 0.0};
    std::list<std::tuple<std::chrono::steady_clock::time_point, Point<double>>> _history;
    bool _isReleased = true;
    Point<double> _pressure{0.0, 0.0};
};

}}
