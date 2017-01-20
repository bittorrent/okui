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
#pragma once

#include <okui/config.h>

#include <okui/views/Button.h>
#include <okui/views/ImageView.h>

#include <unordered_map>

namespace okui::views {

class ImageButton : public Button<View> {
public:
    TextureHandle& texture(State state = State::kNormal) { return _imageViews[state].texture(); }
    ImageView& image(State state = State::kNormal) { return _imageViews[state]; }

    void setTexture(std::string texture, State state = State::kNormal);
    void setTexturePlaceholder(std::string placeholder, State state = State::kNormal);
    void setTextureColor(Color color, State state = State::kNormal);
    void setTextureDistanceField(double edge = 0.5, State state = State::kNormal);
    void setDrawMode(ImageView::TextureDrawMode drawMode, State state = State::kNormal);

    void load();
    void unload();

    virtual void layout() override;
    virtual void stateChanged() override;

private:
    ImageView& _stateImageView(State state);

    std::unordered_map<State, ImageView> _imageViews;
    Color                                _normalColor = Color::kWhite;
};

} // namespace okui::views
