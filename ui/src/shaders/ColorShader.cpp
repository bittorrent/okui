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
        attribute vec4 curveAttrib;

        varying vec4 color;
        varying vec4 curve;

        void main() {
            color = colorAttrib;
            curve = curveAttrib;
            gl_Position = vec4(positionAttrib, 0.0, 1.0);
        }
    )", opengl::Shader::kVertexShader);

    opengl::Shader fsh(
#if OPENGL_ES
        "precision highp float;"
#endif
    R"(
        varying vec4 color;
        varying vec4 curve;
        
        void main() {
            vec4 c = color;
            float alphaMultiplier = 1.0;
            if (curve.z > 1.5) {
                float dist = sqrt(curve.s * curve.s + curve.t * curve.t);
                float aa = curve.w;
                if (dist > 0.5 + 0.5 * aa) {
                    discard;
                } else if (dist > 0.5 - 0.5 * aa) {
                    alphaMultiplier = 1.0 - (dist - (0.5 - 0.5 * aa)) / aa;
                }
            } else if (curve.z != 0.0) {
                float dist = pow(curve.s, 2.0) - curve.t;
                float aa = curve.w;
                dist -= curve.z * aa;
                if (dist < 0.0 != curve.z < 0.0) {
                    float x = abs(dist) / (2.0 * aa);
                    if (x < 1.0) {
                        alphaMultiplier = (1.0 - x);
                    } else {
                        discard;
                    }
                }
            }
            gl_FragColor = vec4(c.rgb, color.a * alphaMultiplier);
        }
    )", opengl::Shader::kFragmentShader);
    
    _program.attachShaders(vsh, fsh);
    
    _program.bindAttribute(kVertexPositionAttribute, "positionAttrib");
    _program.bindAttribute(kVertexColorAttribute, "colorAttrib");
    _program.bindAttribute(kVertexCurveAttribute, "curveAttrib");

    _program.link();
    
    if (!_program.error().empty()) {
        BT_LOG_ERROR("error creating shader: %s", _program.error().c_str());
        return;
    }
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

void ColorShader::drawTriangle(double x1, double y1, double x2, double y2, double x3, double y3, Shader::Curve curve) {
    if (curve) {
        if (curve == kCurveCircularConvex) {
            auto dx = (x1 - x2);
            auto dy = (y1 - y2);
            auto scale = 1.0 / sqrt(dx * dx + dy * dy);
            
            _triangle.a.cu = dx * scale;
            _triangle.b.cu = 0.0;
            _triangle.c.cu = (x3 - x2) * scale;
    
            _triangle.a.cv = dy * scale;
            _triangle.b.cv = 0.0;
            _triangle.c.cv = (y3 - y2) * scale;
            
            _triangle.a.caa = _triangle.b.caa = _triangle.c.caa = scale;
        } else if (curve == kCurveBezierConcave || curve == kCurveBezierConvex) {
            _triangle.a.cu = _triangle.a.cv = 0.0;
            _triangle.b.cu = 0.5;
            _triangle.b.cv = 0.0;
            _triangle.c.cu = _triangle.c.cv = 1.0;

            auto aa = 4.0 / (x1 * (y2 - y3) + x2 * (y3 - y1) + x3 * (y1 - y2));
            _triangle.a.caa = _triangle.b.caa = _triangle.c.caa = aa;
        }
    }

    _transformation.transform(x1, y1, &x1, &y1);
    _transformation.transform(x2, y2, &x2, &y2);
    _transformation.transform(x3, y3, &x3, &y3);

    _triangle.a.x  = x1;
    _triangle.a.y  = y1;
    _triangle.a.cm = curve;

    _triangle.b.x  = x2;
    _triangle.b.y  = y2;
    _triangle.b.cm = curve;

    _triangle.c.x  = x3;
    _triangle.c.y  = y3;
    _triangle.c.cm = curve;

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
    glEnableVertexAttribArray(kVertexCurveAttribute);

    auto stride = reinterpret_cast<char*>(&_vertices[1]) - reinterpret_cast<char*>(&_vertices[0]);
    glVertexAttribPointer(kVertexPositionAttribute, 2, GL_FLOAT, GL_FALSE, stride, &_vertices[0].x);
    glVertexAttribPointer(kVertexColorAttribute, 4, GL_FLOAT, GL_FALSE, stride, &_vertices[0].r);
    glVertexAttribPointer(kVertexCurveAttribute, 4, GL_FLOAT, GL_FALSE, stride, &_vertices[0].cu);

    glDrawArrays(GL_TRIANGLES, 0, _vertices.size());
    
    glDisableVertexAttribArray(kVertexCurveAttribute);
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