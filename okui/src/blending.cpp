#include "onair/okui/blending.h"

namespace onair {
namespace okui {

BlendFunction BlendFunction::kAlphaLock{BlendFactor::kSourceAlpha, BlendFactor::kOneMinusSourceAlpha, BlendFactor::kZero, BlendFactor::kOne};
BlendFunction BlendFunction::kErasure{BlendFactor::kZero, BlendFactor::kOne, BlendFactor::kZero, BlendFactor::kOneMinusSourceAlpha};
BlendFunction BlendFunction::kPremultipliedAlpha{BlendFactor::kOne, BlendFactor::kOneMinusSourceAlpha, BlendFactor::kOne, BlendFactor::kOneMinusSourceAlpha};

namespace {

bool gBlendingEnabled = false;
BlendFunction gBlendFunction;

}

void SetBlendFunction(const BlendFunction& function, BlendFunction* previous) {
    if (previous) {
        *previous = gBlendFunction;
    }
    if (!gBlendingEnabled) {
        opengl::EnableBlending();
        gBlendingEnabled = true;
    }
    opengl::SetBlendFunction(function.sourceRGB, function.destinationRGB, function.sourceAlpha, function.destinationAlpha);
}


}}
