#include "bittorrent/ui/Stencil.h"

namespace bittorrent {
namespace ui {

int Stencil::_sStencilCount = 0;

Stencil::Stencil() {
	// TODO: support multiple contexts / larger stencil buffers
	auto mask = (1 << _sStencilCount);
	++_sStencilCount;
	assert(_sStencilCount <= 8);
	
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
	glColorMask(_colorMask[0], _colorMask[1], _colorMask[2], _colorMask[3]);
	glDepthMask(_depthMask);

	if (_stencilTestEnabled) {
		glStencilMask(_stencilWritemask);
		glStencilFunc(_stencilFunc, _stencilReference, _stencilValueMask);
		glStencilOp(_stencilFail, _stencilPassDepthPass, _stencilPassDepthFail);
	} else {
		glDisable(GL_STENCIL_TEST);
	}

	--_sStencilCount;
}

void Stencil::activate() {	
	glColorMask(_colorMask[0], _colorMask[1], _colorMask[2], _colorMask[3]);
	glDepthMask(_depthMask);

	glStencilMask(0x00);
	glStencilFunc(GL_EQUAL, 0, ~(0xff << _sStencilCount));
}

}}