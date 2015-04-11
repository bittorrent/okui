#include "bittorrent/ui/opengl/VertexBuffer.h"

namespace bittorrent {
namespace ui {
namespace opengl {
	
VertexBuffer::VertexBuffer() {
	glGenBuffers(1, &_object);
}

VertexBuffer::~VertexBuffer() {
	glDeleteBuffers(1, &_object);
}

void VertexBuffer::store(const void* data, size_t size, GLenum usage) {
	glBindBuffer(GL_ARRAY_BUFFER, _object);
	glBufferData(GL_ARRAY_BUFFER, size, data, usage);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VertexBuffer::update(size_t offset, const void* data, size_t size) {
	glBindBuffer(GL_ARRAY_BUFFER, _object);
	glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VertexBuffer::loadAttribute(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, GLuint offset) {
	glBindBuffer(GL_ARRAY_BUFFER, _object);
	glVertexAttribPointer(index, size, type, normalized, stride, reinterpret_cast<GLvoid*>(offset));
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

}}}