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
#include <okui/views/FocusBorder.h>

namespace okui::ml::elements {

class FocusBorder : public View {
public:
    virtual std::unique_ptr<ElementInterface> create() const override {
        return std::make_unique<Element>();
    }

protected:
    class Element : public View::Element<views::FocusBorder<::okui::View>> {
    public:
        virtual void setAttribute(stdts::string_view name, std::vector<ValueComponent> components) override {
            if (name == "color") {
                setColor(SumColorComponents(components).value_or(Color::kWhite));
            } else if (name == "thickness") {
                setThickness(SumNumberComponents(components).value_or(1.0));
            } else {
                elements::View::Element<views::FocusBorder<::okui::View>>::setAttribute(name, std::move(components));
            }
        }
    };
};

} // namespace okui::ml::elements
