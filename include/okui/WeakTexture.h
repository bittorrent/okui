#pragma once

#include "okui/config.h"

#include "okui/TextureInterface.h"

namespace okui {

class WeakTexture : public TextureInterface {
public:
    void set(GLuint id = 0, int width = 0, int height = 0, bool hasPremultipliedAlpha = false) {
        _id = id;
        _width = width;
        _height = height;
        _hasPremultipliedAlpha = hasPremultipliedAlpha;
    }

    virtual int width() const override { return _width; }
    virtual int height() const override { return _height; }

    virtual GLuint id() const override { return _id; }

    virtual bool hasPremultipliedAlpha() const override { return _hasPremultipliedAlpha; }

private:
    int    _width = 0;
    int    _height = 0;
    GLuint _id = 0;
    bool   _hasPremultipliedAlpha{false};
};

} // namespace okui
