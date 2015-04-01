#pragma once

#include "bittorrent/ui/config.h"

#include "bittorrent/ui/opengl/OpenGL.h"

namespace bittorrent {
namespace ui {

/**
* Stencils allow masking of shapes with other shapes.
*
* Create an instance and draw the stencil shape. After calling activate(), shapes
* will not be drawn over the area that the stencil covers.
*/
class Stencil {
public:
	Stencil();
	~Stencil();
	
	/**
	* Call this method to activate the stencil once you are done drawing it.
	*/
	void activate();

private:
	GLboolean _colorMask[4];
	GLboolean _depthMask;
	GLboolean _stencilTestEnabled;
	GLint _stencilWritemask, _stencilFunc, _stencilReference, _stencilValueMask;
	GLint _stencilFail, _stencilPassDepthPass, _stencilPassDepthFail;
	
	static int _sStencilCount;
};

}}