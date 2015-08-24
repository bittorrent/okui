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

    GLuint id() const { return _object; }

    void bind(GLenum target = GL_ARRAY_BUFFER);
    void unbind();

    void store(const uint8_t* data, size_t size, GLenum usage = GL_STATIC_DRAW);
    
    template <typename T>
    void store(const T* data, size_t count, GLenum usage = GL_STATIC_DRAW) {
        store(reinterpret_cast<const uint8_t*>(data), count * sizeof(T), usage);
    }

    template <typename T>
    void stream(const T* data, size_t count) {
        store(data, count, GL_STREAM_DRAW);
    }

private:
    GLuint _object = 0;
    GLenum _currentTarget = 0;
};

}}}
