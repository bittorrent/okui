#pragma once

#include "onair/okui/config.h"

#include "onair/okui/Animation.h"

#include <deque>

namespace onair {
namespace okui {

template <typename T, typename Clock = std::chrono::steady_clock>
class AnimationChain {
public:
    using Interpolator = typename Animation<T, Clock>::Interpolator;

    AnimationChain(T initial = T{}) : _animation{std::move(initial)} {}

    T current();

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

} } //namespace onair::okui
