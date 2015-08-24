#pragma once

#include "onair/okui/config.h"
#include "onair/platform.h"

#include "onair/okui/opengl/OpenGL.h"
#include "onair/okui/opengl/VertexBuffer.h"

namespace onair {
namespace okui {
namespace opengl {

#if OPENGL_ES && !ONAIR_IOS
// for now we need to emulate this for opengl es
// TODO: use extension for android? (some vendor implementations are allegedly crashy...)
#define ONAIR_OKUI_EMULATE_VERTEX_ARRAY 1
#endif

class VertexArray {
public:
    VertexArray();
    ~VertexArray();

    void bind();
    void unbind();

    void setAttribute(GLuint index, VertexBuffer& buffer, GLint size, GLenum type, GLboolean normalized, GLsizei stride, GLuint offset);

private:
#if ONAIR_OKUI_EMULATE_VERTEX_ARRAY
    std::vector<std::tuple<GLuint, GLuint, GLint, GLenum, GLboolean, GLsizei, GLuint>> _attributes;
#else
    GLuint _object = 0;
#endif
};

}}}
