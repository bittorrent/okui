#include <okui/blending.h>

namespace okui {

BlendFunction BlendFunction::kDefault{BlendFactor::kOne, BlendFactor::kOneMinusSourceAlpha, BlendFactor::kOne, BlendFactor::kOneMinusSourceAlpha, true};
BlendFunction BlendFunction::kAlphaLock{BlendFactor::kDestinationAlpha, BlendFactor::kOneMinusSourceAlpha, BlendFactor::kZero, BlendFactor::kOne, true};
BlendFunction BlendFunction::kErasure{BlendFactor::kZero, BlendFactor::kOneMinusSourceColor, BlendFactor::kZero, BlendFactor::kOneMinusSourceAlpha, false};

BlendFunction Blending::_sBlendFunction;
int Blending::_sBlendingDepth = 0;

Blending::Blending(const BlendFunction& function) {
    ++_sBlendingDepth;
    opengl::EnableBlending();
    SetBlendFunction(function, &_previous);
}

Blending::Blending(BlendFactor sourceRGB, BlendFactor destinationRGB, BlendFactor sourceAlpha, BlendFactor destinationAlpha, bool premultipliedSourceAlpha) {
    ++_sBlendingDepth;
    opengl::EnableBlending();
    SetBlendFunction(BlendFunction{sourceRGB, destinationRGB, sourceAlpha, destinationAlpha, premultipliedSourceAlpha}, &_previous);
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

} // namespace okui
