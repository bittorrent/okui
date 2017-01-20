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
#include <okui/opengl/Stencil.h>

namespace okui::opengl {

std::unordered_map<const void*, std::unordered_map<GLint, Stencil::State>> Stencil::_sStencilState;

Stencil::Stencil(const void* context) : _context(context) {
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &_framebuffer);
    auto& state = _state();

    // TODO: support larger stencil buffers
    auto mask = (1 << state.count);
    ++state.count;
    assert(state.count <= 8);

    _stencilTestEnabled = glIsEnabled(GL_STENCIL_TEST);
    if (_stencilTestEnabled) {
        glGetIntegerv(GL_STENCIL_WRITEMASK, &_stencilWritemask);
        glGetIntegerv(GL_STENCIL_FUNC, &_stencilFunc);
        glGetIntegerv(GL_STENCIL_REF, &_stencilReference);
        glGetIntegerv(GL_STENCIL_VALUE_MASK, &_stencilValueMask);
        glGetIntegerv(GL_STENCIL_FAIL, &_stencilFail);
        glGetIntegerv(GL_STENCIL_PASS_DEPTH_PASS, &_stencilPassDepthPass);
        glGetIntegerv(GL_STENCIL_PASS_DEPTH_FAIL, &_stencilPassDepthFail);
    }

    glEnable(GL_STENCIL_TEST);

    glStencilMask(mask);
    glStencilFunc(GL_ALWAYS, 0xff, mask);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

    glClear(GL_STENCIL_BUFFER_BIT);

    glGetBooleanv(GL_COLOR_WRITEMASK, _colorMask);
    glGetBooleanv(GL_DEPTH_WRITEMASK, &_depthMask);

    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    glDepthMask(GL_FALSE);
}

Stencil::~Stencil() {
    auto& state = _state();

    glColorMask(_colorMask[0], _colorMask[1], _colorMask[2], _colorMask[3]);
    glDepthMask(_depthMask);

    if (_stencilTestEnabled) {
        glStencilMask(_stencilWritemask);
        glStencilFunc(_stencilFunc, _stencilReference, _stencilValueMask);
        glStencilOp(_stencilFail, _stencilPassDepthPass, _stencilPassDepthFail);
    } else {
        glDisable(GL_STENCIL_TEST);
    }

    --state.count;
    if (_isActivated) {
        state.funcRef = (state.funcRef >> 1);
    }

    if (!state.count) {
        auto& contextState = _sStencilState[_context];
        contextState.erase(_framebuffer);
        if (contextState.empty()) {
            _sStencilState.erase(_context);
        }
    }
}

void Stencil::activate(bool invert) {
    auto& state = _state();

    _isActivated = true;

    glColorMask(_colorMask[0], _colorMask[1], _colorMask[2], _colorMask[3]);
    glDepthMask(_depthMask);

    state.funcRef = (state.funcRef << 1) | (invert ? 1 : 0);

    glStencilMask(0x00);
    glStencilFunc(GL_EQUAL, state.funcRef, ~(0xff << state.count));
}

Stencil::State& Stencil::_state() {
    return _sStencilState[_context][_framebuffer];
}

} // namespace okui::opengl
