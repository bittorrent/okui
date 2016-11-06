#pragma once

#include <okui/config.h>

#include <okui/opengl/opengl.h>

#include <list>

namespace okui {
namespace opengl {

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

} } // namespace okui::opengl
