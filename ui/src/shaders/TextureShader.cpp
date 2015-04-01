#include "bittorrent/ui/shaders/TextureShader.h"

namespace bittorrent {
namespace ui {
namespace shaders {

TextureShader::TextureShader() {
    opengl::Shader vsh(
    #if OPENGL_ES
        "precision highp float;"
    #endif
    R"(
        attribute vec2 positionAttrib;
        attribute vec4 colorAttrib;
        attribute vec3 bezierAttrib;
        attribute vec2 textureCoordAttrib;
        
        varying vec4 color;
        varying vec3 bezier;
        varying vec2 textureCoord;
        
        void main() {
            color = colorAttrib;
            bezier = bezierAttrib;
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
        varying vec3 bezier;
        varying vec2 textureCoord;
        
        uniform sampler2D texture;
        
        void main() {
            float alphaMultiplier = 1.0;
            if (bezier.z != 0.0) {
                float dist = pow(bezier.s, 2.0) - bezier.t;
                if (dist < 0.0 != bezier.z < 0.0) {
                    float x = abs(dist) / 0.02;
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
    _program.bindAttribute(kVertexBezierAttribute, "bezierAttrib");
    _program.bindAttribute(kVertexTextureCoordinateAttribute, "textureCoordAttrib");

    _program.link();
    _program.use();

    _program.uniform("texture") = 0;
    
    if (!_program.error().empty()) {
        BT_LOG_ERROR("error creating shader: %s", _program.error().c_str());
        return;
    }

    _triangle.a.bu = _triangle.a.bv = 0.0;
    _triangle.b.bu = 0.5;
    _triangle.b.bv = 0.0;
    _triangle.c.bu = _triangle.c.bv = 1.0;
}

void TextureShader::setColor(double r, double g, double b, double a) {
    _triangle.a.r = _triangle.b.r = _triangle.c.r = r;
    _triangle.a.g = _triangle.b.g = _triangle.c.g = g;
    _triangle.a.b = _triangle.b.b = _triangle.c.b = b;
    _triangle.a.a = _triangle.b.a = _triangle.c.a = a;
}

void TextureShader::setTexture(GLuint id, double x, double y, double w, double h) {
    if (_texture != id) {
        flush();
    }
    
    _texture = id;
    
    _transformation.transform(x, y, &_textureX1, &_textureY1);
    
    double x2, y2;
    _transformation.transform(x + w, y + h, &x2, &y2);
    
    _textureWidth  = x2 - _textureX1;
    _textureHeight = y2 - _textureY1;
}

void TextureShader::drawTriangle(double x1, double y1, double x2, double y2, double x3, double y3, Shader::Bezier bezier) {    
    _transformation.transform(x1, y1, &x1, &y1);
    _transformation.transform(x2, y2, &x2, &y2);
    _transformation.transform(x3, y3, &x3, &y3);

    _triangle.a.x  = x1;
    _triangle.a.y  = y1;
    _triangle.a.s  = (x1 - _textureX1) / _textureWidth;
    _triangle.a.t  = (y1 - _textureY1) / _textureHeight;
    _triangle.a.bm = bezier;

    _triangle.b.x  = x2;
    _triangle.b.y  = y2;
    _triangle.b.s  = (x2 - _textureX1) / _textureWidth;
    _triangle.b.t  = (y2 - _textureY1) / _textureHeight;
    _triangle.b.bm = bezier;

    _triangle.c.x  = x3;
    _triangle.c.y  = y3;
    _triangle.c.s  = (x3 - _textureX1) / _textureWidth;
    _triangle.c.t  = (y3 - _textureY1) / _textureHeight;
    _triangle.c.bm = bezier;

    _vertices.push_back(_triangle.a);
    _vertices.push_back(_triangle.b);
    _vertices.push_back(_triangle.c);
}

void TextureShader::flush() {
    if (_vertices.empty()) { return; }
    
    _program.use();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _texture);

    glEnable(GL_BLEND);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE);

    glEnableVertexAttribArray(kVertexPositionAttribute);
    glEnableVertexAttribArray(kVertexColorAttribute);
    glEnableVertexAttribArray(kVertexBezierAttribute);
    glEnableVertexAttribArray(kVertexTextureCoordinateAttribute);

    auto stride = reinterpret_cast<char*>(&_vertices[1]) - reinterpret_cast<char*>(&_vertices[0]);
    glVertexAttribPointer(kVertexPositionAttribute, 2, GL_FLOAT, GL_FALSE, stride, &_vertices[0].x);
    glVertexAttribPointer(kVertexColorAttribute, 4, GL_FLOAT, GL_FALSE, stride, &_vertices[0].r);
    glVertexAttribPointer(kVertexBezierAttribute, 3, GL_FLOAT, GL_FALSE, stride, &_vertices[0].bu);
    glVertexAttribPointer(kVertexTextureCoordinateAttribute, 2, GL_FLOAT, GL_FALSE, stride, &_vertices[0].s);

    glDrawArrays(GL_TRIANGLES, 0, _vertices.size());
    
    glDisableVertexAttribArray(kVertexTextureCoordinateAttribute);
    glDisableVertexAttribArray(kVertexBezierAttribute);
    glDisableVertexAttribArray(kVertexColorAttribute);
    glDisableVertexAttribArray(kVertexPositionAttribute);
    
    _vertices.clear();
}

}}}