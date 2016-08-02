#include "onair/okui/views/ImageView.h"

namespace onair {
namespace okui {
namespace views {

void ImageView::clearTexture() {
    unload();
    _resource.clear();
    _placeholderResource.clear();
    _distanceFieldEdge = stdts::nullopt;
}

void ImageView::setTextureResource(std::string resource) {
    clearTexture();
    _resource = std::move(resource);
    _fromURL = false;

    if (isVisible()) {
        load();
    }
}

void ImageView::setTextureFromURL(std::string url, std::string placeholderResource) {
    clearTexture();
    _resource = std::move(url);
    _fromURL = true;
    _placeholderResource = std::move(placeholderResource);

    if (isVisible()) {
        load();
    }
}

void ImageView::setTextureColor(Color color) {
    _color = color;
    invalidateRenderCache();
}

void ImageView::setTextureDistanceField(double edge) {
    _distanceFieldEdge = edge;
    invalidateRenderCache();
}

void ImageView::render() {
    TextureHandle* texture = (_texture && _texture.isLoaded())                       ? &_texture :
                             (_placeholderTexture && _placeholderTexture.isLoaded()) ? &_placeholderTexture :
                             nullptr;

    if (texture) {
        auto shader = textureShader();

        if (_distanceFieldEdge) {
            auto dfShader = distanceFieldShader();
            dfShader->setEdge(*_distanceFieldEdge);
            shader = dfShader;
        }

        shader->setColor(_color);
        if (_drawMode == TextureDrawMode::kFit) {
            shader->drawScaledFit(**texture, 0, 0, bounds().width, bounds().height);
        } else {
            shader->drawScaledFill(**texture, 0, 0, bounds().width, bounds().height);
        }

        shader->flush();
    }
}

void ImageView::windowChanged() {
    unload();
    load();
}

void ImageView::willAppear() {
    load();
}

void ImageView::load() {
    if (!_resource.empty() && !_texture) {
        _texture = _fromURL ? loadTextureFromURL(_resource) : loadTextureResource(_resource);
    }

    if (!_placeholderResource.empty() && !_placeholderTexture) {
        _placeholderTexture = loadTextureResource(_placeholderResource);
    }
}

void ImageView::unload() {
    _texture = nullptr;
    _placeholderTexture = nullptr;
}

}}}
