#pragma once

#include "onair/okui/config.h"

#include "onair/okui/shaders/TextureShader.h"

namespace onair {
namespace okui {
namespace shaders {

class DistanceFieldShader : public TextureShader {
public:
    DistanceFieldShader();

    void setEdge(double edge) { _edge = edge; }

    virtual void flush() override;

private:
    double _edge = 0.5;
    opengl::ShaderProgram::Uniform _edgeUniform;
};

}}}
