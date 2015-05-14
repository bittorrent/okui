#pragma once

#include "bittorrent/ui/config.h"

#include "bittorrent/ui/opengl/OpenGL.h"

#include <vector>

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
	    Attachment(int width, int height);
	    ~Attachment();
	
	    int width() const { return _width; }
	    int height() const { return _height; }
	
	    GLuint texture() const { return _texture; }
	    GLuint releaseTexture();

	private:
	    GLuint _texture = 0;
	    int _width, _height;
	};
	
	Attachment* addColorAttachment(int width, int height);

private:
	GLuint _framebuffer = 0;
	std::vector<Attachment> _attachments;
	std::vector<GLenum> _drawBuffers;
};

}}}