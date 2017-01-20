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

#include <unordered_map>

namespace okui::opengl {

/**
* Stencils allow masking of shapes with other shapes.
*
* Create an instance and draw the stencil shape. After calling activate(), shapes
* will not be drawn over the area that the stencil covers.
*/
class Stencil {
public:
    /**
    * @param context an opaque pointer representing the OpenGL context. stencils are specific to a single
    *                framebuffer within a context. for most applications, passing a window pointer here
    *                is sufficient.
    */
    Stencil(const void* context);
    ~Stencil();

    /**
    * Call this method to activate the stencil once you are done drawing it.
    */
    void activate(bool invert = false);

private:
    const void* _context = nullptr;
    GLint _framebuffer = 0;
    GLboolean _colorMask[4];
    GLboolean _depthMask;
    GLboolean _stencilTestEnabled;
    GLint _stencilWritemask, _stencilFunc, _stencilReference, _stencilValueMask;
    GLint _stencilFail, _stencilPassDepthPass, _stencilPassDepthFail;
    bool _isActivated = false;

    struct State {
        int count = 0;
        int funcRef = 0;
    };

    // _sStencilState[context][framebuffer] = State
    static std::unordered_map<const void*, std::unordered_map<GLint, State>> _sStencilState;

    State& _state();
};

} // namespace okui::opengl
