#pragma once

#include "onair/okui/config.h"

#include "onair/okui/interpolation.h"

#include <chrono>
#include <list>

namespace onair {
namespace okui {

template <typename T, typename Clock = std::chrono::steady_clock>
class Animation {
public:
    using Interpolator = T(*)(double, const T&, const T&, double);

    Animation() : Animation(T()) {}
    Animation(const T& initial) : _initial(initial), _target(initial) {}

    T current() const {
        auto now = Clock::now();

        if (now >= _end) { return _target; }

        auto t = std::chrono::duration<double>(now - _start).count(),
             d = std::chrono::duration<double>(_end - _start).count();

        return _interpolator(t, _initial, _target-_initial, d);
    }

    Animation& target(const T& target, typename Clock::duration duration, Interpolator interpolator = interpolation::Linear<T>) {
        _initial = current();
        _target = target;
        _interpolator = interpolator;
        _start = Clock::now();
        _end = _start + duration;
        return *this;
    }

    T target() const { return _target; }

    void reset(const T& value) {
        _initial = value;
        _target = value;
        _start = Clock::time_point::min();
        _end = Clock::time_point::min();
    }

    typename Clock::time_point end() const { return _end; }

private:
    T _initial;
    T _target;
    Interpolator _interpolator = nullptr;

    typename Clock::time_point _start = Clock::time_point::min();
    typename Clock::time_point _end = Clock::time_point::min();
};

template <typename T, typename Clock = std::chrono::steady_clock>
class AnimationChain {
public:
    AnimationChain() : AnimationChain(T()) {}
    AnimationChain(const T& initial) : _animation(initial) {}

    T current() {
        auto now = Clock::now();
        while (_animation.end() <= now && !_chain.empty()) {
            _animation.target(std::get<0>(_chain.front()), std::get<1>(_chain.front()), std::get<2>(_chain.front()));
            _chain.pop_front();
        }
        return _animation.current();
    }

    template <typename... Args>
    AnimationChain& target(const T& target, typename Clock::duration duration, Args&&... args) {
        _chain.clear();
        _animation.target(target, duration);
        _appendTargets(std::forward<Args>(args)...);
        return *this;
    }

    template <typename... Args>
    AnimationChain& target(const T& target, typename Clock::duration duration, typename Animation<T, Clock>::Interpolator interpolator, Args&&... args) {
        _chain.clear();
        _animation.target(target, duration, interpolator);
        _appendTargets(std::forward<Args>(args)...);
        return *this;
    }

    void reset(const T& value) {
        _chain.clear();
        _animation.reset(value);
    }

private:
    Animation<T, Clock> _animation;
    std::list<std::tuple<T, typename Clock::duration, typename Animation<T, Clock>::Interpolator>> _chain;

    void _appendTargets() {}

    template <typename... Args>
    void _appendTargets(const T& target, typename Clock::duration duration, Args&&... args) {
        _chain.emplace_back(target, duration, interpolation::Linear<T>);
        _appendTargets(std::forward<Args>(args)...);
    }

    template <typename... Args>
    void _appendTargets(const T& target, typename Clock::duration duration, typename Animation<T, Clock>::Interpolator interpolator, Args&&... args) {
        _chain.emplace_back(target, duration, interpolator);
        _appendTargets(std::forward<Args>(args)...);
    }
};

}}
