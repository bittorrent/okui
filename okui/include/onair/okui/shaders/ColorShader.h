#pragma once

#include "onair/okui/config.h"

#include "onair/okui/Color.h"
#include "onair/okui/Shader.h"

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
    void setColor(const Color& color);

    template <typename ColorArg, typename... RemColorArgs>
    auto setColor(ColorArg&& colorArg, RemColorArgs&&... remColorArgs) -> typename std::enable_if<!std::is_convertible<ColorArg, const Color&>::value>::type {
        setColor(Color(std::forward<ColorArg>(colorArg), std::forward<RemColorArgs>(remColorArgs)...));
    }

    /**
    * Sets colors for a gradient.
    */
    void setColorA(double x, double y, const Color& color);
    void setColorB(double x, double y, const Color& color);

    template <typename ColorArg, typename... RemColorArgs>
    auto setColorA(double x, double y, ColorArg&& colorArg, RemColorArgs&&... remColorArgs) -> typename std::enable_if<!std::is_convertible<ColorArg, const Color&>::value>::type {
        setColorA(x, y, Color(std::forward<ColorArg>(colorArg), std::forward<RemColorArgs>(remColorArgs)...));
    }

    template <typename ColorArg, typename... RemColorArgs>
    auto setColorB(double x, double y, ColorArg&& colorArg, RemColorArgs&&... remColorArgs) -> typename std::enable_if<!std::is_convertible<ColorArg, const Color&>::value>::type {
        setColorB(x, y, Color(std::forward<ColorArg>(colorArg), std::forward<RemColorArgs>(remColorArgs)...));
    }

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
