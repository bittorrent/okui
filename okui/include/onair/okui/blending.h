#pragma once

#include "onair/okui/config.h"

#include "onair/okui/opengl/blending.h"

namespace onair {
namespace okui {

using opengl::BlendFactor;
using opengl::EnableBlending;
using opengl::SetBlendFunction;

inline void SetDefaultBlendFunction() {
    SetBlendFunction(BlendFactor::kSourceAlpha, BlendFactor::kOneMinusSourceAlpha, BlendFactor::kOne, BlendFactor::kOne);
}

inline void SetAlphaLockBlendFunction() {
    SetBlendFunction(BlendFactor::kSourceAlpha, BlendFactor::kOneMinusSourceAlpha, BlendFactor::kZero, BlendFactor::kOne);
}

inline void SetErasureBlendFunction() {
    SetBlendFunction(BlendFactor::kZero, BlendFactor::kOne, BlendFactor::kZero, BlendFactor::kOneMinusSourceAlpha);
}

}} // namespace onair::okui
