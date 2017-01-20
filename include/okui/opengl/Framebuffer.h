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

#include <list>

namespace okui::opengl {

class Framebuffer {
public:
    Framebuffer();
    ~Framebuffer();

    bool isComplete();
    void bind();

    class Attachment {
    public:
        Attachment(int width, int height, bool isDepthStencil = false);
        ~Attachment();

        int width() const { return _width; }
        int height() const { return _height; }

        GLuint texture() const { return _texture; }

        /**
        * Relinquishes ownership of the texture to the caller.
        */
        GLuint relinquishTexture();

    private:
        GLuint _texture = 0;
        int _width, _height;
    };

    Attachment* addColorAttachment(int width, int height);
#if !OPENGL_ES
    Attachment* addDepthStencilAttachment(int width, int height);
#endif

private:
    GLuint _framebuffer = 0;
    std::list<Attachment> _attachments;
    int _colorAttachments = 0;
    std::vector<GLenum> _drawBuffers;
};

} // namespace okui::opengl
