#pragma once

#include "bittorrent/ui/config.h"

namespace bittorrent {
namespace ui {

class Shader;

namespace shapes {

class Arc {
public:
	Arc(double x, double y, double r, double orientation, double angle)
		: x(x), y(y), r(r), orientation(orientation), angle(angle) {}

	void draw(Shader* shader);

	double x, y, r, orientation, angle;
};

}}}