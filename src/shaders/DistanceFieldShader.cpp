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
#include <okui/shaders/DistanceFieldShader.h>

namespace okui::shaders {

std::string DistanceFieldShader::FragmentShader() {
    bool useStandardDerivatives = !scraps::opengl::kIsOpenGLES || scraps::opengl::MajorVersion() >= 3;
    std::vector<std::string> extensions;
    if (!useStandardDerivatives && scraps::opengl::HasExtension("GL_OES_standard_derivatives")) {
        useStandardDerivatives = true;
        extensions.emplace_back("GL_OES_standard_derivatives");
    }
    return CommonOKUIFragmentShaderHeader(extensions) +
    R"(
        VARYING_IN vec4 color;
        VARYING_IN vec2 textureCoord;

        uniform sampler2D textureSampler;
        uniform float innerEdge;
        uniform vec4 innerColor;
        uniform float outerEdge;
        uniform vec4 outerColor;
        uniform bool supersample;

        void main() {
            float sa = SAMPLE(textureSampler, textureCoord).a;
            float aa = )" + (useStandardDerivatives ? "fwidth(sa) * 0.75" : "0.03") + R"(;
            float alpha = smoothstep(outerEdge - aa, outerEdge + aa, sa) * (1.0 - smoothstep(innerEdge - aa, innerEdge + aa, sa));

            if (supersample) {
                vec2 derivUV = )" + (useStandardDerivatives ? "0.35355 * (dFdx(textureCoord) + dFdy(textureCoord));" : "vec2(0.002, 0.0015);") + R"(
                vec4 box = vec4(textureCoord-derivUV, textureCoord+derivUV);

                vec4 ssa = vec4(SAMPLE(textureSampler, box.xy).a, SAMPLE(textureSampler, box.zw).a, SAMPLE(textureSampler, box.xw).a, SAMPLE(textureSampler, box.zy).a);
                vec4 alpha = smoothstep(outerEdge - aa, outerEdge + aa, ssa) * (1.0 - smoothstep(innerEdge - aa, innerEdge + aa, ssa));
                float sum = alpha.x + alpha.y + alpha.z + alpha.w;

                // Weighted average of the other points with the center point: give each of the 4 supersampled points
                // a 0.5 weight, and the center a wieght of 1, so the total is 0.5*4 + 1 = 3
                alpha = (alpha + 0.5 * sum) / 3.0;
            }

            vec4 c = color * mix(outerColor, innerColor, smoothstep(outerEdge, innerEdge, sa));
            COLOR_OUT = multipliedOutput(vec4(c.rgb, c.a * alpha));
        }
    )";
}

DistanceFieldShader::DistanceFieldShader() : TextureShader(FragmentShader()) {
    _program.use();
    _innerEdgeUniform   = _program.uniform("innerEdge");
    _innerColorUniform  = _program.uniform("innerColor");
    _outerEdgeUniform   = _program.uniform("outerEdge");
    _outerColorUniform  = _program.uniform("outerColor");
    _supersampleUniform = _program.uniform("supersample");
}

void DistanceFieldShader::flush() {
    _program.use();
    _supersampleUniform = (GLboolean)_supersample;
    TextureShader::flush();
}

void DistanceFieldShader::_draw() {
    for (auto& region : _regions) {
        // if this region includes the fully opaque areas of the distance field, set the inner edge
        // to something high so we don't try to anti-alias it
        _innerEdgeUniform = static_cast<GLfloat>(region.innerEdge >= 1.0 ? 100.0 : region.innerEdge);

        _innerColorUniform = region.innerColor;
        _outerEdgeUniform = static_cast<GLfloat>(region.outerEdge);
        _outerColorUniform = region.outerColor;
        TextureShader::_draw();
    }
}

} // namespace okui::shaders
