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

    /**
    * okui shaders will do the appropriate conversions to make sure the source alpha is premultiplied when it should be.
    */
    bool premultipliedSourceAlpha = false;

    constexpr bool operator==(const BlendFunction& f) const {
        return sourceRGB == f.sourceRGB && destinationRGB == f.destinationRGB && sourceAlpha == f.sourceAlpha
            && destinationAlpha == f.destinationAlpha && premultipliedSourceAlpha == f.premultipliedSourceAlpha;
    }

    /**
    * The default blend function simply draws to the destination.
    */
    static BlendFunction kDefault;

    /**
    * Alpha lock prevents the destination color from becoming more or less opaque. It's useful for recoloring
    * sections of previously drawn shapes.
    */
    static BlendFunction kAlphaLock;

    /**
    * Erasure removes color components or opacity from the destination.
    *
    * Each channel is erased independently of any others, including the alpha channel. For example, to erase
    * all red from the destination, the (1.0, 0.0, 0.0, 0.0) color should be used.
    */
    static BlendFunction kErasure;
};

class Blending {
public:
    Blending(const BlendFunction& function);
    Blending(BlendFactor sourceRGB, BlendFactor destinationRGB, BlendFactor sourceAlpha, BlendFactor destinationAlpha, bool premultipliedSourceAlpha);
    ~Blending();

    static const BlendFunction& Current() {
        return _sBlendFunction;
    }

private:
    BlendFunction _previous;

    static BlendFunction _sBlendFunction;
    static int _sBlendingDepth;

    static void SetBlendFunction(const BlendFunction& function, BlendFunction* previous = nullptr);
};

} } //namespace onair::okui
