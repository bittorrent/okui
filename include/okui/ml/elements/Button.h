#pragma once

#include <okui/config.h>

#include <okui/ml/Context.h>
#include <okui/ml/elements/View.h>
#include <okui/views/Button.h>

namespace okui {
namespace ml {
namespace elements {

class Button : public View {
public:
    virtual std::unique_ptr<ElementInterface> create() const override {
        return std::make_unique<Element>();
    }

protected:
    class Element : public View::Element<views::Button<::okui::View>> {
    public:
        virtual void setAttribute(const Context& context, stdts::string_view name, stdts::string_view value) override {
            if (scraps::CaseInsensitiveEquals(name, "command")) {
                _command = strtoull(std::string{value}.c_str(), nullptr, 0);
                if (!_command) {
                    _command = scraps::FNV1A64(value.data(), value.size());
                }
                _view.setAction(_command, _commandContext);
            } else if (scraps::CaseInsensitiveEquals(name, "command-context")) {
                auto cc = context.get(std::string{value});
                _commandContext = cc ? *cc : CommandContext{};
                _view.setAction(_command, _commandContext);
            } else {
                elements::View::Element<views::Button<::okui::View>>::setAttribute(context, name, value);
            }
        }

    private:
        Command _command;
        CommandContext _commandContext;
    };
};

}}} // namespace okui::ml::elements
