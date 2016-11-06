#pragma once

#include <okui/config.h>

#include <okui/opengl/opengl.h>

#include <string>

namespace okui {
namespace opengl {

class Shader {
public:
    enum Type {
        kVertexShader,
        kFragmentShader,
    };

    Shader(const char* source, Type type);
    Shader(const std::string& source, Type type) : Shader(source.c_str(), type) {}
    ~Shader();

    const std::string& error() const { return _error; }

    GLuint id() const { return _shader; }

private:
    std::string _error;
    GLuint _shader = 0;
};

} } // namespace okui::opengl
