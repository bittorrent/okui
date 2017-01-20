/**
* Copyright 2017 BitTorrent Inc.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*    http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/
#include <okui/views/ImageView.h>

namespace okui::views {

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
            shader->drawScaledFit(**texture, 0, 0, bounds().width, bounds().height, _rotation);
        } else {
            shader->drawScaledFill(**texture, 0, 0, bounds().width, bounds().height, _rotation);
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

} // namespace okui::views
