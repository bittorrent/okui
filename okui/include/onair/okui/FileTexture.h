#pragma once

#include "onair/okui/config.h"

#include "onair/okui/Texture.h"

namespace onair {
namespace okui {

class FileTexture : public Texture {
public:
    FileTexture() = default;
    FileTexture(std::shared_ptr<const std::string> data) { setData(data); }

    void setData(std::shared_ptr<const std::string> data);

    enum class Type {
        kUnknown, kPNG, kJPEG
    };

    Type type() const { return _type; }
    virtual bool hasMetadata() const override { return _type != Type::kUnknown; }

    virtual int width() const override { return _width; }
    virtual int height() const override { return _height; }

    virtual GLuint load(opengl::TextureCache* textureCache) override;

    virtual GLuint id() const override { return _cacheEntry ? _cacheEntry->id : 0; }

    virtual int allocatedWidth() const override { return _allocatedWidth; }
    virtual int allocatedHeight() const override { return _allocatedHeight; }

private:
    std::shared_ptr<const std::string> _data;
    Type _type = Type::kUnknown;
    int _width{0}, _height{0};
    int _allocatedWidth{0}, _allocatedHeight{0};
    opengl::TextureCache::EntryReference _cacheEntry;
    
    bool _readPNGMetadata();
    GLuint _loadPNG(opengl::TextureCache* textureCache);

    bool _readJPEGMetadata();
    GLuint _loadJPEG(opengl::TextureCache* textureCache);
    
    GLuint _generateTexture(GLint internalformat, GLenum format, GLenum type, const GLvoid * data);
};

}}
