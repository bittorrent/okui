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

#include <okui/opengl/opengl.h>

namespace okui::opengl {

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

} // namespace okui::opengl
