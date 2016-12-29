#pragma once

#include <okui/config.h>

#include <okui/ml/Context.h>
#include <okui/ml/elements/View.h>
#include <okui/views/LabeledPopoutButton.h>

namespace okui::ml::elements {

class LabeledPopoutButton : public View {
public:
    virtual std::unique_ptr<ElementInterface> create() const override {
        return std::make_unique<Element>();
    }

protected:
    class Element : public View::Element<views::LabeledPopoutButton> {
    public:
        virtual void setAttribute(stdts::string_view name, stdts::string_view value) override {
            if (name == "command") {
                _command = strtoull(std::string(value).c_str(), nullptr, 0);
                if (!_command) {
                    _command = scraps::FNV1A64(value.data(), value.size());
                }
                _view.setAction(_command, _commandContext);
            } else if (name == "command-context") {
                auto cc = context()->get(std::string(value));
                _commandContext = cc ? *cc : CommandContext{};
                _view.setAction(_command, _commandContext);
            } else if (name == "icon") {
                _view.setIcon(value.to_string());
            } else if (name == "label-font") {
                _view.setLabelStyle(views::TextView::Style{_view.focusedLabelStyle()}.font(std::string(value) + ".png", std::string(value) + ".fnt"));
            } else {
                elements::View::Element<views::LabeledPopoutButton>::setAttribute(name, value);
            }
        }

        virtual void setAttribute(stdts::string_view name, std::vector<ValueComponent> components) override {
            if (name == "icon-color") {
                if (auto color = SumColorComponents(components)) {
                    _view.setIconColor(*color);
                }
            } else if (name == "label-text-size") {
                _view.setLabelStyle(views::TextView::Style{_view.focusedLabelStyle()}.textSize(SumNumberComponents(components).value_or(0.0)));
            } else if (name == "label-text-color") {
                _view.setLabelStyle(views::TextView::Style{_view.focusedLabelStyle()}.textColor(SumColorComponents(components).value_or(Color::kBlack)));
            } else {
                elements::View::Element<views::LabeledPopoutButton>::setAttribute(name, components);
            }
        }

        virtual void setText(stdts::string_view text) override {
            _view.setLabelText(text.to_string());
        }

    private:
        Command _command;
        CommandContext _commandContext;
    };
};

} // namespace okui::ml::elements
