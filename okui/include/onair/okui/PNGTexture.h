#pragma once

#include "onair/okui/config.h"

#include "onair/okui/Texture.h"

namespace onair {
namespace okui {

class PNGTexture : public Texture {
public:
    PNGTexture() {}
    PNGTexture(std::shared_ptr<const std::string> data) { setData(data); }

    void setData(std::shared_ptr<const std::string> data);

    virtual bool hasMetadata() const override { return _data != nullptr; }

    virtual int width() const override { return _width; }
    virtual int height() const override { return _height; }

    virtual GLuint load(opengl::TextureCache* textureCache) override;

    virtual GLuint id() const override { return _cacheEntry ? _cacheEntry->id : 0; }

private:
    std::shared_ptr<const std::string> _data;
    int _width{0}, _height{0};
    opengl::TextureCache::EntryReference _cacheEntry;
};

}}
