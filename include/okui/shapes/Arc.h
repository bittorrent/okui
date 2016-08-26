#pragma once

#include "okui/config.h"

namespace okui {

class Shader;

namespace shapes {

class Arc {
public:
	Arc(double x, double y, double r, double orientation, double angle)
		: x(x), y(y), r(r), orientation(orientation), angle(angle) {}

	void draw(Shader* shader);

	double x, y, r, orientation, angle;
};

} } // namespace okui::shapes
