#pragma once

#include <chrono>
#include <list>

#include <cmath>

namespace bittorrent {
namespace ui {

namespace Interpolation {
	using Function = double(*)(double);
	
	static constexpr double Linear(double p) { return p; }
	static double EaseInExpo(double p) { return pow(2, 10.0 * (p - 1.0)); }
	static double EaseOutExpo(double p) { return 1.0 - pow(2, -10.0 * p); }
}

template <typename T, typename Clock = std::chrono::steady_clock>
class Animation {
public:
	Animation() : Animation(T()) {}
	Animation(const T& initial) : _initial(initial), _target(initial) {}

	T current() const {
		auto now = Clock::now();

		if (now >= _end) { return _target; }

		double progress = std::chrono::duration<double>(now - _start).count() / std::chrono::duration<double>(_end - _start).count();
		progress = (*_interpolation)(progress);
		return _initial * (1.0 - progress) + _target * progress;
	}

	void target(const T& target, typename Clock::duration duration, Interpolation::Function interpolation = Interpolation::Linear) {
		_initial = current();
		_target = target;
		_interpolation = interpolation;
		_start = Clock::now();
		_end = _start + duration;
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
	Interpolation::Function _interpolation = nullptr;
	
	typename Clock::time_point _start;
	typename Clock::time_point _end;
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
	void target(const T& target, typename Clock::duration duration, Args&&... args) {
		_chain.clear();
		_animation.target(target, duration);
		_appendTargets(std::forward<Args>(args)...);
	}

	template <typename... Args>
	void target(const T& target, typename Clock::duration duration, Interpolation::Function interpolation, Args&&... args) {
		_chain.clear();
		_animation.target(target, duration, interpolation);
		_appendTargets(std::forward<Args>(args)...);
	}
	
	void reset(const T& value) {
		_chain.clear();
		_animation.reset(value);
	}
	
private:
	Animation<T, Clock> _animation;
	std::list<std::tuple<T, typename Clock::duration, Interpolation::Function>> _chain;

	void _appendTargets() {}

	template <typename... Args>
	void _appendTargets(const T& target, typename Clock::duration duration, Args&&... args) {
		_chain.emplace_back(target, duration, Interpolation::Linear);
		_appendTargets(std::forward<Args>(args)...);
	}

	template <typename... Args>
	void _appendTargets(const T& target, typename Clock::duration duration, Interpolation::Function interpolation, Args&&... args) {
		_chain.emplace_back(target, duration, interpolation);
		_appendTargets(std::forward<Args>(args)...);
	}
};

}}