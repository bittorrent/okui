#include "onair/okui/shaders/TextureShader.h"

#include "onair/okui/Texture.h"
#include "onair/okui/shapes/Rectangle.h"

namespace onair {
namespace okui {
namespace shaders {

TextureShader::TextureShader() {
    opengl::Shader vsh(
    #if OPENGL_ES
        "precision highp float;"
    #endif
    R"(
        attribute vec2 positionAttrib;
        attribute vec4 colorAttrib;
        attribute vec4 curveAttrib;
        attribute vec2 textureCoordAttrib;

        varying vec4 color;
        varying vec4 curve;
        varying vec2 textureCoord;

        void main() {
            color = colorAttrib;
            curve = curveAttrib;
            textureCoord = textureCoordAttrib;
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
        varying vec2 textureCoord;

        uniform sampler2D texture;

        void main() {
            float alphaMultiplier = 1.0;
            if (curve.z > 1.5) {
                if (abs(curve.s) >= 0.5 || abs(curve.t) >= 0.5) {
                    discard;
                }
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
            gl_FragColor = texture2D(texture, textureCoord) * vec4(color.rgb, color.a * alphaMultiplier);
        }
    )", opengl::Shader::kFragmentShader);

    _program.attachShaders(vsh, fsh);

    _program.bindAttribute(kVertexPositionAttribute, "positionAttrib");
    _program.bindAttribute(kVertexColorAttribute, "colorAttrib");
    _program.bindAttribute(kVertexCurveAttribute, "curveAttrib");
    _program.bindAttribute(kVertexTextureCoordinateAttribute, "textureCoordAttrib");

    _program.link();
    _program.use();

    _program.uniform("texture") = 0;

    if (!_program.error().empty()) {
        ONAIR_LOG_ERROR("error creating shader: %s", _program.error().c_str());
        return;
    }
}

void TextureShader::setColor(double r, double g, double b, double a) {
    _triangle.a.r = _triangle.b.r = _triangle.c.r = r;
    _triangle.a.g = _triangle.b.g = _triangle.c.g = g;
    _triangle.a.b = _triangle.b.b = _triangle.c.b = b;
    _triangle.a.a = _triangle.b.a = _triangle.c.a = a;
}

void TextureShader::setTexture(GLuint id, double x, double y, double w, double h, const AffineTransformation& texCoordTransform) {
    if (_texture != id) {
        flush();
    }

    _texture = id;

    _transformation.transform(x, y, &_textureX1, &_textureY1);

    double x2, y2;
    _transformation.transform(x + w, y + h, &x2, &y2);

    _textureWidth  = x2 - _textureX1;
    _textureHeight = y2 - _textureY1;

    _texCoordTransform = texCoordTransform;
}

void TextureShader::drawScaledFill(const Texture& texture, Rectangle<double> area, double r) {
    setTexture(texture.id(), area.scaledFill(texture.aspectRatio()), AffineTransformation{0.5, 0.5, -0.5, -0.5, 1.0, 1.0, -r});
    okui::shapes::Rectangle(area).rotate(r).draw(this);
}

void TextureShader::drawScaledFit(const Texture& texture, Rectangle<double> area, double r) {
    setTexture(texture.id(), area.scaledFit(texture.aspectRatio()), AffineTransformation{0.5, 0.5, -0.5, -0.5, 1.0, 1.0, -r});
    okui::shapes::Rectangle(area).rotate(r).draw(this);
}

void TextureShader::drawTriangle(double x1, double y1, double x2, double y2, double x3, double y3, Shader::Curve curve) {
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
    double s, t;
    _texCoordTransform.transform((x1 - _textureX1) / _textureWidth, (y1 - _textureY1) / _textureHeight, &s, &t);
    _triangle.a.s  = s;
    _triangle.a.t  = t;
    _triangle.a.cm = curve;

    _triangle.b.x  = x2;
    _triangle.b.y  = y2;
    _triangle.b.s  = (x2 - _textureX1) / _textureWidth;
    _triangle.b.t  = (y2 - _textureY1) / _textureHeight;
    _texCoordTransform.transform((x2 - _textureX1) / _textureWidth, (y2 - _textureY1) / _textureHeight, &s, &t);
    _triangle.b.s  = s;
    _triangle.b.t  = t;
    _triangle.b.cm = curve;

    _triangle.c.x  = x3;
    _triangle.c.y  = y3;
    _triangle.c.s  = (x3 - _textureX1) / _textureWidth;
    _triangle.c.t  = (y3 - _textureY1) / _textureHeight;
    _texCoordTransform.transform((x3 - _textureX1) / _textureWidth, (y3 - _textureY1) / _textureHeight, &s, &t);
    _triangle.c.s  = s;
    _triangle.c.t  = t;
    _triangle.c.cm = curve;

    _vertices.push_back(_triangle.a);
    _vertices.push_back(_triangle.b);
    _vertices.push_back(_triangle.c);
}

void TextureShader::flush() {
    if (_vertices.empty()) { return; }

    _program.use();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _texture);

    glEnableVertexAttribArray(kVertexPositionAttribute);
    glEnableVertexAttribArray(kVertexColorAttribute);
    glEnableVertexAttribArray(kVertexCurveAttribute);
    glEnableVertexAttribArray(kVertexTextureCoordinateAttribute);

    auto stride = reinterpret_cast<char*>(&_vertices[1]) - reinterpret_cast<char*>(&_vertices[0]);
    glVertexAttribPointer(kVertexPositionAttribute, 2, GL_FLOAT, GL_FALSE, stride, &_vertices[0].x);
    glVertexAttribPointer(kVertexColorAttribute, 4, GL_FLOAT, GL_FALSE, stride, &_vertices[0].r);
    glVertexAttribPointer(kVertexCurveAttribute, 4, GL_FLOAT, GL_FALSE, stride, &_vertices[0].cu);
    glVertexAttribPointer(kVertexTextureCoordinateAttribute, 2, GL_FLOAT, GL_FALSE, stride, &_vertices[0].s);

    glDrawArrays(GL_TRIANGLES, 0, _vertices.size());

    glDisableVertexAttribArray(kVertexTextureCoordinateAttribute);
    glDisableVertexAttribArray(kVertexCurveAttribute);
    glDisableVertexAttribArray(kVertexColorAttribute);
    glDisableVertexAttribArray(kVertexPositionAttribute);

    _vertices.clear();
}

}}}
