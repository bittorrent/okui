#pragma once

#include "bittorrent/ui/config.h"

namespace bittorrent {
namespace ui {

class Shader {
public:
	virtual ~Shader() {}
	
	enum Bezier {
		kBezierNone    = 0,
		kBezierConcave = 1,
		kBezierConvex  = -1,
	};

	/**
	* Draws a triangle. The shader may not actually render the triangle immediately. If you need
	* to render something on top of the triangle with another shader, make a call to flush() to
	* ensure that it's been rendered.
	*/
	virtual void drawTriangle(double x1, double y1, double x2, double y2, double x3, double y3, Bezier bezier = kBezierNone) = 0;

	/**
	* Ensures that all triangles have been rendered.
	*/
	virtual void flush() {}
};

}}