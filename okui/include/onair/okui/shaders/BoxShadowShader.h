#pragma once

#include "onair/okui/config.h"

#include "onair/okui/Color.h"
#include "onair/okui/Shader.h"

namespace onair {
namespace okui {
namespace shaders {

struct BoxShadowVertex {
	GLfloat x, y;
	GLfloat r{0.0}, g{0.0}, b{0.0}, a{0.1};
	GLfloat boxX, boxY, boxWidth, boxHeight;
	GLfloat spread{0.0}, inverted{0.0};
};
	
class BoxShadowShader : public ShaderBase<BoxShadowVertex> {
public:
	BoxShadowShader();

    void setColor(const Color& color);

    template <typename ColorArg, typename... RemColorArgs>
    auto setColor(ColorArg&& colorArg, RemColorArgs&&... remColorArgs) -> typename std::enable_if<!std::is_convertible<ColorArg, const Color&>::value>::type {
        setColor(Color(std::forward<ColorArg>(colorArg), std::forward<RemColorArgs>(remColorArgs)...));
    }

	void setBox(double x, double y, double w, double h);
	void setSpread(double spread);
	void setInverted(bool inverted = true);

	/**
	* Convenience method.
	*/
	void drawShadow(double x, double y, double w, double h, double spread, bool inverted = false);

private:
	double _boxX, _boxY;

    virtual void _processTriangle(const std::array<Point<double>, 3>& p, const std::array<Point<double>, 3>& pT, Shader::Curve curve) override;
};

}}}
