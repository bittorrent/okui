#include "onair/okui/views/ImageView.h"

namespace onair {
namespace okui {
namespace views {

void ImageView::clearTexture() {
    _texture = nullptr;
    _resource.clear();
}

void ImageView::setTextureResource(std::string resource) {
    _texture = loadTextureResource(resource);
    _resource = std::move(resource);
    _fromURL = false;
}

void ImageView::setTextureFromURL(std::string url) {
    _texture = loadTextureFromURL(url);
    _resource = std::move(url);
    _fromURL = true;
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
    if (_texture.isLoaded()) {
        auto shader = textureShader();

        if (_distanceFieldEdge) {
            auto dfShader = distanceFieldShader();
            dfShader->setEdge(*_distanceFieldEdge);
            shader = dfShader;
        }

        shader->setColor(_color);
        shader->drawScaledFit(*_texture, 0, 0, bounds().width, bounds().height);
        shader->flush();
    }
}

void ImageView::windowChanged() {
    if (!_resource.empty()) {
        _texture = _fromURL ? loadTextureFromURL(_resource) : loadTextureResource(_resource);
    }
}

}}}
