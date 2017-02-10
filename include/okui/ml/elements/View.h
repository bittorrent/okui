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
#include <okui/ml/ElementInterface.h>
#include <okui/ml/ElementTypeInterface.h>

#include <okui/View.h>

#include <unordered_map>
#include <vector>

namespace okui::ml::elements {

class View : public ElementTypeInterface {
public:
    virtual std::unique_ptr<ElementInterface> create() const override {
        return std::make_unique<Element<::okui::View>>();
    }

protected:
    class ElementBase : public ElementInterface, private ElementInterface::StateMachineDelegate {
    public:
        virtual void initialize(const Context* context, const pugi::xml_node& xml) override;
        virtual void update() override;

        const Context* context() const { return _context; }

        virtual const char* id() const override;
        virtual ElementInterface* descendantWithId(stdts::string_view id) const override;

        /**
        * Override this for attributes that cannot be interpolated.
        */
        virtual void setAttribute(stdts::string_view name, stdts::string_view value) {
            setAttribute(name, {ValueComponent{value, 1.0}});
        }

        using ElementInterface::StateMachineDelegate::ValueComponent;

        /**
        * Override this for attributes that can be interpolated.
        */
        virtual void setAttribute(stdts::string_view name, std::vector<ValueComponent> components) {}

        virtual void setText(stdts::string_view text) {}

        /**
        * Utility methods commonly used by subclasses.
        */
        stdts::optional<Color> SumColorComponents(const std::vector<ValueComponent>& components);
        stdts::optional<std::string> SumExpressionComponents(const std::vector<ValueComponent>& components);
        stdts::optional<double> SumNumberComponents(const std::vector<ValueComponent>& components);

        std::vector<StateMachineInterface*> findStateMachinesWithState(stdts::string_view id);
        std::vector<StateMachineInterface*> findStateMachinesWithTrigger(stdts::string_view id);

    private:
        virtual void apply(stdts::string_view attribute, std::vector<ValueComponent> components) override;

        const Context*                                 _context = nullptr;
        std::unordered_map<std::string, std::string>   _attributes;
        std::string                                    _text;
        std::vector<std::unique_ptr<ElementInterface>> _children;
        std::vector<StateMachineInterface*>            _stateMachines;
    };

    template <typename ViewType>
    class Element : public ElementBase, public ViewType {
    public:
        virtual void initialize(const Context* context, const pugi::xml_node& xml) override {
            ElementBase::initialize(context, xml);

            auto focused = ViewType::isFocus() ? "focused" : "unfocused";

            for (auto& stateMachine : findStateMachinesWithTrigger("focused")) {
                stateMachine->reset(focused);
            }

            for (auto& stateMachine : findStateMachinesWithTrigger("unfocused")) {
                stateMachine->reset(focused);
            }
        }

        virtual void setAttribute(stdts::string_view name, stdts::string_view value) override {
            if (name == "visible") {
                this->setIsVisible(ParseBoolean(value).value_or(true));
            } else if (name == "preferred-focus") {
                if (auto element = descendantWithId(value)) {
                    this->setPreferredFocus(element->view());
                }
            } else if (name == "can-become-direct-focus") {
                _canBecomeDirectFocus = ParseBoolean(value);
            } else if (name == "intercepts-interactions") {
                this->setInterceptsInteractions(ParseBoolean(value).value_or(true));
            } else if (name == "children-intercept-interactions") {
                this->setChildrenInterceptInteractions(ParseBoolean(value).value_or(true));
            } else {
                ElementBase::setAttribute(name, value);
            }
        }

        virtual void setAttribute(stdts::string_view name, std::vector<ValueComponent> components) override {
            if (name == "background-color") {
                this->setBackgroundColor(SumColorComponents(components).value_or(Color::kTransparentBlack));
            } else if (name == "x") {
                attributes().x = SumExpressionComponents(components);
                setNeedsLayout();
            } else if (name == "y") {
                attributes().y = SumExpressionComponents(components);
                setNeedsLayout();
            } else if (name == "width") {
                attributes().width = SumExpressionComponents(components);
                setNeedsLayout();
            } else if (name == "height") {
                attributes().height = SumExpressionComponents(components);
                setNeedsLayout();
            } else if (name == "opacity") {
                this->setOpacity(SumNumberComponents(components).value_or(1.0));
            } else if (name == "tint-color") {
                this->setTintColor(SumColorComponents(components).value_or(Color::kWhite));
            } else if (name == "scale") {
                this->setScale(SumNumberComponents(components).value_or(1.0));
            } else {
                ElementBase::setAttribute(name, std::move(components));
            }
        }

        virtual void update() override {
            ElementBase::update();
            layoutIfNeeded();
        }

        virtual ::okui::View* view() override { return this; }

        void layoutIfNeeded() {
            if (_needsLayout) {
                layout();
                ViewType::removeUpdateHook("ml::Elements::View::View::setNeedsLayout");
            }
        }

        void setNeedsLayout() {
            if (_needsLayout) { return; }
            _needsLayout = true;
            ViewType::addUpdateHook("ml::Elements::View::View::setNeedsLayout", [this] {
                layoutIfNeeded();
            });
        }

        struct Attributes {
            stdts::optional<std::string> x, y, width, height;
        };

        Attributes& attributes() { return _attributes; }

        virtual void layout() override {
            _needsLayout = false;

            if (this->superview()) {
                if (_attributes.x || _attributes.y || _attributes.width || _attributes.height) {
                    std::unordered_map<std::string, double> xUnits, yUnits;
                    xUnits["%pw"] = yUnits["%pw"] = xUnits["%"] = this->superview()->bounds().width / 100.0;
                    xUnits["%ph"] = yUnits["%ph"] = yUnits["%"] = this->superview()->bounds().height / 100.0;

                    this->setBounds(
                        _attributes.x ? ParseNumber(*_attributes.x, xUnits).value_or(0.0) : this->bounds().x,
                        _attributes.y ? ParseNumber(*_attributes.y, yUnits).value_or(0.0) : this->bounds().y,
                        _attributes.width ? ParseNumber(*_attributes.width, xUnits).value_or(0.0) : this->bounds().width,
                        _attributes.height ? ParseNumber(*_attributes.height, yUnits).value_or(0.0) : this->bounds().height
                    );
                }

                for (auto& subview : this->subviews()) {
                    subview->layout();
                }
            }

            ViewType::layout();
        }

        virtual void focusGained() override {
            for (auto& stateMachine : findStateMachinesWithTrigger("focused")) {
                stateMachine->setState("focused");
            }

            ViewType::focusGained();
        }

        virtual void focusLost() override {
            for (auto& stateMachine : findStateMachinesWithTrigger("unfocused")) {
                stateMachine->setState("unfocused");
            }

            ViewType::focusLost();
        }

        virtual bool canBecomeDirectFocus() override {
            if (_canBecomeDirectFocus) {
                return *_canBecomeDirectFocus;
            }

            return ViewType::canBecomeDirectFocus();
        }

    private:
        Attributes            _attributes;
        bool                  _needsLayout = false;
        stdts::optional<bool> _canBecomeDirectFocus;
    };
};

} // namespace okui::ml::elements
