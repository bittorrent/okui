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
#include "okui/SwipeVelocity.h"

#include "scraps/utility.h"

namespace okui {

SwipeVelocity::SwipeVelocity(std::chrono::steady_clock::duration swipeTimeout,
                             double stoppedVelocityThreshold,
                             std::chrono::steady_clock::duration minSwipeTime)
    : _swipeTimeout{swipeTimeout}
    , _stoppedVelocityThreshold{stoppedVelocityThreshold}
    , _minSwipeTime{minSwipeTime}
{}

void SwipeVelocity::pushPoint(Point<double> p) {
    auto now = std::chrono::steady_clock::now();

    while (!_history.empty() && (now - _history.front().first) > _swipeTimeout) {
        _history.pop_front();
    }

    if (!_history.empty()) {
        auto& last = _history.back();
        auto elapsed = std::chrono::duration<double>{now-last.first};
        auto lastVelocity = (p-last.second).magnitude() / elapsed.count();
        if (elapsed > 1ms && lastVelocity < _stoppedVelocityThreshold) {
            _history.clear();
        }
    }

    _history.push_back({now, p});
}

/**
* returns velocity of the swipe
*/
math::Vector<double, 2> SwipeVelocity::endSwipe() {
    auto velocity = math::Vector<double, 2>{};

    if (_history.empty()) {
        return velocity;
    }

    auto now = std::chrono::steady_clock::now();

    if (((now-_history.front().first) < _minSwipeTime || (now-_history.back().first) > _minSwipeTime)) {
        return velocity;
    }

    for (auto prev = _history.begin(), next = std::next(prev); next != _history.end(); ++prev, ++next) {
        auto elapsed    = std::chrono::duration<double>{next->first - prev->first}.count();
        auto distance   = next->second - prev->second;
        auto rate       = distance / elapsed;
        auto hysteresis = scraps::Clamp(1.0 - std::min(elapsed*2 / std::chrono::duration<double>(_swipeTimeout).count(), 1.0), 0.1, 0.75);
        velocity = velocity * hysteresis + rate * (1.0 - hysteresis);
    }

     _history.clear();

    return velocity;
}

void SwipeVelocity::reset() {
	_history.clear();
}

} // namespace okui
