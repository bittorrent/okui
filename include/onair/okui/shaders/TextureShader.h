#pragma once

#include "onair/okui/config.h"

#include "onair/okui/Color.h"
#include "onair/okui/Rectangle.h"
#include "onair/okui/Shader.h"

namespace onair {
namespace okui {

class Texture;

namespace shaders {

struct TextureVertex {
    GLfloat x, y;
    GLfloat r{1.0}, g{1.0}, b{1.0}, a{1.0};
    GLfloat cu, cv, cm, caa;
    GLfloat s, t;
};
    
class TextureShader : public ShaderBase<TextureVertex> {
public:
    TextureShader(const char* fragmentShader = nullptr);
    virtual ~TextureShader() {}

    void setColor(const Color& color);

    template <typename ColorArg, typename... RemColorArgs>
    auto setColor(ColorArg&& colorArg, RemColorArgs&&... remColorArgs) -> typename std::enable_if<!std::is_convertible<ColorArg, const Color&>::value>::type {
        setColor(Color(std::forward<ColorArg>(colorArg), std::forward<RemColorArgs>(remColorArgs)...));
    }

    void setTexture(const Texture& texture, Rectangle<double> bounds, const AffineTransformation& texCoordTransform = AffineTransformation{})
        { setTexture(texture, bounds.x, bounds.y, bounds.width, bounds.height, texCoordTransform); }
    void setTexture(const Texture& texture, double x, double y, double w, double h, const AffineTransformation& texCoordTransform = AffineTransformation{});

    /**
    * Draws a texture, scaling it to fill the given area without stretching.
    */
    void drawScaledFill(const Texture& texture, Rectangle<double> area, double r = 0);
    void drawScaledFill(const Texture& texture, double x, double y, double w, double h, double r = 0) { drawScaledFill(texture, Rectangle<double>(x, y, w, h), r); }

    /**
    * Draws a texture, scaling it to fit inside the given area without stretching.
    */
    void drawScaledFit(const Texture& texture, Rectangle<double> area, double r = 0);
    void drawScaledFit(const Texture& texture, double x, double y, double w, double h, double r = 0) { drawScaledFit(texture, Rectangle<double>(x, y, w, h), r); }

    virtual void flush() override;

private:
    AffineTransformation _texCoordTransform;
    
    GLuint _texture{0};
    double _textureX1, _textureY1, _textureWidth, _textureHeight;
    bool _textureHasPremultipliedAlpha{false};

    virtual void _processTriangle(const std::array<Point<double>, 3>& p, const std::array<Point<double>, 3>& pT, Shader::Curve curve) override;
};

}}}
