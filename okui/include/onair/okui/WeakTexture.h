#pragma once

#include "onair/okui/config.h"

#include "onair/okui/Texture.h"

namespace onair {
namespace okui {

class WeakTexture : public Texture {
public:
    void set(GLuint id = 0, int width = 0, int height = 0) {
        _id = id;
        _width = width;
        _height = height;
    }

    virtual int width() const override { return _width; }
    virtual int height() const override { return _height; }

    virtual GLuint id() const override { return _id; }

private:
    int _width{0}, _height{0};
    GLuint _id = 0;
};

}}
