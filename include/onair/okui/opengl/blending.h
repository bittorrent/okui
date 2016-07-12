#pragma once

#include "onair/okui/config.h"

#include "onair/okui/opengl/OpenGL.h"

namespace onair {
namespace okui {
namespace opengl {

enum class BlendFactor : GLenum {
    kZero = GL_ZERO,
    kOne = GL_ONE,
    kSourceColor = GL_SRC_COLOR,
    kOneMinusSourceColor = GL_ONE_MINUS_SRC_COLOR,
    kDestinationColor = GL_DST_COLOR,
    kOneMinusDestinationColor = GL_ONE_MINUS_DST_COLOR,
    kSourceAlpha = GL_SRC_ALPHA,
    kOneMinusSourceAlpha = GL_ONE_MINUS_SRC_ALPHA,
    kDestinationAlpha = GL_DST_ALPHA,
    kOneMinusDestinationAlpha = GL_ONE_MINUS_DST_ALPHA,
    kConstantColor = GL_CONSTANT_COLOR,
    kOneMinusConstantColor = GL_ONE_MINUS_CONSTANT_COLOR,
    kConstantAlpha = GL_CONSTANT_ALPHA,
    kOneMinusConstantAlpha = GL_ONE_MINUS_CONSTANT_ALPHA,
    kSourceAlphaSaturate = GL_SRC_ALPHA_SATURATE,
};

inline void EnableBlending() {
    glEnable(GL_BLEND);
}

inline void DisableBlending() {
    glDisable(GL_BLEND);
}

inline void SetBlendFunction(BlendFactor sourceRGB, BlendFactor destinationRGB, BlendFactor sourceAlpha, BlendFactor destinationAlpha) {
    glBlendFuncSeparate(static_cast<GLenum>(sourceRGB), static_cast<GLenum>(destinationRGB), static_cast<GLenum>(sourceAlpha), static_cast<GLenum>(destinationAlpha));
}

}}} // namespace onair::okui::opengl