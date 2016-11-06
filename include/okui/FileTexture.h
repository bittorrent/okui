#pragma once

#include <okui/config.h>

#include <okui/TextureInterface.h>

namespace okui {

class FileTexture : public TextureInterface {
public:
    enum class Type {
        kUnknown,
        kPNG,
        kJPEG
    };

    FileTexture() = default;
    explicit FileTexture(std::string name) : _name{std::move(name)} {}
    explicit FileTexture(std::shared_ptr<const std::string> data, std::string name = "") { setData(std::move(data), std::move(name)); }
    virtual ~FileTexture();

    void setName(std::string name) { _name = std::move(name); }
    void setData(std::shared_ptr<const std::string> data, std::string name = "");

    const std::string& name() const           { return _name; }
    Type type() const                         { return _type; }
    virtual bool hasMetadata() const override { return _type != Type::kUnknown; }

    virtual int width() const override        { return _width; }
    virtual int height() const override       { return _height; }

    /**
    * returns success
    */
    bool decompress();

    virtual void load() override;

    virtual GLuint id() const override           { return _id; }

    virtual int allocatedWidth() const override  { return _allocatedWidth; }
    virtual int allocatedHeight() const override { return _allocatedHeight; }

private:
    struct TextureType {
        GLenum format;
        GLenum type;
    };

    bool _readPNGMetadata();
    bool _loadPNG();
    bool _readJPEGMetadata();
    bool _loadJPEG();

    std::string                          _name;
    std::shared_ptr<const std::string>   _data; // typically a reference into the application cache
    std::vector<uint8_t>                 _decompressedData;
    Type                                 _type = Type::kUnknown;
    int                                  _width = 0;
    int                                  _height = 0;
    int                                  _allocatedWidth = 0;
    int                                  _allocatedHeight = 0;
    TextureType                          _textureType;
    GLuint                               _id = 0;
};

} // namespace okui
