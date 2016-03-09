#include "onair/okui/blending.h"

namespace onair {
namespace okui {

BlendFunction BlendFunction::kAlphaLock{BlendFactor::kDestinationAlpha, BlendFactor::kZero, BlendFactor::kZero, BlendFactor::kOne};
BlendFunction BlendFunction::kErasure{BlendFactor::kZero, BlendFactor::kOneMinusSourceAlpha, BlendFactor::kZero, BlendFactor::kOneMinusSourceAlpha};
BlendFunction BlendFunction::kPremultipliedAlpha{BlendFactor::kOne, BlendFactor::kOneMinusSourceAlpha, BlendFactor::kOne, BlendFactor::kOneMinusSourceAlpha};

BlendFunction Blending::_sBlendFunction;
int Blending::_sBlendingDepth = 0;

Blending::Blending(const BlendFunction& function) {
    if (++_sBlendingDepth == 1) {
        opengl::EnableBlending();
    }
    SetBlendFunction(function, &_previous);
}

Blending::Blending(BlendFactor sourceRGB, BlendFactor destinationRGB, BlendFactor sourceAlpha, BlendFactor destinationAlpha) {
    if (++_sBlendingDepth == 1) {
        opengl::EnableBlending();
    }
    SetBlendFunction(BlendFunction{sourceRGB, destinationRGB, sourceAlpha, destinationAlpha}, &_previous);
}

Blending::~Blending() {
    SetBlendFunction(_previous);
    if (--_sBlendingDepth == 0) {
        opengl::DisableBlending();
    }
}

void Blending::SetBlendFunction(const BlendFunction& function, BlendFunction* previous) {
    if (previous) {
        *previous = _sBlendFunction;
    }
    _sBlendFunction = function;
    opengl::SetBlendFunction(function.sourceRGB, function.destinationRGB, function.sourceAlpha, function.destinationAlpha);
}


}}
