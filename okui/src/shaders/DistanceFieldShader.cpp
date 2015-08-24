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
            COLOR_OUT = vec4(color.rgb, smoothstep(edge - aa, edge + aa, sample.a));
        }
    )"
) {
    _program.use();
    _edgeUniform = _program.uniform("edge");
}

void DistanceFieldShader::flush() {
    _program.use();
    _edgeUniform = (GLfloat)_edge;
    TextureShader::flush();
}

}}}
