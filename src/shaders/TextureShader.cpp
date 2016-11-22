#include <okui/shaders/TextureShader.h>

#include <okui/TextureInterface.h>
#include <okui/shapes/Rectangle.h>

namespace okui {
namespace shaders {

TextureShader::TextureShader(const char* fragmentShader) {
    opengl::Shader vsh(scraps::opengl::CommonVertexShaderHeader() + R"(
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

    opengl::Shader fsh(fragmentShader ? std::string(fragmentShader) : CommonOKUIFragmentShaderHeader() + R"(
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

            vec4 sample = unmultipliedInput(SAMPLE(textureSampler, textureCoord));
            COLOR_OUT = multipliedOutput(vec4(sample.rgb * color.rgb, sample.a * color.a * alphaMultiplier));
        }
    )", opengl::Shader::kFragmentShader);

    enum : GLuint {
        kPositionAttrib,
        kColorAttrib,
        kCurveAttrib,
        kTextureCoordAttrib,
    };

    _program.attachShaders(vsh, fsh);
    _program.bindAttribute(kPositionAttrib, "positionAttrib");
    _program.bindAttribute(kColorAttrib, "colorAttrib");
    _program.bindAttribute(kCurveAttrib, "curveAttrib");
    _program.bindAttribute(kTextureCoordAttrib, "textureCoordAttrib");

    _program.link();
    _program.use();

    _program.uniform("texture") = 0;
    _blendingFlagsUniform = _program.uniform("blendingFlags");

    if (!_program.error().empty()) {
        SCRAPS_LOGF_ERROR("error creating shader: %s", _program.error().c_str());
        return;
    }

    auto stride = reinterpret_cast<char*>(&_vertices[1]) - reinterpret_cast<char*>(&_vertices[0]);
    _vertexArrayBuffer.setAttribute(kPositionAttrib, 2, GL_FLOAT, GL_FALSE, stride, offsetof(Vertex, x));
    _vertexArrayBuffer.setAttribute(kColorAttrib, 4, GL_FLOAT, GL_FALSE, stride, offsetof(Vertex, r));
    _vertexArrayBuffer.setAttribute(kCurveAttrib, 4, GL_FLOAT, GL_FALSE, stride, offsetof(Vertex, cu));
    _vertexArrayBuffer.setAttribute(kTextureCoordAttrib, 2, GL_FLOAT, GL_FALSE, stride, offsetof(Vertex, s));

    SCRAPS_GL_ERROR_CHECK();
}

void TextureShader::setColor(const Color& color) {
    _triangle.a.r = _triangle.b.r = _triangle.c.r = color.redF();
    _triangle.a.g = _triangle.b.g = _triangle.c.g = color.greenF();
    _triangle.a.b = _triangle.b.b = _triangle.c.b = color.blueF();
    _triangle.a.a = _triangle.b.a = _triangle.c.a = color.alphaF();
}

void TextureShader::setTexture(const TextureInterface& texture, double x, double y, double w, double h, const AffineTransformation& texCoordTransform) {
    if (_texture != texture.id()) {
        flush();
    }

    _texture = texture.id();
    _textureHasPremultipliedAlpha = texture.hasPremultipliedAlpha();

    _transformation.transform(x, y, &_textureX1, &_textureY1);

    double x2, y2;
    _transformation.transform(x + w, y + h, &x2, &y2);

    _textureWidth  = (x2 - _textureX1) * static_cast<double>(texture.allocatedWidth()) / texture.width();
    _textureHeight = (y2 - _textureY1) * static_cast<double>(texture.allocatedHeight()) / texture.height();

    _texCoordTransform = texCoordTransform;
}

void TextureShader::drawScaledFill(const TextureInterface& texture, Rectangle<double> area, double r) {
    setTexture(texture, area.scaledFill(texture.aspectRatio()), AffineTransformation{0.5, 0.5, -0.5, -0.5, 1.0, 1.0, -r});
    okui::shapes::Rectangle(area).rotate(r).draw(this);
}

void TextureShader::drawScaledFit(const TextureInterface& texture, Rectangle<double> area, double r) {
    auto fit = area.scaledFit(texture.aspectRatio());
    setTexture(texture, fit, AffineTransformation{0.5, 0.5, -0.5, -0.5, 1.0, 1.0, -r});
    okui::shapes::Rectangle(fit).rotate(r).draw(this);
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
    _program.use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _texture);
    ShaderBase<Vertex>::_flush(_textureHasPremultipliedAlpha);
}

} } // namespace okui::shaders
