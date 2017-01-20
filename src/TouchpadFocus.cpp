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
#include <okui/TouchpadFocus.h>

#include <okui/View.h>
#include <okui/Window.h>

namespace okui {

constexpr auto kNavThreshold            =  0.2;
constexpr auto kMinReleaseDistance      =  0.2;
constexpr auto kMaxReleaseVelocity      = 14.0;
constexpr auto kPressureRelease         =  4.0;
constexpr auto kMinPressureRelease      =  2.0;
constexpr auto kVelocityDamping         =  2.0;

double TouchpadFocus::pressureThreshold() const {
    return kNavThreshold;
}

void TouchpadFocus::update(std::chrono::high_resolution_clock::duration elapsed, View* view, View** newFocus) {
    auto elapsedSeconds = std::chrono::duration<double>(elapsed).count();

    if (!_isReleased && !view->isFocus()) {
        _isReleased = true;
    }

    if (_isReleased) {
        auto release = _pressure * kPressureRelease;
        if (release.x > 0.0 && fabs(release.x) < kMinPressureRelease) {
            release.x *= kMinPressureRelease / release.x;
        }
        if (release.y > 0.0 && fabs(release.y) < kMinPressureRelease) {
            release.y *= kMinPressureRelease / release.y;
        }
        _pressure.x = _pressure.x > 0.0 ? std::max(0.0, _pressure.x - release.x * elapsedSeconds) : std::min(0.0, _pressure.x - release.x * elapsedSeconds);
        _pressure.y = _pressure.y > 0.0 ? std::max(0.0, _pressure.y - release.y * elapsedSeconds) : std::min(0.0, _pressure.y - release.y * elapsedSeconds);
        _pressure += _velocity * elapsedSeconds;
        _pressure.x = std::max(-1.0, std::min(1.0, _pressure.x));
        _pressure.y = std::max(-1.0, std::min(1.0, _pressure.y));
        auto damping = _velocity * kVelocityDamping;
        _velocity.x = _velocity.x > 0.0 ? std::max(0.0, _velocity.x - damping.x * elapsedSeconds) : std::min(0.0, _velocity.x - damping.x * elapsedSeconds);
        _velocity.y = _velocity.y > 0.0 ? std::max(0.0, _velocity.y - damping.y * elapsedSeconds) : std::min(0.0, _velocity.y - damping.y * elapsedSeconds);
    } else {
        _velocity = {0.0, 0.0};
    }

    auto window = view->window();
    auto threshold = pressureThreshold();
    auto slope = fabs(_pressure.y / _pressure.x);

    bool moved = false;
    if (view->isFocus()) {
        if (_pressure.x >= threshold) {
            if (slope > 0.75 && slope < 1.25) {
                if (_pressure.y > 0.0) {
                    moved = window->moveFocus(Direction::kDownRight);
                } else if (_pressure.y <= -threshold) {
                    moved = window->moveFocus(Direction::kUpRight);
                }
            }
            if (!moved) {
                moved = window->moveFocus(Direction::kRight);
            }
        } else if (_pressure.x <= -threshold) {
            if (slope > 0.75 && slope < 1.25) {
                if (_pressure.y > 0.0) {
                    moved = window->moveFocus(Direction::kDownLeft);
                } else if (_pressure.y <= -threshold) {
                    moved = window->moveFocus(Direction::kUpLeft);
                }
            }
            if (!moved) {
                moved = window->moveFocus(Direction::kLeft);
            }
        }
        if (!moved && _pressure.y >= threshold) {
            moved = window->moveFocus(Direction::kDown);
        }
        if (!moved && _pressure.y <= -threshold) {
            moved = window->moveFocus(Direction::kUp);
        }
    }

    if (moved) {
        if (auto focus = window->focus()) {
            auto& touchpadFocus = focus->touchpadFocus();
            touchpadFocus._isReleased = _isReleased;
            touchpadFocus._history = _history;
            touchpadFocus._velocity = _velocity;
            touchpadFocus._didMove = true;
            if (newFocus) {
                *newFocus = focus;
            }
        }
    } else {
        if (fabs(_pressure.x) >= threshold) {
            _velocity.x = 0.0;
        }
        if (fabs(_pressure.y) >= threshold) {
            _velocity.y = 0.0;
        }
    }
}

bool TouchpadFocus::needsUpdates() {
    return _velocity.magnitudeSquared() > 0.0001 || _pressure.magnitudeSquared() > 0.0001;
}

void TouchpadFocus::touchUp(Point<double> position) {
    _isReleased = true;
    if (_history.empty()) {
        _velocity = {0.0, 0.0};
    } else {
        _velocity = std::get<Point<double>>(_history.back()) - std::get<Point<double>>(_history.front());
        if (_velocity.magnitude() < kMinReleaseDistance) {
            _velocity = {0.0, 0.0};
        }
        _velocity /= std::chrono::duration<double>(std::get<std::chrono::steady_clock::time_point>(_history.back()) - std::get<std::chrono::steady_clock::time_point>(_history.front())).count();
        if (fabs(_velocity.x / _velocity.y) < 0.75) {
            _velocity.x = 0.0;
        } else if (fabs(_velocity.x / _velocity.y) > 1.25) {
            _velocity.y = 0.0;
        }
        auto mag = _velocity.magnitude();
        if (mag > kMaxReleaseVelocity) {
            _velocity *= kMaxReleaseVelocity / mag;
        }
    }
}

void TouchpadFocus::touchDown(Point<double> position) {
    _isReleased = false;
    _didMove = false;
    _history = {};
}

void TouchpadFocus::touchMovement(Point<double> position, Point<double> distance) {
    _isReleased = false;
    _pressure += distance * (_didMove ? 0.4 : 1.0);
    _history.emplace_back(std::chrono::steady_clock::now(), position);
    while (_history.size() > 3) {
        _history.pop_front();
    }
}

} // namespace okui
