/**
* Copyright 2017 BitTorrent Inc.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*    http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/
#pragma once

#include <okui/config.h>

#include <okui/Color.h>
#include <okui/Rectangle.h>
#include <okui/Shader.h>

namespace okui {
class TextureInterface;
}

namespace okui::shaders {

struct TextureVertex {
    GLfloat x, y;
    GLfloat r{1.0}, g{1.0}, b{1.0}, a{1.0};
    GLfloat cu, cv, cm, caa;
    GLfloat s, t;
};

class TextureShader : public ShaderBase<TextureVertex> {
public:
    explicit TextureShader(const char* fragmentShader = nullptr);
    explicit TextureShader(const std::string& fragmentShader) : TextureShader(fragmentShader.c_str()) {}
    virtual ~TextureShader() {}

    void setColor(const Color& color);

    void setTexture(const TextureInterface& texture, Rectangle<double> bounds, const AffineTransformation& texCoordTransform = AffineTransformation{})
        { setTexture(texture, bounds.x, bounds.y, bounds.width, bounds.height, texCoordTransform); }
    void setTexture(const TextureInterface& texture, double x, double y, double w, double h, const AffineTransformation& texCoordTransform = AffineTransformation{});

    /**
    * Draws a texture, scaling it to fill the given area without stretching.
    */
    void drawScaledFill(const TextureInterface& texture, Rectangle<double> area, double r = 0);
    void drawScaledFill(const TextureInterface& texture, double x, double y, double w, double h, double r = 0) { drawScaledFill(texture, Rectangle<double>(x, y, w, h), r); }

    /**
    * Draws a texture, scaling it to fit inside the given area without stretching.
    */
    void drawScaledFit(const TextureInterface& texture, Rectangle<double> area, double r = 0);
    void drawScaledFit(const TextureInterface& texture, double x, double y, double w, double h, double r = 0) { drawScaledFit(texture, Rectangle<double>(x, y, w, h), r); }

    virtual void flush() override;

private:
    AffineTransformation _texCoordTransform;

    GLuint _texture{0};
    double _textureX1, _textureY1, _textureWidth, _textureHeight;
    bool _textureHasPremultipliedAlpha{false};

    virtual void _processTriangle(const std::array<Point<double>, 3>& p, const std::array<Point<double>, 3>& pT, Shader::Curve curve) override;
};

} // namespace okui::shaders
