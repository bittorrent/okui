#pragma once

#include <okui/config.h>

#include <okui/ml/parsing.h>
#include <okui/ml/elements/View.h>
#include <okui/views/TextView.h>

namespace okui {
namespace ml {
namespace elements {

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
                _view.setFont(value.to_string() + ".png", value.to_string() + ".fnt");
            } else if (name == "horizontal-alignment") {
                if (value == "center") {
                    _view.setAlignment(views::TextView::Style::HorizontalAlignment::kCenter);
                } else if (value == "left") {
                    _view.setAlignment(views::TextView::Style::HorizontalAlignment::kLeft);
                } else if (value == "right") {
                    _view.setAlignment(views::TextView::Style::HorizontalAlignment::kRight);
                }
            } else if (name == "vertical-alignment") {
                if (value == "center") {
                    _view.setAlignment(views::TextView::Style::VerticalAlignment::kCenter);
                } else if (value == "bottom") {
                    _view.setAlignment(views::TextView::Style::VerticalAlignment::kBottom);
                } else if (value == "top") {
                    _view.setAlignment(views::TextView::Style::VerticalAlignment::kTop);
                }
            } else if (name == "overflow-behavior") {
                if (value == "none") {
                    _view.setOverflowBehavior(views::TextView::Style::OverflowBehavior::kNone);
                } else if (value == "wrap") {
                    _view.setOverflowBehavior(views::TextView::Style::OverflowBehavior::kWrap);
                } else if (value == "ellipses") {
                    _view.setOverflowBehavior(views::TextView::Style::OverflowBehavior::kEllipses);
                } else if (value == "shrink") {
                    _view.setOverflowBehavior(views::TextView::Style::OverflowBehavior::kShrink);
                }
            } else if (name == "letter-spacing") {
                _view.setLetterSpacing(ParseNumber(value).value_or(0.0));
            } else if (name == "text-size") {
                _view.setTextSize(ParseNumber(value).value_or(0.0));
            } else if (name == "text-color") {
                _view.setTextColor(ParseColor(value).value_or(Color::kBlack));
            } else {
                elements::View::Element<views::TextView>::setAttribute(name, value);
            }
        }

        virtual void setText(stdts::string_view text) override {
            _view.setText(text);
        }
    };
};

}}} // namespace okui::ml::elements
