#include "bittorrent/ui/opengl/Framebuffer.h"

namespace bittorrent {
namespace ui {
namespace opengl {

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
    
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + _attachments.size() - 1, GL_TEXTURE_2D, attachment.texture(), 0);
    _drawBuffers.push_back(GL_COLOR_ATTACHMENT0 + _attachments.size() - 1);
    
#ifndef OPENGL_ES
    glDrawBuffers(_drawBuffers.size(), _drawBuffers.data());
#endif
    return &attachment;
}

Framebuffer::Attachment::Attachment(int width, int height) {
    glGenTextures(1, &_texture);
    glBindTexture(GL_TEXTURE_2D, _texture);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

Framebuffer::Attachment::~Attachment() {
    if (_texture) {
        glDeleteTextures(1, &_texture);
    }
}

GLuint Framebuffer::Attachment::releaseTexture() {
    auto ret = _texture;
    _texture = 0;
    return ret;
}

}}}