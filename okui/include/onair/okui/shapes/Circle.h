#pragma once

#include "onair/okui/config.h"

#include "onair/okui/shapes/Arc.h"

namespace onair {
namespace okui {

class Shader;

namespace shapes {

class Circle {
public:
	Circle(double x, double y, double r)
		: x(x), y(y), r(r) {}

	void draw(Shader* shader) {
		Arc(x, y, r, 0.0, 2.0 * M_PI).draw(shader);
	}

	double x, y, r;
};

}}}
