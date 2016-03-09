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

class Blending {
public:
    Blending(const BlendFunction& function);
    Blending(BlendFactor sourceRGB, BlendFactor destinationRGB, BlendFactor sourceAlpha, BlendFactor destinationAlpha);
    ~Blending();

private:
    BlendFunction _previous;

    static BlendFunction _sBlendFunction;
    static int _sBlendingDepth;

    static void SetBlendFunction(const BlendFunction& function, BlendFunction* previous = nullptr);
};

}} // namespace onair::okui
