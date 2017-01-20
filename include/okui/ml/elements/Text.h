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

#include <okui/ml/parsing.h>
#include <okui/ml/elements/View.h>
#include <okui/views/TextView.h>

namespace okui::ml::elements {

class Text : public View {
public:
    virtual std::unique_ptr<ElementInterface> create() const override {
        return std::make_unique<Element>();
    }

protected:
    class Element : public View::Element<views::TextView> {
    public:
        virtual void setAttribute(stdts::string_view name, stdts::string_view value) override {
            if (name == "font") {
                _view.setFont(std::string(value) + ".png", std::string(value) + ".fnt");
            } else if (name == "horizontal-alignment") {
                if (value == "center") {
                    _view.setAlignment(views::TextView::Style::HorizontalAlignment::kCenter);
                } else if (value == "left") {
                    _view.setAlignment(views::TextView::Style::HorizontalAlignment::kLeft);
                } else if (value == "right") {
                    _view.setAlignment(views::TextView::Style::HorizontalAlignment::kRight);
                }
            } else if (name == "vertical-alignment") {
                if (value == "middle") {
                    _view.setAlignment(views::TextView::Style::VerticalAlignment::kMiddle);
                } else if (value == "bottom") {
                    _view.setAlignment(views::TextView::Style::VerticalAlignment::kBottom);
                } else if (value == "top") {
                    _view.setAlignment(views::TextView::Style::VerticalAlignment::kTop);
                }
            } else {
                elements::View::Element<views::TextView>::setAttribute(name, value);
            }
        }

        virtual void setAttribute(stdts::string_view name, std::vector<ValueComponent> components) override {
            if (name == "letter-spacing") {
                _view.setLetterSpacing(SumNumberComponents(components).value_or(0.0));
            } else if (name == "text-size") {
                _view.setTextSize(SumNumberComponents(components).value_or(0.0));
            } else if (name == "text-color") {
                _view.setTextColor(SumColorComponents(components).value_or(Color::kBlack));
            } else if (name == "weight") {
                _view.setWeight(SumNumberComponents(components).value_or(100.0));
            } else {
                elements::View::Element<views::TextView>::setAttribute(name, components);
            }
        }

        virtual void setText(stdts::string_view text) override {
            _view.setText(text);
        }
    };
};

} // namespace okui::ml::elements
