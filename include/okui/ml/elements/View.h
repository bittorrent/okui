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
    class ElementBase : public ElementInterface {
    public:
        virtual void initialize(const Context& context, const pugi::xml_node& xml) override;
        virtual void update(const Context& context) override;

        virtual const char* id() const override;
        virtual ElementInterface* descendantWithId(stdts::string_view id) const override;

        virtual void setAttribute(const Context& context, stdts::string_view name, stdts::string_view value) {}
        virtual void setText(const Context& context, stdts::string_view text) {}

    private:
        std::unordered_map<std::string, std::string> _attributes;
        std::string _text;
        std::vector<std::unique_ptr<ElementInterface>> _children;
    };

    template <typename ViewType>
    class Element : public ElementBase {
    public:
        virtual void setAttribute(const Context& context, stdts::string_view name, stdts::string_view value) override {
            if (scraps::CaseInsensitiveEquals(name, "background-color")) {
                _view.setBackgroundColor(ParseColor(value).value_or(Color::kTransparentBlack));
            } else if (scraps::CaseInsensitiveEquals(name, "x")) {
                _view.attributes.x = std::string(value);
                _view.layout();
            } else if (scraps::CaseInsensitiveEquals(name, "y")) {
                _view.attributes.y = std::string(value);
                _view.layout();
            } else if (scraps::CaseInsensitiveEquals(name, "width")) {
                _view.attributes.width = std::string(value);
                _view.layout();
            } else if (scraps::CaseInsensitiveEquals(name, "height")) {
                _view.attributes.height = std::string(value);
                _view.layout();
            } else if (scraps::CaseInsensitiveEquals(name, "visible")) {
                _view.setIsVisible(ParseBoolean(value).value_or(true));
            } else if (scraps::CaseInsensitiveEquals(name, "preferred-focus")) {
                if (auto element = descendantWithId(value)) {
                    _view.setPreferredFocus(element->view());
                }
            } else if (scraps::CaseInsensitiveEquals(name, "opacity")) {
                _view.setOpacity(ParseNumber(value).value_or(1.0));
            } else if (scraps::CaseInsensitiveEquals(name, "tint-color")) {
                _view.setTintColor(ParseColor(value).value_or(Color::kWhite));
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
