#pragma once

#include "onair/okui/config.h"
#include "onair/okui/opengl/OpenGL.h"
#include "onair/okui/opengl/VertexArray.h"
#include "onair/okui/opengl/VertexBuffer.h"

namespace onair {
namespace okui {
namespace opengl {

/**
* Convenience class for when you just need a vertex array with a single buffer.
*/
class VertexArrayBuffer : public VertexArray, public VertexBuffer {
public:
    void bind() { VertexArray::bind(); VertexBuffer::bind(); }
    void unbind() { VertexArray::unbind(); VertexBuffer::unbind(); }

    void setAttribute(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, GLuint offset) {
        VertexArray::setAttribute(index, *this, size, type, normalized, stride, offset);
    }
};

}}}
