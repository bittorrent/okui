#include "onair/okui/opengl/VertexBuffer.h"

namespace onair {
namespace okui {
namespace opengl {
    
VertexBuffer::VertexBuffer() {
    glGenBuffers(1, &_object);
}

VertexBuffer::~VertexBuffer() {
    glDeleteBuffers(1, &_object);
}

void VertexBuffer::bind(GLenum target) {
    glBindBuffer(target, _object);
    _currentTarget = target;
}

void VertexBuffer::unbind() {
    if (_currentTarget) {
        glBindBuffer(_currentTarget, 0);
        _currentTarget = 0;
    }
}

void VertexBuffer::store(const uint8_t* data, size_t size, GLenum usage) {
    bind();
    glBufferData(GL_ARRAY_BUFFER, size, data, usage);
    unbind();
}

}}}
