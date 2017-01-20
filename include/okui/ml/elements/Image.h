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

#include <okui/ml/elements/View.h>
#include <okui/views/ImageView.h>

namespace okui::ml::elements {

class Image : public View {
public:
    virtual std::unique_ptr<ElementInterface> create() const override {
        return std::make_unique<Element>();
    }

protected:
    class Element : public View::Element<views::ImageView> {
    public:
        virtual void setAttribute(stdts::string_view name, stdts::string_view value) override {
            if (name == "texture") {
                _view.setTexture(std::string(value));
            } else if (name == "texture-placeholder") {
                _view.setTexturePlaceholder(std::string(value));
            } else if (name == "texture-distance-field") {
                _view.setTextureDistanceField(ParseNumber(value));
            } else if (name == "draw-mode") {
                if (value == "fill") {
                    _view.setDrawMode(views::ImageView::TextureDrawMode::kFill);
                } else if (value == "fit") {
                    _view.setDrawMode(views::ImageView::TextureDrawMode::kFit);
                }
            } else {
                elements::View::Element<views::ImageView>::setAttribute(name, value);
            }
        }

        virtual void setAttribute(stdts::string_view name, std::vector<ValueComponent> components) override {
            if (name == "rotation") {
                _view.setRotation(SumNumberComponents(components).value_or(0.0));
            } else {
                elements::View::Element<views::ImageView>::setAttribute(name, std::move(components));
            }
        }
    };
};

} // namespace okui::ml::elements
