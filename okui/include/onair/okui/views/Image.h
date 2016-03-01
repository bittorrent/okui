#pragma once

#include "onair/okui/Color.h"
#include "onair/okui/View.h"

#include "onair/optional.h"

namespace onair {
namespace okui {
namespace views {

class Image : public View {
public:
    void setTextureResource(std::string resource);
    void setTextureFromURL(std::string url);
    void setTextureColor(Color color);
    void setTextureDistanceField(double edge = 0.5);

    TextureHandle texture() { return _textureHandle.newHandle(); }

    virtual void render() override;
    virtual void windowChanged() override;

private:
    TextureHandle    _textureHandle;
    std::string      _resource;
    bool             _fromURL = false;
    optional<Color>  _color;
    optional<double> _distanceFieldEdge;
};

}}}
