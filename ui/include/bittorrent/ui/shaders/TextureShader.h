#pragma once

#include "bittorrent/ui/config.h"

#include "bittorrent/ui/Rectangle.h"
#include "bittorrent/ui/Shader.h"
#include "bittorrent/ui/opengl/ShaderProgram.h"
#include "bittorrent/ui/AffineTransformation.h"

namespace bittorrent {
namespace ui {
namespace shaders {

class TextureShader : public Shader {
public:
	TextureShader();

	void setTransformation(const AffineTransformation& transformation) { _transformation = transformation; }

	void setColor(double r, double g, double b, double a);

	void setTexture(GLuint id, Rectangle<double> bounds) { setTexture(id, bounds.x, bounds.y, bounds.width, bounds.height); }
	void setTexture(GLuint id, double x, double y, double w, double h);

	virtual void drawTriangle(double x1, double y1, double x2, double y2, double x3, double y3, Curve curve) override;
	virtual void flush() override;

private:
	opengl::ShaderProgram _program;
	opengl::ShaderProgram::Uniform _curveModeUniform;
	AffineTransformation _transformation;
	
	GLuint _texture = 0;
	double _textureX1, _textureY1, _textureWidth, _textureHeight;

	enum : GLuint {
		kVertexPositionAttribute,
		kVertexColorAttribute,
		kVertexCurveAttribute,
		kVertexTextureCoordinateAttribute,
	};

	struct Vertex {
		GLfloat x, y;
		GLfloat r{1.0}, g{1.0}, b{1.0}, a{1.0};
		GLfloat cu, cv, cm, caa;
		GLfloat s, t;
	};

	struct Triangle {
		Vertex a, b, c;
	} _triangle;

	std::vector<Vertex> _vertices;
};

}}}