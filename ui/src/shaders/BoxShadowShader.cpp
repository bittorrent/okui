#include "bittorrent/ui/shaders/BoxShadowShader.h"

namespace bittorrent {
namespace ui {
namespace shaders {

BoxShadowShader::BoxShadowShader() {
    opengl::Shader vsh(
    #if OPENGL_ES
        "precision highp float;"
    #endif
    R"(
        attribute vec2 positionAttrib;
        attribute vec4 colorAttrib;
        attribute vec3 bezierAttrib;
        attribute vec4 shadowBoxAttrib;
        attribute vec2 shadowParametersAttrib;
        
        varying vec4 color;
        varying vec3 bezier;
        varying vec4 shadowBox;
        varying vec2 shadowParameters;
        
        void main() {
            color = colorAttrib;
            bezier = bezierAttrib;
            shadowBox = shadowBoxAttrib;
            shadowParameters = shadowParametersAttrib;
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
        varying vec4 shadowBox;
        varying vec2 shadowParameters;
        
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
            
            gl_FragColor = vec4(color.rgb, color.a * alphaMultiplier * opacity * opacity);
        }
    )", opengl::Shader::kFragmentShader);
    
    _program.attachShaders(vsh, fsh);
    
    _program.bindAttribute(kVertexPositionAttribute, "positionAttrib");
    _program.bindAttribute(kVertexColorAttribute, "colorAttrib");
    _program.bindAttribute(kVertexBezierAttribute, "bezierAttrib");
    _program.bindAttribute(kVertexShadowBoxAttribute, "shadowBoxAttrib");
    _program.bindAttribute(kVertexShadowParametersAttribute, "shadowParametersAttrib");

    _program.link();
    
    if (!_program.error().empty()) {
        BT_LOG_ERROR("error creating shader: %s", _program.error().c_str());
        return;
    }
    
    _triangle.a.bu = _triangle.a.bv = 0.0;
    _triangle.b.bu = 0.5;
    _triangle.b.bv = 0.0;
    _triangle.c.bu = _triangle.c.bv = 1.0;
}

void BoxShadowShader::setColor(double r, double g, double b, double a) {
    _triangle.a.r = _triangle.b.r = _triangle.c.r = r;
    _triangle.a.g = _triangle.b.g = _triangle.c.g = g;
    _triangle.a.b = _triangle.b.b = _triangle.c.b = b;
    _triangle.a.a = _triangle.b.a = _triangle.c.a = a;
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
    drawTriangle(x - spread, y - spread, x - spread, y + h + spread, x + w + spread, y - spread, kBezierNone);
    drawTriangle(x + w + spread, y + h + spread, x + w + spread, y - spread, x - spread, y + h + spread, kBezierNone);
}

void BoxShadowShader::drawTriangle(double x1, double y1, double x2, double y2, double x3, double y3, Shader::Bezier bezier) {
    _triangle.a.boxX = x1 - _boxX;
    _triangle.a.boxY = y1 - _boxY;

    _triangle.b.boxX = x2 - _boxX;
    _triangle.b.boxY = y2 - _boxY;

    _triangle.c.boxX = x3 - _boxX;
    _triangle.c.boxY = y3 - _boxY;

    _transformation.transform(x1, y1, &x1, &y1);
    _transformation.transform(x2, y2, &x2, &y2);
    _transformation.transform(x3, y3, &x3, &y3);

    _triangle.a.x  = x1;
    _triangle.a.y  = y1;
    _triangle.a.bm = bezier;

    _triangle.b.x = x2;
    _triangle.b.y = y2;
    _triangle.b.bm = bezier;

    _triangle.c.x = x3;
    _triangle.c.y = y3;
    _triangle.c.bm = bezier;

    _vertices.push_back(_triangle.a);
    _vertices.push_back(_triangle.b);
    _vertices.push_back(_triangle.c);
}

void BoxShadowShader::flush() {
    if (_vertices.empty()) { return; }
    
    _program.use();

    glEnable(GL_BLEND);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE);

    glEnableVertexAttribArray(kVertexPositionAttribute);
    glEnableVertexAttribArray(kVertexColorAttribute);
    glEnableVertexAttribArray(kVertexBezierAttribute);
    glEnableVertexAttribArray(kVertexShadowBoxAttribute);
    glEnableVertexAttribArray(kVertexShadowParametersAttribute);

    auto stride = reinterpret_cast<char*>(&_vertices[1]) - reinterpret_cast<char*>(&_vertices[0]);
    glVertexAttribPointer(kVertexPositionAttribute, 2, GL_FLOAT, GL_FALSE, stride, &_vertices[0].x);
    glVertexAttribPointer(kVertexColorAttribute, 4, GL_FLOAT, GL_FALSE, stride, &_vertices[0].r);
    glVertexAttribPointer(kVertexBezierAttribute, 3, GL_FLOAT, GL_FALSE, stride, &_vertices[0].bu);
    glVertexAttribPointer(kVertexShadowBoxAttribute, 4, GL_FLOAT, GL_FALSE, stride, &_vertices[0].boxX);
    glVertexAttribPointer(kVertexShadowParametersAttribute, 2, GL_FLOAT, GL_FALSE, stride, &_vertices[0].spread);

    glDrawArrays(GL_TRIANGLES, 0, _vertices.size());
    
    glDisableVertexAttribArray(kVertexShadowParametersAttribute);
    glDisableVertexAttribArray(kVertexShadowBoxAttribute);
    glDisableVertexAttribArray(kVertexBezierAttribute);
    glDisableVertexAttribArray(kVertexColorAttribute);
    glDisableVertexAttribArray(kVertexPositionAttribute);
    
    _vertices.clear();
}

}}}