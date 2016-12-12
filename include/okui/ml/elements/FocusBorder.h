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
                _view.setColor(SumColorComponents(components).value_or(Color::kWhite));
            } else if (name == "thickness") {
                _view.setThickness(SumNumberComponents(components).value_or(1.0));
            } else {
                elements::View::Element<views::FocusBorder<::okui::View>>::setAttribute(name, std::move(components));
            }
        }
    };
};

} // namespace okui::ml::elements
