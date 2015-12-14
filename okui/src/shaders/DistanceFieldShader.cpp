#include "onair/okui/shaders/DistanceFieldShader.h"

namespace onair {
namespace okui {
namespace shaders {

DistanceFieldShader::DistanceFieldShader() : TextureShader(
#if OPENGL_ES && GL_OES_standard_derivatives
        "#extension GL_OES_standard_derivatives : enable\n"
#endif
    ONAIR_OKUI_FRAGMENT_SHADER_HEADER
    R"(
        VARYING_IN vec4 color;
        VARYING_IN vec2 textureCoord;

        uniform sampler2D textureSampler;
        uniform float edge;
        uniform bool supersample;

        void main() {
            vec4 sample = SAMPLE(textureSampler, textureCoord);
    )"
#if !OPENGL_ES || GL_OES_standard_derivatives
            "float aa = fwidth(sample.a) * 0.75;"
#else
            "float aa = 0.03;"
#endif
    R"(
            if (sample.r < edge + aa) {
                discard;
            }

            float alpha = smoothstep(edge - aa, edge + aa, sample.a);

            if (supersample) {
    )"
#if !OPENGL_ES || GL_OES_standard_derivatives
                "float derivScale = 0.35355;" // 1 / sqrt(2) / 2
                "vec2 derivUV = derivScale * (dFdx(textureCoord) + dFdy(textureCoord));"
#else
                "vec2 derivUV = vec2(0.002, 0.0015);"
#endif
    R"(
                vec4 box = vec4(textureCoord-derivUV, textureCoord+derivUV);

                float sum = smoothstep(edge - aa, edge + aa, SAMPLE(textureSampler, box.xy).a)
                          + smoothstep(edge - aa, edge + aa, SAMPLE(textureSampler, box.zw).a)
                          + smoothstep(edge - aa, edge + aa, SAMPLE(textureSampler, box.xw).a)
                          + smoothstep(edge - aa, edge + aa, SAMPLE(textureSampler, box.zy).a);

                // Weighted average of the other points with the center point: give each of the 4 supersampled points
                // a 0.5 weight, and the center a wieght of 1, so the total is 0.5*4 + 1 = 3
                alpha = (alpha + 0.5 * sum) / 3.0;
            }

            COLOR_OUT = vec4(color.rgb, color.a * alpha);
        }
    )"
) {
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

}}}
