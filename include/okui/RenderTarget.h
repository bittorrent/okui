#pragma once

#include <okui/config.h>

namespace okui {

class RenderTarget {
public:
    RenderTarget(int width, int height) : _width(width), _height(height) {}

    int width() const { return _width; }
    int height() const { return _height; }

private:
    int _width, _height;
};

} // namespace okui
