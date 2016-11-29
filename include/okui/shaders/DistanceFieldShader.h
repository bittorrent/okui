#pragma once

#include <okui/config.h>

#include <okui/shaders/TextureShader.h>

#include <vector>

namespace okui::shaders {

class DistanceFieldShader : public TextureShader {
public:
    DistanceFieldShader();

    /**
    * Configures a single region that extends to the given edge.
    */
    void setEdge(double edge) { _regions = {{1.0, okui::Color::kWhite, edge, okui::Color::kWhite}}; }

    struct Region {
        Region() = default;
        Region(double innerEdge, okui::Color innerColor, double outerEdge, okui::Color outerColor)
            : innerEdge(innerEdge), innerColor(std::move(innerColor)), outerEdge(outerEdge), outerColor(std::move(outerColor)) {}

        double innerEdge;
        okui::Color innerColor;
        double outerEdge;
        okui::Color outerColor;
    };

    template <typename T>
    void setRegions(T&& regions) { _regions = std::move(regions); }

    void enableSupersampling(bool enable) { _supersample = enable; }

    virtual void flush() override;

protected:
    virtual void _draw() override;

private:
    std::vector<Region> _regions;
    opengl::ShaderProgram::Uniform _innerEdgeUniform;
    opengl::ShaderProgram::Uniform _innerColorUniform;
    opengl::ShaderProgram::Uniform _outerEdgeUniform;
    opengl::ShaderProgram::Uniform _outerColorUniform;
    bool _supersample = false;
    opengl::ShaderProgram::Uniform _supersampleUniform;

    std::string FragmentShader();
};

} // namespace okui::shaders
