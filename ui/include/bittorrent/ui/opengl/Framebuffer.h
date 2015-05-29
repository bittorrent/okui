#pragma once

#include "bittorrent/ui/config.h"

#include "bittorrent/ui/opengl/OpenGL.h"

#include <list>

namespace bittorrent {
namespace ui {
namespace opengl {

class Framebuffer {
public:
	Framebuffer();
	~Framebuffer();
	
	bool isComplete();
	void bind();
	
	class Attachment {
	public:
	    Attachment(int width, int height, bool isDepthStencil = false);
	    ~Attachment();
	
	    int width() const { return _width; }
	    int height() const { return _height; }
	
	    GLuint texture() const { return _texture; }
	        
	    /**
	    * Relinquishes ownership of the texture to the caller.
	    */
	    GLuint relinquishTexture();

	private:
	    GLuint _texture = 0;
	    int _width, _height;
	};
	
	Attachment* addColorAttachment(int width, int height);
#ifndef OPENGL_ES
	Attachment* addDepthStencilAttachment(int width, int height);
#endif

private:
	GLuint _framebuffer = 0;
	std::list<Attachment> _attachments;
	int _colorAttachments = 0;
	std::vector<GLenum> _drawBuffers;
};

}}}