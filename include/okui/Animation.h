#pragma once

#include <okui/config.h>

#include <okui/interpolation.h>

#include <chrono>

namespace okui {

template <typename T, typename Clock = std::chrono::steady_clock>
class Animation {
public:
    using Interpolator = T(*)(const T&, const T&, double);

    Animation(T initial = T{});

    void target(T target, typename Clock::duration duration, Interpolator interpolator = interpolation::Linear<T>);
    void reset(T value);

    T current() const;

    const T& initial() const                         { return _initial; }
    const T& target() const                          { return _target; }
    const typename Clock::time_point& start() const  { return _start; }
    typename Clock::time_point end() const           { return _start + _duration; }
    const typename Clock::duration& duration() const { return _duration; }

private:
    T                          _initial;
    T                          _target;
    typename Clock::time_point _start;
    typename Clock::duration   _duration = Clock::duration::zero();
    Interpolator               _interpolator = nullptr;
};

template<typename T, typename Clock>
Animation<T, Clock>::Animation(T initial)
    : _initial{std::move(initial)}
    , _target{_initial}
{}

template<typename T, typename Clock>
void Animation<T, Clock>::target(T target, typename Clock::duration duration, Interpolator interpolator) {
    _initial      = current();
    _target       = std::move(target);
    _start        = Clock::now();
    _duration     = std::move(duration);
    _interpolator = std::move(interpolator);
}

template<typename T, typename Clock>
void Animation<T, Clock>::reset(T value) {
    _initial      = std::move(value);
    _target       = _initial;
    _start        = {};
    _duration     = Clock::duration::zero();
    _interpolator = nullptr;
}

template<typename T, typename Clock>
T Animation<T, Clock>::current() const {
    if (!_interpolator) {
        return _target;
    }

    auto elapsed = Clock::now() - _start;

    if (elapsed >= _duration) {
        return _target;
    }

    auto t = std::chrono::duration<double>(elapsed).count(),
         d = std::chrono::duration<double>(_duration).count();

    return _interpolator(_initial, _target-_initial, t/d);
}

} // namespace okui
