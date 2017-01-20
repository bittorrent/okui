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
#include <okui/views/ImageButton.h>

#include <okui/Application.h>

namespace okui::views {

namespace {

constexpr auto kDepressedBrightness = 0.8;

constexpr auto DepressedColor(Color c) {
    auto color = (c * kDepressedBrightness).withAlphaF(c.alphaF());
    return color;
}

} // anonymous namespace

void ImageButton::setTexture(std::string texture, State state) {
    _stateImageView(state).setTexture(std::move(texture));
}

void ImageButton::setTexturePlaceholder(std::string placeholder, State state) {
    _stateImageView(state).setTexturePlaceholder(std::move(placeholder));
}

void ImageButton::setTextureColor(Color color, State state) {
    _stateImageView(state).setTextureColor(color);
    if (state == State::kNormal) { _normalColor = color; }
}

void ImageButton::setTextureDistanceField(double edge, State state) {
    _stateImageView(state).setTextureDistanceField(edge);
}

void ImageButton::setDrawMode(ImageView::TextureDrawMode drawMode, State state) {
    _stateImageView(state).setDrawMode(drawMode);
}

void ImageButton::load() {
    for (auto& i : _imageViews) {
        i.second.load();
    }
}

void ImageButton::unload() {
    for (auto& i : _imageViews) {
        i.second.unload();
    }
}

void ImageButton::layout() {
    for (auto& i : _imageViews) {
        i.second.setBoundsRelative(0, 0, 1, 1);
    }
}

ImageView& ImageButton::_stateImageView(State state) {
    auto imageIt = _imageViews.find(state);

    if (imageIt == _imageViews.end()){
        auto& image = _imageViews[state];
        addHiddenSubview(&image);
        image.setBoundsRelative(0, 0, 1, 1);
        if (this->state() == state) {
            stateChanged();
        }

        return image;
    }

    return imageIt->second;
}

void ImageButton::stateChanged() {
    for (auto& i : _imageViews) {
        i.second.setIsVisible(i.first == state());
    }

    auto normalImageView = _imageViews.find(State::kNormal);

    switch (state()) {
        case State::kNormal:
            if (normalImageView != _imageViews.end()) {
                normalImageView->second.setTextureColor(_normalColor);
            }
            break;
        case State::kDepressed:
            if (!_imageViews.count(State::kDepressed) && normalImageView != _imageViews.end()) {
                normalImageView->second.setIsVisible(true);
                normalImageView->second.setTextureColor(DepressedColor(_normalColor));
            }
            break;
        default:
            break;
    }
}

} // namespace okui::views
