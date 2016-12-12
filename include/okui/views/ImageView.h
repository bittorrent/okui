#pragma once

#include <okui/config.h>

#include <okui/Color.h>
#include <okui/View.h>

#include <stdts/optional.h>

namespace okui::views {

class ImageView : public View {
public:
    enum class TextureDrawMode {
        kFill,
        kFit
    };

    /**
    * Sets the texture to the given URL or resource.
    */
    void setTexture(std::string texture);

    /**
    * If the texture was set to a URL, a placeholder resource may be displayed until the texture is loaded.
    */
    void setTexturePlaceholder(std::string placeholder);

    void setTextureColor(Color color);
    void setTextureDistanceField(stdts::optional<double> edge = 0.5);

    void setDrawMode(TextureDrawMode drawMode) { _drawMode = drawMode; }

    void setRotation(double r) { _rotation = r; }

    TextureHandle& texture() { return _texture; }
    const Color& textureColor() const { return _color; }

    void load();
    void unload();

    virtual void render() override;
    virtual void windowChanged() override;
    virtual void willAppear() override;

private:
    TextureHandle           _texture;
    TextureHandle           _placeholderTexture;
    std::string             _resource;
    std::string             _placeholderResource;
    bool                    _fromURL = false;
    Color                   _color = Color::kWhite;
    stdts::optional<double> _distanceFieldEdge;
    TextureDrawMode         _drawMode = TextureDrawMode::kFit;
    double                  _rotation = 0;
};

} // namespace okui::views
