#pragma once

#include "onair/okui/config.h"

#include "onair/okui/opengl/OpenGL.h"
#include "onair/okui/opengl/TextureCache.h"

namespace onair {
namespace okui {

class Texture {
public:
    Texture() {}
    Texture(std::shared_ptr<const std::string> data) { setData(data); }

    void setData(std::shared_ptr<const std::string> data);

    bool hasMetadata() const { return _data != nullptr; }

    /**
    * Return the dimensions of the texture. Available only when hasMetadata returns true.
    *
    * Return 0 if the image data could not be parsed.
    */
    int width() const { return _width; }
    int height() const { return _height; }

    /**
    * Returns the aspect ratio (width / height) of the texture. Available only when hasMetadata returns true.
    */
    double aspectRatio() const { return (double)_width / _height; }

    bool isLoaded() const { return _cacheEntry ? true : false; }

    /**
    * Requires the render context to be active.
    *
    * Loads the texture to the GPU.
    *
    * This object can be deleted at any time, regardless of whether the render context is still active.
    */
    GLuint load(opengl::TextureCache* textureCache);

    /**
    * If loaded, returns the id of the GPU texture.
    */
    GLuint id() const { return _cacheEntry->id; }

private:
    std::shared_ptr<const std::string> _data;
    int _width{0}, _height{0};
    opengl::TextureCache::EntryReference _cacheEntry;
};

}}
