#pragma once

#include <okui/config.h>

#include <okui/ml/elements/View.h>
#include <okui/views/PopoutFocus.h>

namespace okui {
namespace ml {
namespace elements {

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
        virtual void setAttribute(const Context& context, stdts::string_view name, stdts::string_view value) override {
            if (name == "x") {
                _view.popoutAttributes.x = std::string{value};
                _view.popoutAttributes.anchorParentX = {};
            } else if (name == "y") {
                _view.popoutAttributes.y = std::string{value};
                _view.popoutAttributes.anchorParentY = {};
            } else if (name == "width") {
                _view.popoutAttributes.width = std::string{value};
            } else if (name == "height") {
                _view.popoutAttributes.height = std::string{value};
            } else if (name == "anchor-x") {
                _view.popoutAttributes.anchorX = std::string{value};
            } else if (name == "anchor-y") {
                _view.popoutAttributes.anchorY = std::string{value};
            } else if (name == "anchor-parent-x") {
                _view.popoutAttributes.x = {};
                _view.popoutAttributes.anchorParentX = std::string{value};
            } else if (name == "anchor-parent-y") {
                _view.popoutAttributes.y = {};
                _view.popoutAttributes.anchorParentY = std::string{value};
            } else {
                if (name == "scaling") {
                    if (auto scaling = ParseNumber(value)) {
                        _view.setScaling(*scaling);
                    }
                } else {
                    elements::View::Element<ViewImplementation>::setAttribute(context, name, value);
                }
                return;
            }
            _view.layout();
        }
    };
};

}}} // namespace okui::ml::elements
