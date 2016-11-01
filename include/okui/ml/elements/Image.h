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
        virtual void setAttribute(const Context& context, stdts::string_view name, stdts::string_view value) override {
            if (scraps::CaseInsensitiveEquals(name, "texture")) {
                _view.setTexture(std::string{value});
            } else if (scraps::CaseInsensitiveEquals(name, "texture-placeholder")) {
                _view.setTexturePlaceholder(std::string{value});
            } else if (scraps::CaseInsensitiveEquals(name, "texture-distance-field")) {
                _view.setTextureDistanceField(ParseNumber(value));
            } else {
                elements::View::Element<views::ImageView>::setAttribute(context, name, value);
            }
        }
    };
};

}}} // namespace okui::ml::elements
