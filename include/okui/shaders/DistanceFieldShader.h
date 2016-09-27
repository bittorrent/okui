#pragma once

#include "okui/config.h"

#include "okui/shaders/TextureShader.h"

namespace okui {
namespace shaders {

class DistanceFieldShader : public TextureShader {
public:
    DistanceFieldShader();

    void setEdge(double edge) { _edge = edge; }

    void enableSupersampling(bool enable) { _supersample = enable; }

    virtual void flush() override;

private:
    double _edge = 0.5;
    opengl::ShaderProgram::Uniform _edgeUniform;
    bool _supersample = false;
    opengl::ShaderProgram::Uniform _supersampleUniform;

    std::string FragmentShader();
};

} } // namespace okui::shaders
