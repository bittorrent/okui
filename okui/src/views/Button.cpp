#include "onair/okui/views/Button.h"

namespace onair {
namespace okui {
namespace views {

using namespace onair;

void Button::setTextureResource(std::string resource, State state) {
    auto& texture = _textures[state];
    texture.handle = window() ? loadTextureResource(resource) : nullptr;
    texture.resource = std::move(resource);
    invalidateRenderCache();
}

void Button::setTextureColor(Color color, State state) {
    auto& texture = _textures[state];
    texture.color = std::move(color);
    invalidateRenderCache();
}

void Button::setTextureDistanceField(double edge, State state) {
    auto& texture = _textures[state];
    texture.distanceFieldEdge = edge;
    invalidateRenderCache();
}

void Button::render() {
    auto current = _textures.find(_state);
    auto normal = _textures.find(State::kNormal);

    auto shader = textureShader();

    if (current != _textures.end() && current->second.distanceFieldEdge) {
        auto dfShader = distanceFieldShader();
        dfShader->setEdge(*current->second.distanceFieldEdge);
        shader = dfShader;
    } else if (normal != _textures.end() && normal->second.distanceFieldEdge) {
        auto dfShader = distanceFieldShader();
        dfShader->setEdge(*normal->second.distanceFieldEdge);
        shader = dfShader;
    }

    auto defaultBrightness = _state == State::kDepressed ? 0.8 : 1.0;
    
    if (current != _textures.end() && current->second.color) {
        shader->setColor(current->second.color->r, current->second.color->g, current->second.color->b, current->second.color->a);
    } else if (normal != _textures.end() && normal->second.color) {
        shader->setColor(defaultBrightness * normal->second.color->r, defaultBrightness * normal->second.color->g, defaultBrightness * normal->second.color->b, normal->second.color->a);
    } else {
        shader->setColor(defaultBrightness, defaultBrightness, defaultBrightness, 1.0);
    }

    if (current != _textures.end() && current->second.handle) {
        shader->drawScaledFit(*current->second.handle, 0, 0, bounds().width, bounds().height);
    } else if (normal != _textures.end() && normal->second.handle) {
        shader->drawScaledFit(*normal->second.handle, 0, 0, bounds().width, bounds().height);
    }

    shader->flush();
}

void Button::mouseDown(MouseButton button, int x, int y) {
    _state = State::kDepressed;
    invalidateRenderCache();
}

void Button::mouseUp(MouseButton button, int startX, int startY, int x, int y) {
    if (_state == State::kDepressed) {
        _action();
    }
    _state = State::kNormal;
    invalidateRenderCache();
}

void Button::mouseExit() {
    _state = State::kNormal;
    invalidateRenderCache();
}

void Button::keyDown(KeyCode key, KeyModifiers mod, bool repeat) {
    if (key == KeyCode::kSpace || key == KeyCode::kReturn) {
        _action();
        return;
    }
    
    onair::okui::View::keyDown(key, mod, repeat);
}

void Button::windowChanged() {
    if (window()) {
        for (auto& kv : _textures) {
            if (!kv.second.handle) {
                kv.second.handle = loadTextureResource(kv.second.resource);
            }
        }
    }
}

}}}
