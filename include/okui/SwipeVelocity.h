#pragma once

#include "okui/config.h"

#include "okui/Animation.h"
#include "okui/Point.h"

#include <chrono>
#include <deque>

namespace okui {

class SwipeVelocity {
public:
    explicit SwipeVelocity(std::chrono::steady_clock::duration swipeTimeout = 150ms,
                           double stoppedVelocityThreshold = 100,
                           std::chrono::steady_clock::duration minSwipeTime = 50ms);

    void pushPoint(Point<double> p);

    /**
    * returns velocity of the swipe
    */
    math::Vector<double, 2> endSwipe();

    void reset();

private:
    std::chrono::steady_clock::duration                                         _swipeTimeout;
    double                                                                      _stoppedVelocityThreshold;
    std::chrono::steady_clock::duration                                         _minSwipeTime;
    std::deque<std::pair<std::chrono::steady_clock::time_point, Point<double>>> _history;
};

} // namespace okui
