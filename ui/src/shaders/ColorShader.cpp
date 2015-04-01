#include "bittorrent/ui/shaders/ColorShader.h"

namespace bittorrent {
namespace ui {
namespace shaders {

ColorShader::ColorShader() {
    opengl::Shader vsh(
    #if OPENGL_ES
        "precision highp float;"
    #endif
    R"(
        attribute vec2 positionAttrib;
        attribute vec4 colorAttrib;
        attribute vec3 bezierAttrib;

        varying vec4 color;
        varying vec3 bezier;

        void main() {
            color = colorAttrib;
            bezier = bezierAttrib;
            gl_Position = vec4(positionAttrib, 0.0, 1.0);
        }
    )", opengl::Shader::kVertexShader);

    opengl::Shader fsh(
#if OPENGL_ES
        "precision highp float;"
#endif
    R"(
        varying vec4 color;
        varying vec3 bezier;
        
        void main() {
            float alphaMultiplier = 1.0;
            if (bezier.z != 0.0) {
                float dist = pow(bezier.s, 2.0) - bezier.t;
                if (dist < 0.0 != bezier.z < 0.0) {
                    float x = abs(dist) / 0.02;
                    if (x < 1.0) {
                        alphaMultiplier = (1.0 - x);
                    } else {
                        discard;
                    }
                }
            }
            gl_FragColor = vec4(color.rgb, color.a * alphaMultiplier);
        }
    )", opengl::Shader::kFragmentShader);
    
    _program.attachShaders(vsh, fsh);
    
    _program.bindAttribute(kVertexPositionAttribute, "positionAttrib");
    _program.bindAttribute(kVertexColorAttribute, "colorAttrib");
    _program.bindAttribute(kVertexBezierAttribute, "bezierAttrib");

    _program.link();
    
    if (!_program.error().empty()) {
        BT_LOG_ERROR("error creating shader: %s", _program.error().c_str());
        return;
    }
    
    _triangle.a.bu = _triangle.a.bv = 0.0;
    _triangle.b.bu = 0.5;
    _triangle.b.bv = 0.0;
    _triangle.c.bu = _triangle.c.bv = 1.0;
}

void ColorShader::setColor(double r, double g, double b, double a) {
    _triangle.a.r = _triangle.b.r = _triangle.c.r = r;
    _triangle.a.g = _triangle.b.g = _triangle.c.g = g;
    _triangle.a.b = _triangle.b.b = _triangle.c.b = b;
    _triangle.a.a = _triangle.b.a = _triangle.c.a = a;
    _gradient = false;
}

void ColorShader::setColorA(double x, double y, double r, double g, double b, double a) {
    _transformation.transform(x, y, &_pointA.x, &_pointA.y);
    _pointA.r = r; _pointA.g = g; _pointA.b = b; _pointA.a = a;
    _gradient = true;
}

void ColorShader::setColorB(double x, double y, double r, double g, double b, double a) {
    _transformation.transform(x, y, &_pointB.x, &_pointB.y);
    _pointB.r = r; _pointB.g = g; _pointB.b = b; _pointB.a = a;
    _gradient = true;
}

void ColorShader::drawTriangle(double x1, double y1, double x2, double y2, double x3, double y3, Shader::Bezier bezier) {
    _transformation.transform(x1, y1, &x1, &y1);
    _transformation.transform(x2, y2, &x2, &y2);
    _transformation.transform(x3, y3, &x3, &y3);

    _triangle.a.x  = x1;
    _triangle.a.y  = y1;
    _triangle.a.bm = bezier;

    _triangle.b.x = x2;
    _triangle.b.y = y2;
    _triangle.b.bm = bezier;

    _triangle.c.x = x3;
    _triangle.c.y = y3;
    _triangle.c.bm = bezier;
    
    if (_gradient) {
        _calculateGradientColor(x1, y1, &_triangle.a.r, &_triangle.a.g, &_triangle.a.b, &_triangle.a.a);
        _calculateGradientColor(x2, y2, &_triangle.b.r, &_triangle.b.g, &_triangle.b.b, &_triangle.b.a);
        _calculateGradientColor(x3, y3, &_triangle.c.r, &_triangle.c.g, &_triangle.c.b, &_triangle.c.a);
    }

    _vertices.push_back(_triangle.a);
    _vertices.push_back(_triangle.b);
    _vertices.push_back(_triangle.c);
}

void ColorShader::flush() {
    if (_vertices.empty()) { return; }
    
    _program.use();

    glEnable(GL_BLEND);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE);

    glEnableVertexAttribArray(kVertexPositionAttribute);
    glEnableVertexAttribArray(kVertexColorAttribute);
    glEnableVertexAttribArray(kVertexBezierAttribute);

    auto stride = reinterpret_cast<char*>(&_vertices[1]) - reinterpret_cast<char*>(&_vertices[0]);
    glVertexAttribPointer(kVertexPositionAttribute, 2, GL_FLOAT, GL_FALSE, stride, &_vertices[0].x);
    glVertexAttribPointer(kVertexColorAttribute, 4, GL_FLOAT, GL_FALSE, stride, &_vertices[0].r);
    glVertexAttribPointer(kVertexBezierAttribute, 3, GL_FLOAT, GL_FALSE, stride, &_vertices[0].bu);

    glDrawArrays(GL_TRIANGLES, 0, _vertices.size());
    
    glDisableVertexAttribArray(kVertexBezierAttribute);
    glDisableVertexAttribArray(kVertexColorAttribute);
    glDisableVertexAttribArray(kVertexPositionAttribute);
    
    _vertices.clear();
}

void ColorShader::_calculateGradientColor(double x, double y, GLfloat* r, GLfloat* g, GLfloat* b, GLfloat* a) {
    auto bWeight = std::min(std::max(_calculateGradientPosition(x, y), 0.0), 1.0);
    auto aWeight = 1.0 - bWeight;

    *r = _pointA.r * aWeight + _pointB.r * bWeight;
    *g = _pointA.g * aWeight + _pointB.g * bWeight;
    *b = _pointA.b * aWeight + _pointB.b * bWeight;
    *a = _pointA.a * aWeight + _pointB.a * bWeight;
}

double ColorShader::_calculateGradientPosition(double x, double y) {
    // if the points are on the same x or y coordinate, things are easy

    if (_pointA.y == _pointB.y) {
        return (x - _pointA.x) / (_pointB.x - _pointA.x);
    } else if (_pointA.x == _pointB.x) {
        return (y - _pointA.y) / (_pointB.y - _pointA.y);
    }
    
    // otherwise we need to calculate the x coordinate of the intersection of the pointA-pointB line 
    // and the perpendicular that passes through (x, y)
    
    auto gradientSlope = (_pointB.y - _pointA.y) / (_pointB.x - _pointA.x);
    auto perpendicularSlope = -1.0 / gradientSlope;

    auto positionX = (_pointA.y - y - gradientSlope * _pointA.x + perpendicularSlope * x) / (perpendicularSlope - gradientSlope);

    return (positionX - _pointA.x) / (_pointB.x - _pointA.x);
}

}}}