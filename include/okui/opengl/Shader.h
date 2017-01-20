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

#include <okui/opengl/opengl.h>

#include <string>

namespace okui::opengl {

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

} // namespace okui::opengl
