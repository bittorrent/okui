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

    private:
        const Context* _context = nullptr;
        std::unordered_map<std::string, std::string> _attributes;
        std::string _text;
        std::vector<std::unique_ptr<ElementInterface>> _children;
        std::vector<StateMachineInterface*> _stateMachines;

        virtual void apply(stdts::string_view attribute, std::vector<ValueComponent> components) override;
    };

    template <typename ViewType>
    class Element : public ElementBase {
    public:
        virtual void setAttribute(stdts::string_view name, stdts::string_view value) override {
            if (name == "visible") {
                _view.setIsVisible(ParseBoolean(value).value_or(true));
            } else if (name == "preferred-focus") {
                if (auto element = descendantWithId(value)) {
                    _view.setPreferredFocus(element->view());
                }
            } else if (name == "intercepts-interactions") {
                _view.setInterceptsInteractions(ParseBoolean(value).value_or(true));
            } else if (name == "children-intercept-interactions") {
                _view.setChildrenInterceptInteractions(ParseBoolean(value).value_or(true));
            } else {
                ElementBase::setAttribute(name, value);
            }
        }

        virtual void setAttribute(stdts::string_view name, std::vector<ValueComponent> components) override {
            if (name == "background-color") {
                _view.setBackgroundColor(SumColorComponents(components).value_or(Color::kTransparentBlack));
            } else if (name == "x") {
                _view.attributes.x = SumExpressionComponents(components);
                _view.layout();
            } else if (name == "y") {
                _view.attributes.y = SumExpressionComponents(components);
                _view.layout();
            } else if (name == "width") {
                _view.attributes.width = SumExpressionComponents(components);
                _view.layout();
            } else if (name == "height") {
                _view.attributes.height = SumExpressionComponents(components);
                _view.layout();
            } else if (name == "opacity") {
                _view.setOpacity(SumNumberComponents(components).value_or(1.0));
            } else if (name == "tint-color") {
                _view.setTintColor(SumColorComponents(components).value_or(Color::kWhite));
            } else if (name == "scale") {
                _view.setScale(SumNumberComponents(components).value_or(1.0));
            } else {
                ElementBase::setAttribute(name, std::move(components));
            }
        }

        virtual ::okui::View* view() override { return &_view; }

    protected:
        class View : public ViewType {
        public:
            virtual void layout() override {
                if (this->superview()) {
                    if (attributes.x || attributes.y || attributes.width || attributes.height) {
                        std::unordered_map<std::string, double> xUnits, yUnits;
                        xUnits["%pw"] = yUnits["%pw"] = xUnits["%"] = this->superview()->bounds().width / 100.0;
                        xUnits["%ph"] = yUnits["%ph"] = yUnits["%"] = this->superview()->bounds().height / 100.0;

                        this->setBounds(
                            attributes.x ? ParseNumber(*attributes.x, xUnits).value_or(0.0) : this->bounds().x,
                            attributes.y ? ParseNumber(*attributes.y, yUnits).value_or(0.0) : this->bounds().y,
                            attributes.width ? ParseNumber(*attributes.width, xUnits).value_or(0.0) : this->bounds().width,
                            attributes.height ? ParseNumber(*attributes.height, yUnits).value_or(0.0) : this->bounds().height
                        );
                    }

                    for (auto& subview : this->subviews()) {
                        subview->layout();
                    }
                }

                ViewType::layout();
            }

            struct {
                stdts::optional<std::string> x, y, width, height;
            } attributes;
        } _view;
    };
};

} // namespace okui::ml::elements
