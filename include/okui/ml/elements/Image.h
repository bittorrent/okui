#pragma once

#include <okui/config.h>

#include <okui/ml/elements/View.h>
#include <okui/views/ImageView.h>

namespace okui::ml::elements {

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
                _view.setTexture(std::string(value));
            } else if (name == "texture-placeholder") {
                _view.setTexturePlaceholder(std::string(value));
            } else if (name == "texture-distance-field") {
                _view.setTextureDistanceField(ParseNumber(value));
            } else if (name == "draw-mode") {
                if (value == "fill") {
                    _view.setDrawMode(views::ImageView::TextureDrawMode::kFill);
                } else if (value == "fit") {
                    _view.setDrawMode(views::ImageView::TextureDrawMode::kFit);
                }
            } else {
                elements::View::Element<views::ImageView>::setAttribute(name, value);
            }
        }
    };
};

} // namespace okui::ml::elements
