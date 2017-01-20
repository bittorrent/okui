/**
* Copyright 2017 BitTorrent Inc.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*    http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/
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
