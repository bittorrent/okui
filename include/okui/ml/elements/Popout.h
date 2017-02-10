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
#include <okui/views/PopoutFocus.h>

namespace okui::ml::elements {

class Popout : public View {
public:
    virtual std::unique_ptr<ElementInterface> create() const override {
        return std::make_unique<Element>();
    }

protected:
    class ViewImplementation : public views::PopoutFocus<::okui::View> {
    public:
        virtual void layout() override {
            if (this->superview()) {
                std::unordered_map<std::string, double> xUnits, yUnits;
                xUnits["%pw"] = yUnits["%pw"] = xUnits["%"] = this->superview()->bounds().width / 100.0;
                xUnits["%ph"] = yUnits["%ph"] = yUnits["%"] = this->superview()->bounds().height / 100.0;

                auto width = popoutAttributes.width ? ParseNumber(*popoutAttributes.width, xUnits).value_or(0.0) : size().x;
                auto height = popoutAttributes.height ? ParseNumber(*popoutAttributes.height, yUnits).value_or(0.0) : size().y;

                auto anchorX = ParseNumber(popoutAttributes.anchorX.value_or("50%")).value_or(0.5);
                auto anchorY = ParseNumber(popoutAttributes.anchorY.value_or("50%")).value_or(0.5);

                this->setAnchor(
                    {
                        popoutAttributes.x
                            ? ParseNumber(*popoutAttributes.x, xUnits).value_or(0.0) + anchorX * width
                            : ParseNumber(popoutAttributes.anchorParentX.value_or("50%"), xUnits).value_or(0.0)
                        ,
                        popoutAttributes.y
                            ? ParseNumber(*popoutAttributes.y, yUnits).value_or(0.0) + anchorY * height
                            : ParseNumber(popoutAttributes.anchorParentY.value_or("50%"), yUnits).value_or(0.0)
                    },
                    {anchorX, anchorY}
                );

                if (popoutAttributes.width || popoutAttributes.height) {
                    setSize(width, height);
                }
            }

            views::PopoutFocus<::okui::View>::layout();
        }

        struct {
            stdts::optional<std::string> x, y, width, height, anchorX, anchorY, anchorParentX, anchorParentY;
        } popoutAttributes;
    };

    class Element : public View::Element<ViewImplementation> {
    public:
        virtual void setAttribute(stdts::string_view name, std::vector<ValueComponent> components) override {
            if (name == "x") {
                popoutAttributes.x = SumExpressionComponents(components);
                popoutAttributes.anchorParentX = {};
            } else if (name == "y") {
                popoutAttributes.y = SumExpressionComponents(components);
                popoutAttributes.anchorParentY = {};
            } else if (name == "width") {
                popoutAttributes.width = SumExpressionComponents(components);
            } else if (name == "height") {
                popoutAttributes.height = SumExpressionComponents(components);
            } else if (name == "anchor-x") {
                popoutAttributes.anchorX = SumExpressionComponents(components);
            } else if (name == "anchor-y") {
                popoutAttributes.anchorY = SumExpressionComponents(components);
            } else if (name == "anchor-parent-x") {
                popoutAttributes.x = {};
                popoutAttributes.anchorParentX = SumExpressionComponents(components);
            } else if (name == "anchor-parent-y") {
                popoutAttributes.y = {};
                popoutAttributes.anchorParentY = SumExpressionComponents(components);
            } else {
                if (name == "scaling") {
                    if (auto scaling = SumNumberComponents(components)) {
                        setScaling(*scaling);
                    }
                } else {
                    elements::View::Element<ViewImplementation>::setAttribute(name, components);
                }
                return;
            }
            setNeedsLayout();
        }
    };
};

} // namespace okui::ml::elements
