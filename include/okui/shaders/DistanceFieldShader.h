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
