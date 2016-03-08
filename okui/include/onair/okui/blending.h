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
    template <typename... Args>
    Blending(Args&&... args) {
        SetBlendFunction(BlendFunction{std::forward<Args>(args)...}, &_previous);
    }

    ~Blending() {
        SetBlendFunction(_previous);
    }

private:
    BlendFunction _previous;
};

}} // namespace onair::okui
