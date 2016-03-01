#include "onair/okui/views/Image.h"

namespace onair {
namespace okui {
namespace views {

using namespace onair;

void Image::setTextureResource(std::string resource) {
    _fromURL = false;

    if (window()) {
        _textureHandle = loadTextureResource(resource);
    }

    _resource = std::move(resource);
    invalidateRenderCache();
}

void Image::setTextureFromURL(std::string url) {
    _fromURL = true;

    if (window()) {
        _textureHandle = loadTextureResource(url);
    }

    _resource = std::move(url);
    invalidateRenderCache();
}

void Image::setTextureColor(Color color) {
    _color = color;
    invalidateRenderCache();
}

void Image::setTextureDistanceField(double edge) {
    _distanceFieldEdge = edge;
    invalidateRenderCache();
}

void Image::render() {
    if (_textureHandle && _textureHandle->isLoaded()) {
        auto shader = textureShader();

        if (_distanceFieldEdge) {
            auto dfShader = distanceFieldShader();
            dfShader->setEdge(*_distanceFieldEdge);
            shader = dfShader;
        }

        shader->setColor(_color ? *_color : Color{1, 1, 1, 1});
        shader->drawScaledFit(*_textureHandle, 0, 0, bounds().width, bounds().height);
        shader->flush();
    }
}

void Image::windowChanged() {
    if (window() && !_resource.empty()) {
        _textureHandle = _fromURL ? loadTextureFromURL(_resource) : loadTextureResource(_resource);
    }
}

}}}
