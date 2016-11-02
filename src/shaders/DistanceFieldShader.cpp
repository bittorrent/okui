#include "okui/shaders/DistanceFieldShader.h"

namespace okui {
namespace shaders {

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
        uniform float edge;
        uniform bool supersample;

        void main() {
            vec4 sample = SAMPLE(textureSampler, textureCoord);
            float aa = )" + (useStandardDerivatives ? "fwidth(sample.a) * 0.75" : "0.03") + R"(;
            float alpha = smoothstep(edge - aa, edge + aa, sample.a);

            if (supersample) {
                vec2 derivUV = )" + (useStandardDerivatives ? "0.35355 * (dFdx(textureCoord) + dFdy(textureCoord));" : "vec2(0.002, 0.0015);") + R"(
                vec4 box = vec4(textureCoord-derivUV, textureCoord+derivUV);

                float sum = smoothstep(edge - aa, edge + aa, SAMPLE(textureSampler, box.xy).a)
                          + smoothstep(edge - aa, edge + aa, SAMPLE(textureSampler, box.zw).a)
                          + smoothstep(edge - aa, edge + aa, SAMPLE(textureSampler, box.xw).a)
                          + smoothstep(edge - aa, edge + aa, SAMPLE(textureSampler, box.zy).a);

                // Weighted average of the other points with the center point: give each of the 4 supersampled points
                // a 0.5 weight, and the center a wieght of 1, so the total is 0.5*4 + 1 = 3
                alpha = (alpha + 0.5 * sum) / 3.0;
            }

            COLOR_OUT = multipliedOutput(vec4(color.rgb, color.a * alpha));
        }
    )";
}

DistanceFieldShader::DistanceFieldShader() : TextureShader(FragmentShader()) {
    _program.use();
    _edgeUniform = _program.uniform("edge");
    _supersampleUniform = _program.uniform("supersample");
}

void DistanceFieldShader::flush() {
    _program.use();
    _edgeUniform = (GLfloat)_edge;
    _supersampleUniform = (GLboolean)_supersample;
    TextureShader::flush();
}

} } // namespace okui::shaders
