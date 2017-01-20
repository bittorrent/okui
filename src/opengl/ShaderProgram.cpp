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
#include <okui/opengl/ShaderProgram.h>

namespace okui::opengl {

ShaderProgram::ShaderProgram() {
    _program = glCreateProgram();
}

ShaderProgram::~ShaderProgram() {
    glDeleteProgram(_program);
}

void ShaderProgram::attachShader(const Shader& shader) {
    if (!_error.empty()) { return; }
    if (!(_error = shader.error()).empty()) { return; }

    glAttachShader(_program, shader.id());
}

void ShaderProgram::bindAttribute(GLuint id, const char* name) {
    glBindAttribLocation(_program, id, name);
}

void ShaderProgram::link() {
    if (!_error.empty()) { return; }

    glLinkProgram(_program);

    GLint linkStatus = GL_FALSE;
    glGetProgramiv(_program, GL_LINK_STATUS, &linkStatus);

    if (!linkStatus) {
        char buf[200];
        glGetProgramInfoLog(_program, sizeof(buf), nullptr, buf);
        _error = buf;
        glDeleteProgram(_program);
        _program = 0;
    }
}

} // namespace okui::opengl
