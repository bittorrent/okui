#pragma once

#include "onair/okui/config.h"
#include "onair/okui/opengl/OpenGL.h"

namespace onair {
namespace okui {
namespace opengl {
	
class VertexBuffer {
public:
	VertexBuffer();
	~VertexBuffer();

	void store(const void* data, size_t size, GLenum usage = GL_STATIC_DRAW);
	void update(size_t offset, const void* data, size_t size);
	void loadAttribute(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, GLuint offset);
	
private:
	GLuint _object = 0;
};

}}}
