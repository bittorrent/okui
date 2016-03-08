#pragma once

#include "onair/okui/config.h"

#include "onair/okui/opengl/blending.h"

namespace onair {
namespace okui {

using opengl::BlendFactor;

struct BlendFunction {
    BlendFactor sourceRGB = BlendFactor::kOne;
    BlendFactor destinationRGB = BlendFactor::kZero;
    BlendFactor sourceAlpha = BlendFactor::kOne;
    BlendFactor destinationAlpha = BlendFactor::kZero;

    static BlendFunction kAlphaLock;
    static BlendFunction kErasure;
    static BlendFunction kPremultipliedAlpha;
};

void SetBlendFunction(const BlendFunction& function, BlendFunction* previous = nullptr);

class Blending {
public:
    Blending(const BlendFunction& function) {
        SetBlendFunction(function, &_previous);
    }

    Blending(BlendFactor sourceRGB, BlendFactor destinationRGB, BlendFactor sourceAlpha, BlendFactor destinationAlpha) {
        SetBlendFunction(BlendFunction{sourceRGB, destinationRGB, sourceAlpha, destinationAlpha}, &_previous);
    }

    ~Blending() {
        SetBlendFunction(_previous);
    }

private:
    BlendFunction _previous;
};

}} // namespace onair::okui
