#pragma once

#include <okui/config.h>

#include <okui/ml/elements/View.h>
#include <okui/views/ImageView.h>

namespace okui {
namespace ml {
namespace elements {

class Image : public View {
public:
    virtual std::unique_ptr<ElementInterface> create() const override {
        return std::make_unique<Element>();
    }

protected:
    class Element : public View::Element<views::ImageView> {
    public:
        virtual void setAttribute(stdts::string_view name, stdts::string_view value) override {
            if (name == "texture") {
                _view.setTexture(value.to_string());
            } else if (name == "texture-placeholder") {
                _view.setTexturePlaceholder(value.to_string());
            } else {
                elements::View::Element<views::ImageView>::setAttribute(name, value);
            }
        }
    };
};

}}} // namespace okui::ml::elements
