#pragma once

#include "onair/okui/config.h"

#include "onair/okui/Point.h"

#include "onair/okui/opengl/ShaderProgram.h"
#include "onair/okui/opengl/VertexArrayBuffer.h"
#include "onair/okui/AffineTransformation.h"

namespace onair {
namespace okui {

/**
* Shader interface.
*/
class Shader {
public:
	virtual ~Shader() {}
	
	enum Curve {
		kCurveNone    = 0,
		kCurveBezierConcave  = 1,
		kCurveBezierConvex   = -1,
		kCurveCircularConvex = 2,
	};

	/**
	* Draws a triangle. The shader may not actually render the triangle immediately. If you need
	* to render something on top of the triangle with another shader, make a call to flush() to
	* ensure that it's been rendered.
	*/
	virtual void drawTriangle(double x1, double y1, double x2, double y2, double x3, double y3, Curve curve = kCurveNone) = 0;

	/**
	* Ensures that all triangles have been rendered.
	*/
	virtual void flush() {}
};

/**
* The starting point for all of the provided shaders.
*/
template <typename VertexType>
class ShaderBase : public Shader {
public:
    virtual ~ShaderBase() {}
    
    using Vertex = VertexType;
    
    void setTransformation(const AffineTransformation& transformation) { _transformation = transformation; }

    virtual void drawTriangle(double x1, double y1, double x2, double y2, double x3, double y3, Curve curve) override {
        std::array<Point<double>, 3> p{{Point<double>{x1, y1}, Point<double>{x2, y2}, Point<double>{x3, y3}}};
        std::array<Point<double>, 3> pT;

        _transformation.transform(p[0].x, p[0].y, &pT[0].x, &pT[0].y);
        _transformation.transform(p[1].x, p[1].y, &pT[1].x, &pT[1].y);
        _transformation.transform(p[2].x, p[2].y, &pT[2].x, &pT[2].y);
    
        _triangle.a.x  = pT[0].x;
        _triangle.a.y  = pT[0].y;
        _triangle.b.x  = pT[1].x;
        _triangle.b.y  = pT[1].y;    
        _triangle.c.x  = pT[2].x;
        _triangle.c.y  = pT[2].y;

        _processTriangle(p, pT, curve);

        _vertices.push_back(_triangle.a);
        _vertices.push_back(_triangle.b);
        _vertices.push_back(_triangle.c);
    }

    virtual void flush() override {
        if (_vertices.empty()) { return; }
    
        _program.use();

        _vertexArrayBuffer.bind();
        _vertexArrayBuffer.stream(_vertices.data(), _vertices.size());
        glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(_vertices.size()));
        _vertexArrayBuffer.unbind();
    
        _vertices.clear();
    }

protected:
    opengl::ShaderProgram _program;
    std::vector<Vertex> _vertices;
    opengl::VertexArrayBuffer _vertexArrayBuffer;
    AffineTransformation _transformation;

    struct Triangle {
        Vertex a, b, c;
    } _triangle;

    struct TriangleCurveProcessor {
        template <typename Triangle>
        static void Process(Triangle& triangle, const std::array<Point<double>, 3>& p, Shader::Curve curve) {
            if (curve) {
                if (curve == kCurveCircularConvex) {
                    auto dx = (p[0].x - p[1].x);
                    auto dy = (p[0].y - p[1].y);
                    auto scale = 1.0 / sqrt(dx * dx + dy * dy);
                    
                    triangle.a.cu = dx * scale;
                    triangle.b.cu = 0.0;
                    triangle.c.cu = (p[2].x - p[1].x) * scale;
            
                    triangle.a.cv = dy * scale;
                    triangle.b.cv = 0.0;
                    triangle.c.cv = (p[2].y - p[1].y) * scale;
                    
                    triangle.a.caa = triangle.b.caa = triangle.c.caa = scale;
                } else if (curve == kCurveBezierConcave || curve == kCurveBezierConvex) {
                    triangle.a.cu = triangle.a.cv = 0.0;
                    triangle.b.cu = 0.5;
                    triangle.b.cv = 0.0;
                    triangle.c.cu = triangle.c.cv = 1.0;
        
                    auto aa = 4.0 / (p[0].x * (p[1].y - p[2].y) + p[1].x * (p[2].y - p[0].y) + p[2].x * (p[0].y - p[1].y));
                    triangle.a.caa = triangle.b.caa = triangle.c.caa = aa;
                }
            }
        
            triangle.a.cm = curve;
            triangle.b.cm = curve;
            triangle.c.cm = curve;
        }
    };

    /**
    * Override this to finalize the triangle before drawing.
    */
    virtual void _processTriangle(const std::array<Point<double>, 3>& p, const std::array<Point<double>, 3>& pT, Shader::Curve curve) {}
};

}}
