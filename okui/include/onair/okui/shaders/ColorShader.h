#pragma once

#include "onair/okui/config.h"

#include "onair/okui/Shader.h"
#include "onair/okui/opengl/ShaderProgram.h"
#include "onair/okui/AffineTransformation.h"

namespace onair {
namespace okui {
namespace shaders {

class ColorShader : public Shader {
public:
	ColorShader();

	void setTransformation(const AffineTransformation& transformation) { _transformation = transformation; }

	/**
	* Sets a flat color.
	*/
	void setColor(double r, double g, double b, double a);

	/**
	* Sets colors for a gradient.
	*/
	void setColorA(double x, double y, double r, double g, double b, double a);
	void setColorB(double x, double y, double r, double g, double b, double a);

	virtual void drawTriangle(double x1, double y1, double x2, double y2, double x3, double y3, Curve curve) override;
	virtual void flush() override;

private:
	opengl::ShaderProgram _program;
	AffineTransformation _transformation;

	struct Point {
		double x, y;
		double r, g, b, a;
	};
	
	Point _pointA, _pointB;
	
	bool _gradient = false;
	
	void _calculateGradientColor(double x, double y, GLfloat* r, GLfloat* g, GLfloat* b, GLfloat* a);
	double _calculateGradientPosition(double x, double y);

	enum : GLuint {
		kVertexPositionAttribute,
		kVertexColorAttribute,
		kVertexCurveAttribute,
	};

	struct Vertex {
		GLfloat x, y;
		GLfloat r{1.0}, g{1.0}, b{1.0}, a{1.0};
		GLfloat cu, cv, cm, caa;
	};
	
	struct Triangle {
		Vertex a, b, c;
	} _triangle;

	std::vector<Vertex> _vertices;
};

}}}
