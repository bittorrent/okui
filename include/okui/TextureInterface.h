#pragma once

#include "okui/config.h"

#include "okui/opengl/OpenGL.h"
#include "okui/opengl/TextureCache.h"

namespace okui {

class TextureInterface {
public:
    virtual ~TextureInterface() {}

    virtual bool hasMetadata() const { return isLoaded(); }

    /**
    * Return the dimensions of the texture. Available only when hasMetadata returns true.
    *
    * Return 0 if the image data could not be parsed.
    */
    virtual int width() const = 0;
    virtual int height() const = 0;

    /**
    * Returns the aspect ratio (width / height) of the texture. Available only when hasMetadata returns true.
    */
    double aspectRatio() const { return (double)width() / height(); }

    /**
    * Requires the render context to be active.
    *
    * Loads the texture to the GPU if necessary.
    *
    * This object can be deleted at any time, regardless of whether the render context is still active.
    */
    virtual void load() {}

    virtual bool isLoaded() const { return id(); }

    virtual bool hasPremultipliedAlpha() const { return false; }

    virtual int allocatedWidth() const { return width(); }
    virtual int allocatedHeight() const { return height(); }

    /**
    * If loaded, returns the id of the GPU texture. Otherwise, returns 0.
    */
    virtual GLuint id() const = 0;
};

} // namespace okui
