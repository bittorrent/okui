#pragma once

#include "bittorrent/ui/config.h"

#include "bittorrent/ui/opengl/OpenGL.h"

#include <unordered_map>

namespace bittorrent {
namespace ui {
namespace opengl {

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

}}}