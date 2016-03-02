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

    TextureHandle texture() { return _texture.newHandle(); }

    virtual void render() override;
    virtual void windowChanged() override;

private:
    TextureHandle    _texture;
    std::string      _resource;
    bool             _fromURL = false;
    Color            _color = {1, 1, 1, 1};
    optional<double> _distanceFieldEdge;
};

}}}
