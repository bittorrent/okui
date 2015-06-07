#pragma once

#include "onair/okui/config.h"

#include "onair/okui/Shader.h"
#include "onair/okui/opengl/ShaderProgram.h"
#include "onair/okui/AffineTransformation.h"

namespace onair {
namespace okui {
namespace shaders {

class BoxShadowShader : public Shader {
public:
	BoxShadowShader();

	void setTransformation(const AffineTransformation& transformation) { _transformation = transformation; }

	void setColor(double r, double g, double b, double a);

	void setBox(double x, double y, double w, double h);
	void setSpread(double spread);
	void setInverted(bool inverted = true);

	/**
	* Convenience method.
	*/
	void drawShadow(double x, double y, double w, double h, double spread, bool inverted = false);

	/**
	* @param bezier must be kCurveNone
	*/
	virtual void drawTriangle(double x1, double y1, double x2, double y2, double x3, double y3, Curve curve = kCurveNone) override;
	virtual void flush() override;

private:
	opengl::ShaderProgram _program;
	AffineTransformation _transformation;

	double _boxX, _boxY;

	enum : GLuint {
		kVertexPositionAttribute,
		kVertexColorAttribute,
		kVertexShadowBoxAttribute,
		kVertexShadowParametersAttribute,
	};

	struct Vertex {
		GLfloat x, y;
		GLfloat r{0.0}, g{0.0}, b{0.0}, a{0.1};
		GLfloat boxX, boxY, boxWidth, boxHeight;
		GLfloat spread{0.0}, inverted{0.0};
	};
	
	struct Triangle {
		Vertex a, b, c;
	} _triangle;

	std::vector<Vertex> _vertices;
};

}}}
