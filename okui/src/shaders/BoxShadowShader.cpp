#include "onair/okui/shaders/BoxShadowShader.h"

namespace onair {
namespace okui {
namespace shaders {

BoxShadowShader::BoxShadowShader() {
    opengl::Shader vsh(ONAIR_OKUI_VERTEX_SHADER_HEADER R"(
        ATTRIBUTE_IN vec2 positionAttrib;
        ATTRIBUTE_IN vec4 colorAttrib;
        ATTRIBUTE_IN vec4 shadowBoxAttrib;
        ATTRIBUTE_IN vec2 shadowParametersAttrib;

        VARYING_OUT vec4 color;
        VARYING_OUT vec4 shadowBox;
        VARYING_OUT vec2 shadowParameters;

        void main() {
            color = colorAttrib;
            shadowBox = shadowBoxAttrib;
            shadowParameters = shadowParametersAttrib;
            gl_Position = vec4(positionAttrib, 0.0, 1.0);
        }
    )", opengl::Shader::kVertexShader);

    opengl::Shader fsh(ONAIR_OKUI_FRAGMENT_SHADER_HEADER R"(
        VARYING_IN vec4 color;
        VARYING_IN vec4 shadowBox;
        VARYING_IN vec2 shadowParameters;

        void main() {
            vec2 boxDistance = vec2(0.0);
            if (shadowBox.x < 0.0) {
                boxDistance.x = -shadowBox.x;
            } else if (shadowBox.x > shadowBox.z) {
                boxDistance.x = shadowBox.x - shadowBox.z;
            }
            if (shadowBox.y < 0.0) {
                boxDistance.y = -shadowBox.y;
            } else if (shadowBox.y > shadowBox.w) {
                boxDistance.y = shadowBox.y - shadowBox.w;
            }
            float opacity = 1.0 - min(length(boxDistance) / shadowParameters.s, 1.0);
            if (shadowParameters.t > 0.5) { opacity = 1.0 - opacity; }

            COLOR_OUT = vec4(color.rgb, color.a * opacity * opacity);
        }
    )", opengl::Shader::kFragmentShader);

    _program.attachShaders(vsh, fsh);
    _program.link();

    if (!_program.error().empty()) {
        ONAIR_LOGF_ERROR("error creating shader: %s", _program.error().c_str());
        return;
    }

    auto stride = reinterpret_cast<char*>(&_vertices[1]) - reinterpret_cast<char*>(&_vertices[0]);
    _vertexArrayBuffer.setAttribute(_program.attribute("positionAttrib"), 2, GL_FLOAT, GL_FALSE, stride, offsetof(Vertex, x));
    _vertexArrayBuffer.setAttribute(_program.attribute("colorAttrib"), 4, GL_FLOAT, GL_FALSE, stride, offsetof(Vertex, r));
    _vertexArrayBuffer.setAttribute(_program.attribute("shadowBoxAttrib"), 4, GL_FLOAT, GL_FALSE, stride, offsetof(Vertex, boxX));
    _vertexArrayBuffer.setAttribute(_program.attribute("shadowParametersAttrib"), 2, GL_FLOAT, GL_FALSE, stride, offsetof(Vertex, spread));

    ONAIR_OKUI_GL_ERROR_CHECK();
}

void BoxShadowShader::setColor(const Color& color) {
    _triangle.a.r = _triangle.b.r = _triangle.c.r = color.r;
    _triangle.a.g = _triangle.b.g = _triangle.c.g = color.g;
    _triangle.a.b = _triangle.b.b = _triangle.c.b = color.b;
    _triangle.a.a = _triangle.b.a = _triangle.c.a = color.a;
}

void BoxShadowShader::setBox(double x, double y, double w, double h) {
    _boxX = x;
    _boxY = y;

    _triangle.a.boxWidth = _triangle.b.boxWidth = _triangle.c.boxWidth = w;
    _triangle.a.boxHeight = _triangle.b.boxHeight = _triangle.c.boxHeight = h;
}

void BoxShadowShader::setSpread(double spread) {
    _triangle.a.spread = _triangle.b.spread = _triangle.c.spread = spread;
}

void BoxShadowShader::setInverted(bool inverted) {
    _triangle.a.inverted = _triangle.b.inverted = _triangle.c.inverted = (inverted ? 1.0 : 0.0);
}

void BoxShadowShader::drawShadow(double x, double y, double w, double h, double spread, bool inverted) {
    setBox(x, y, w, h);
    setSpread(spread);
    setInverted(inverted);
    drawTriangle(x - spread, y - spread, x - spread, y + h + spread, x + w + spread, y - spread, kCurveNone);
    drawTriangle(x + w + spread, y + h + spread, x + w + spread, y - spread, x - spread, y + h + spread, kCurveNone);
}

void BoxShadowShader::_processTriangle(const std::array<Point<double>, 3>& p, const std::array<Point<double>, 3>& pT, Shader::Curve curve) {
    assert(curve == kCurveNone);

    _triangle.a.boxX = p[0].x - _boxX;
    _triangle.a.boxY = p[0].y - _boxY;

    _triangle.b.boxX = p[1].x - _boxX;
    _triangle.b.boxY = p[1].y - _boxY;

    _triangle.c.boxX = p[2].x - _boxX;
    _triangle.c.boxY = p[2].y - _boxY;
}

}}}
