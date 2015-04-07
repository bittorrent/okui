#pragma once

namespace bittorrent {
namespace ui {

template <typename T>
struct Rectangle {
	Rectangle() : x(0), y(0), width(0), height(0) {}
	Rectangle(T x, T y, T width, T height)
		: x(x), y(y), width(width), height(height) {}
	
	T x, y, width, height;
};

}}