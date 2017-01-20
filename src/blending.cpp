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
