#include "okui/views/ImageView.h"

namespace okui {
namespace views {

void ImageView::setTexture(std::string texture) {
    if (texture == _resource) { return; }

    _texture = nullptr;
    _fromURL = texture.find("://") != std::string::npos;
    _resource = std::move(texture);

    if (isVisibleInOpenWindow()) {
        load();
    }
}

void ImageView::setTexturePlaceholder(std::string placeholder) {
    if (_placeholderResource == placeholder) { return; }

    _placeholderTexture = nullptr;
    _placeholderResource = std::move(placeholder);

    if (isVisibleInOpenWindow()) {
        load();
    }
}

void ImageView::setTextureColor(Color color) {
    _color = color;
    invalidateRenderCache();
}

void ImageView::setTextureDistanceField(stdts::optional<double> edge) {
    if (edge == _distanceFieldEdge) { return; }
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

    if (isVisibleInOpenWindow()) {
        load();
    }
}

void ImageView::willAppear() {
    load();
}

void ImageView::load() {
    if (!_placeholderResource.empty() && !_placeholderTexture) {
        _placeholderTexture = loadTextureResource(_placeholderResource);
    }

    if (!_resource.empty() && !_texture) {
        _texture = _fromURL ? loadTextureFromURL(_resource) : loadTextureResource(_resource);
    }
}

void ImageView::unload() {
    _texture = nullptr;
    _placeholderTexture = nullptr;
}

} } // namespace okui::views
