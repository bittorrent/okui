/**
* Copyright 2017 BitTorrent Inc.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*    http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/
#pragma once

#include <okui/config.h>

#include <okui/Point.h>

#include <list>

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

} // namespace okui
