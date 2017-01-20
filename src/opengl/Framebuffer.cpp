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
#include <okui/opengl/Framebuffer.h>

#include <cassert>

namespace okui::opengl {

Framebuffer::Framebuffer() {
    glGenFramebuffers(1, &_framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer);
}

Framebuffer::~Framebuffer() {
    glDeleteFramebuffers(1, &_framebuffer);
}

bool Framebuffer::isComplete() {
    bind();
    return glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
}

void Framebuffer::bind() {
    glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer);
}

Framebuffer::Attachment* Framebuffer::addColorAttachment(int width, int height) {
    bind();

    _attachments.emplace_back(width, height);
    auto& attachment = _attachments.back();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, attachment.texture());

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + _colorAttachments, GL_TEXTURE_2D, attachment.texture(), 0);
    _drawBuffers.push_back(GL_COLOR_ATTACHMENT0 + _colorAttachments);
    ++_colorAttachments;

    glBindTexture(GL_TEXTURE_2D, 0);

#if !OPENGL_ES // TODO: fix for OpenGL ES
    glDrawBuffers(_drawBuffers.size(), _drawBuffers.data());
#endif
    return &attachment;
}

#if !OPENGL_ES
Framebuffer::Attachment* Framebuffer::addDepthStencilAttachment(int width, int height) {
    bind();

    _attachments.emplace_back(width, height, true);
    auto& attachment = _attachments.back();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, attachment.texture());

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, attachment.texture(), 0);

    glBindTexture(GL_TEXTURE_2D, 0);

    return &attachment;
}
#endif

Framebuffer::Attachment::Attachment(int width, int height, bool isDepthStencil) : _width(width), _height(height) {
    glGenTextures(1, &_texture);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _texture);

    if (isDepthStencil) {
#if !OPENGL_ES
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);
#else
        assert(false);
#endif
    } else {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    }

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // GL_CLAMP_TO_EDGE is required for npot attachments
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_2D, 0);
}

Framebuffer::Attachment::~Attachment() {
    if (_texture) {
        glDeleteTextures(1, &_texture);
    }
}

GLuint Framebuffer::Attachment::relinquishTexture() {
    auto ret = _texture;
    _texture = 0;
    return ret;
}

} // namespace okui::opengl
