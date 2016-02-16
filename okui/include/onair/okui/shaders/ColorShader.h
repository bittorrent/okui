#pragma once

#include "onair/okui/config.h"

#include "onair/okui/Shader.h"

#include "onair/okui/Color.h"

namespace onair {
namespace okui {
namespace shaders {

struct ColorVertex {
    GLfloat x, y;
    GLfloat r{1.0}, g{1.0}, b{1.0}, a{1.0};
    GLfloat cu, cv, cm, caa;
};

class ColorShader : public ShaderBase<ColorVertex> {
public:
    ColorShader();

    /**
    * Sets a flat color.
    */
    void setColor(double r, double g, double b, double a);
    void setColor(const Color& color);

    /**
    * Sets colors for a gradient.
    */
    void setColorA(double x, double y, double r, double g, double b, double a);
    void setColorB(double x, double y, double r, double g, double b, double a);

private:
    struct GradientPoint {
        double x, y;
        double r, g, b, a;
    };

    GradientPoint _gradientPointA, _gradientPointB;

    bool _gradient = false;

    virtual void _processTriangle(const std::array<Point<double>, 3>& p, const std::array<Point<double>, 3>& pT, Shader::Curve curve) override;

    void _calculateGradientColor(double x, double y, GLfloat* r, GLfloat* g, GLfloat* b, GLfloat* a);
    double _calculateGradientPosition(double x, double y);
};

}}}
