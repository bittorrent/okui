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

#include <okui/Animation.h>

#include <deque>

namespace okui {

template <typename T, typename Clock = std::chrono::steady_clock>
class AnimationChain {
public:
    using Interpolator = typename Animation<T, Clock>::Interpolator;

    AnimationChain(T initial = T{}) : _animation{std::move(initial)} {}

    T current();

    bool isComplete() const { return _chain.empty() && _animation.isComplete(); }

    template <typename... Args>
    void target(T target, typename Clock::duration duration, Args&&... args);
    template <typename... Args>
    void target(T target, typename Clock::duration duration, Interpolator interpolator, Args&&... args);

    void reset(T value);

private:
    struct Target {
        T                        target;
        typename Clock::duration duration;
        Interpolator             interpolator;
    };

    void _appendTargets() {}
    template <typename... Args>
    void _appendTargets(T target, typename Clock::duration duration, Args&&... args);
    template <typename... Args>
    void _appendTargets(T target, typename Clock::duration duration, Interpolator interpolator, Args&&... args);

    Animation<T, Clock> _animation;
    std::deque<Target>  _chain;
};

template <typename T, typename Clock>
T AnimationChain<T, Clock>::current() {
    auto now = Clock::now();
    while (_animation.end() <= now && !_chain.empty()) {
        auto& front = _chain.front();
        _animation.target(std::move(front.target), std::move(front.duration), std::move(front.interpolator));
        _chain.pop_front();
    }
    return _animation.current();
}

template <typename T, typename Clock>
template <typename... Args>
void AnimationChain<T, Clock>::target(T target, typename Clock::duration duration, Args&&... args) {
    _chain.clear();
    _animation.target(std::move(target), std::move(duration));
    _appendTargets(std::forward<Args>(args)...);
}

template <typename T, typename Clock>
template <typename... Args>
void AnimationChain<T, Clock>::target(T target, typename Clock::duration duration, Interpolator interpolator, Args&&... args) {
    _chain.clear();
    _animation.target(std::move(target), std::move(duration), std::move(interpolator));
    _appendTargets(std::forward<Args>(args)...);
}

template <typename T, typename Clock>
void AnimationChain<T, Clock>::reset(T value) {
    _animation.reset(std::move(value));
    _chain.clear();
}

template <typename T, typename Clock>
template <typename... Args>
void AnimationChain<T, Clock>::_appendTargets(T target, typename Clock::duration duration, Args&&... args) {
    _chain.push_back({std::move(target), std::move(duration), interpolation::Linear<T>});
    _appendTargets(std::forward<Args>(args)...);
}

template <typename T, typename Clock>
template <typename... Args>
void AnimationChain<T, Clock>::_appendTargets(T target, typename Clock::duration duration, Interpolator interpolator, Args&&... args) {
    _chain.push_back({std::move(target), std::move(duration), std::move(interpolator)});
    _appendTargets(std::forward<Args>(args)...);
}

} // namespace okui
