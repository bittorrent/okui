#include "onair/okui/opengl/VertexArray.h"

namespace onair {
namespace okui {
namespace opengl {
    
#if SCRAPS_IOS || SCRAPS_TVOS
#define glGenVertexArrays glGenVertexArraysOES
#define glDeleteVertexArrays glDeleteVertexArraysOES
#define glBindVertexArray glBindVertexArrayOES
#endif
    
VertexArray::VertexArray() {
#if !ONAIR_OKUI_EMULATE_VERTEX_ARRAY
    glGenVertexArrays(1, &_object);
#endif
}

VertexArray::~VertexArray() {
#if !ONAIR_OKUI_EMULATE_VERTEX_ARRAY
    glDeleteVertexArrays(1, &_object);
#endif
}

void VertexArray::bind() {
#if !ONAIR_OKUI_EMULATE_VERTEX_ARRAY
    glBindVertexArray(_object);
#else
    for (auto& attribute : _attributes) {
        glEnableVertexAttribArray(std::get<0>(attribute));
        glBindBuffer(GL_ARRAY_BUFFER, std::get<1>(attribute));
        glVertexAttribPointer(std::get<0>(attribute), std::get<2>(attribute), std::get<3>(attribute), std::get<4>(attribute), std::get<5>(attribute), reinterpret_cast<GLvoid*>(std::get<6>(attribute)));
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
#endif
}

void VertexArray::unbind() {
#if !ONAIR_OKUI_EMULATE_VERTEX_ARRAY
    glBindVertexArray(0);
#else
    for (auto& attribute : _attributes) {
        glDisableVertexAttribArray(std::get<0>(attribute));
    }
#endif
}

void VertexArray::setAttribute(GLuint index, VertexBuffer& buffer, GLint size, GLenum type, GLboolean normalized, GLsizei stride, GLuint offset) {
#if !ONAIR_OKUI_EMULATE_VERTEX_ARRAY
    bind();
    buffer.bind();
    glEnableVertexAttribArray(index);
    glVertexAttribPointer(index, size, type, normalized, stride, reinterpret_cast<GLvoid*>(offset));
    unbind();
#else
    _attributes.emplace_back(index, buffer.id(), size, type, normalized, stride, offset);
#endif
}

}}}
