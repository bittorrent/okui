#pragma once

#include "bittorrent/ui/config.h"

#include "bittorrent/ui/opengl/OpenGL.h"
#include "bittorrent/ui/opengl/TextureCache.h"

namespace bittorrent {
namespace ui {

class Texture {
public:
    Texture(std::shared_ptr<const std::string> data);

    /**
    * Return the dimensions of the texture. Available immediately, even if the image is not loaded yet.
    *
    * Return 0 if the image data could not be parsed.
    */
    int width();
    int height();

    bool isLoaded() { return _cacheEntry ? true : false; }

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
    GLuint id() { return _cacheEntry->id; }

private:    
    std::shared_ptr<const std::string> _data;
    int _width{0}, _height{0};
    opengl::TextureCache::EntryReference _cacheEntry;
};

}}
