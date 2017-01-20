/**
* Copyright 2017 BitTorrent Inc.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*    http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/
#pragma once

#include <okui/config.h>

#include <okui/Color.h>
#include <okui/opengl/Shader.h>

namespace okui::opengl {

class ShaderProgram {
public:
    ShaderProgram();
    ~ShaderProgram();

    void attachShader(const Shader& shader);

    inline void attachShaders() {}

    template <typename T, typename... Rem>
    void attachShaders(T&& shader, Rem&&... rem) {
        if (!_error.empty()) { return; }
        attachShader(std::forward<T>(shader));
        attachShaders(std::forward<Rem>(rem)...);
    }

    void bindAttribute(GLuint id, const char* name);

    void link();

    const std::string& error() const { return _error; }

    GLuint id() const { return _program; }
    void use() const { glUseProgram(_program); }

    struct Uniform {
        Uniform() {}
        explicit Uniform(GLint location_) : location{location_} {}
        void operator=(GLint n) { glUniform1i(location, n); }
        void operator=(GLfloat f) { glUniform1f(location, f); }
        void operator=(Color c) { glUniform4f(location, static_cast<GLfloat>(c.redF()), static_cast<GLfloat>(c.greenF()), static_cast<GLfloat>(c.blueF()), static_cast<GLfloat>(c.alphaF())); }
        void setMatrix4(const GLfloat* m, size_t count = 1) { glUniformMatrix4fv(location, static_cast<GLsizei>(count), GL_FALSE, m); }

        GLint location = GL_INVALID_VALUE;
    };

    Uniform uniform(const char* name) { return Uniform(glGetUniformLocation(_program, name)); }

private:
    std::string _error;
    GLuint _program = 0;
};

} // namespace okui::opengl
