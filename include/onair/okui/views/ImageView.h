#pragma once

#include "onair/okui/config.h"

#include "onair/okui/Color.h"
#include "onair/okui/View.h"

#include "scraps/stdts/optional.h"

namespace onair {
namespace okui {
namespace views {

class ImageView : public View {
public:
    enum class TextureDrawMode {
        kFill,
        kFit
    };

    void setTextureResource(std::string resource);
    void setTextureFromURL(std::string url, std::string placeholderResource = "");
    void setTextureColor(Color color);
    void setTextureDistanceField(double edge = 0.5);

    void setDrawMode(TextureDrawMode drawMode) { _drawMode = drawMode; }

    void clearTexture();

    TextureHandle& texture() { return _texture; }
    const Color& textureColor() const { return _color; }

    virtual void render() override;
    virtual void windowChanged() override;

private:
    TextureHandle                   _texture;
    TextureHandle                   _placeholderTexture;
    std::string                     _resource;
    std::string                     _placeholderResource;
    bool                            _fromURL = false;
    Color                           _color = {1, 1, 1, 1};
    scraps::stdts::optional<double> _distanceFieldEdge;
    TextureDrawMode                 _drawMode = TextureDrawMode::kFit;
};

}}}