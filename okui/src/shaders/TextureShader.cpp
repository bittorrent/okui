#include "onair/okui/shaders/TextureShader.h"

#include "onair/okui/Texture.h"
#include "onair/okui/shapes/Rectangle.h"

namespace onair {
namespace okui {
namespace shaders {

TextureShader::TextureShader(const char* fragmentShader) {
    opengl::Shader vsh(ONAIR_OKUI_VERTEX_SHADER_HEADER R"(
        ATTRIBUTE_IN vec2 positionAttrib;
        ATTRIBUTE_IN vec4 colorAttrib;
        ATTRIBUTE_IN vec4 curveAttrib;
        ATTRIBUTE_IN vec2 textureCoordAttrib;

        VARYING_OUT vec4 color;
        VARYING_OUT vec4 curve;
        VARYING_OUT vec2 textureCoord;

        void main() {
            color = colorAttrib;
            curve = curveAttrib;
            textureCoord = textureCoordAttrib;
            gl_Position = vec4(positionAttrib, 0.0, 1.0);
        }
    )", opengl::Shader::kVertexShader);

    opengl::Shader fsh(fragmentShader ? fragmentShader : ONAIR_OKUI_FRAGMENT_SHADER_HEADER R"(
        VARYING_IN vec4 color;
        VARYING_IN vec4 curve;
        VARYING_IN vec2 textureCoord;

        uniform sampler2D textureSampler;

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
            COLOR_OUT = SAMPLE(textureSampler, textureCoord) * vec4(color.rgb, color.a * alphaMultiplier);
        }
    )", opengl::Shader::kFragmentShader);

    _program.attachShaders(vsh, fsh);
    _program.link();
    _program.use();

    _program.uniform("texture") = 0;

    if (!_program.error().empty()) {
        ONAIR_LOGF_ERROR("error creating shader: %s", _program.error().c_str());
        return;
    }

    auto stride = reinterpret_cast<char*>(&_vertices[1]) - reinterpret_cast<char*>(&_vertices[0]);
    _vertexArrayBuffer.setAttribute(_program.attribute("positionAttrib"), 2, GL_FLOAT, GL_FALSE, stride, offsetof(Vertex, x));
    _vertexArrayBuffer.setAttribute(_program.attribute("colorAttrib"), 4, GL_FLOAT, GL_FALSE, stride, offsetof(Vertex, r));
    _vertexArrayBuffer.setAttribute(_program.attribute("curveAttrib"), 4, GL_FLOAT, GL_FALSE, stride, offsetof(Vertex, cu));
    _vertexArrayBuffer.setAttribute(_program.attribute("textureCoordAttrib"), 2, GL_FLOAT, GL_FALSE, stride, offsetof(Vertex, s));

    ONAIR_OKUI_GL_ERROR_CHECK();
}

void TextureShader::setColor(const Color& color) {
    _triangle.a.r = _triangle.b.r = _triangle.c.r = color.r;
    _triangle.a.g = _triangle.b.g = _triangle.c.g = color.g;
    _triangle.a.b = _triangle.b.b = _triangle.c.b = color.b;
    _triangle.a.a = _triangle.b.a = _triangle.c.a = color.a;
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

void TextureShader::_processTriangle(const std::array<Point<double>, 3>& p, const std::array<Point<double>, 3>& pT, Shader::Curve curve) {
    TriangleCurveProcessor::Process(_triangle, p, curve);

    double s, t;
    _texCoordTransform.transform((pT[0].x - _textureX1) / _textureWidth, (pT[0].y - _textureY1) / _textureHeight, &s, &t);
    _triangle.a.s  = s;
    _triangle.a.t  = t;
    _texCoordTransform.transform((pT[1].x - _textureX1) / _textureWidth, (pT[1].y - _textureY1) / _textureHeight, &s, &t);
    _triangle.b.s  = s;
    _triangle.b.t  = t;
    _texCoordTransform.transform((pT[2].x - _textureX1) / _textureWidth, (pT[2].y - _textureY1) / _textureHeight, &s, &t);
    _triangle.c.s  = s;
    _triangle.c.t  = t;
}

void TextureShader::flush() {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _texture);
    ShaderBase<Vertex>::flush();
}

}}}
