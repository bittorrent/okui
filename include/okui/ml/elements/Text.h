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
        virtual void setAttribute(const Context& context, stdts::string_view name, stdts::string_view value) override {
            if (scraps::CaseInsensitiveEquals(name, "font")) {
                _view.setFont(std::string(value) + ".png", std::string(value) + ".fnt");
            } else if (scraps::CaseInsensitiveEquals(name, "horizontal-alignment")) {
                if (scraps::CaseInsensitiveEquals(value, "center")) {
                    _view.setAlignment(views::TextView::Style::HorizontalAlignment::kCenter);
                } else if (scraps::CaseInsensitiveEquals(value, "left")) {
                    _view.setAlignment(views::TextView::Style::HorizontalAlignment::kLeft);
                } else if (scraps::CaseInsensitiveEquals(value, "right")) {
                    _view.setAlignment(views::TextView::Style::HorizontalAlignment::kRight);
                }
            } else if (scraps::CaseInsensitiveEquals(name, "vertical-alignment")) {
                if (scraps::CaseInsensitiveEquals(value, "center")) {
                    _view.setAlignment(views::TextView::Style::VerticalAlignment::kCenter);
                } else if (scraps::CaseInsensitiveEquals(value, "bottom")) {
                    _view.setAlignment(views::TextView::Style::VerticalAlignment::kBottom);
                } else if (scraps::CaseInsensitiveEquals(value, "top")) {
                    _view.setAlignment(views::TextView::Style::VerticalAlignment::kTop);
                }
            } else if (scraps::CaseInsensitiveEquals(name, "letter-spacing")) {
                _view.setLetterSpacing(ParseNumber(value).value_or(0.0));
            } else if (scraps::CaseInsensitiveEquals(name, "text-size")) {
                _view.setTextSize(ParseNumber(value).value_or(0.0));
            } else if (scraps::CaseInsensitiveEquals(name, "text-color")) {
                _view.setTextColor(ParseColor(value).value_or(Color::kBlack));
            } else {
                elements::View::Element<views::TextView>::setAttribute(context, name, value);
            }
        }

        virtual void setText(const Context& context, stdts::string_view text) override {
            _view.setText(text);
        }
    };
};

} // namespace okui::ml::elements
