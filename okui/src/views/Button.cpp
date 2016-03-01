#include "onair/okui/views/Button.h"

namespace onair {
namespace okui {
namespace views {

using namespace onair;

void Button::setTextureResource(std::string resource, State state) {
    _setTexture(std::move(resource), state, false);
}

void Button::setTextureFromURL(std::string url, State state) {
    _setTexture(std::move(url), state, true);
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

const TextureHandle& Button::texture(State state) const {
    auto i = _textures.find(state);
    if (i != _textures.end()) {
        return i->second.handle;
    }

    return nullptr;
}

void Button::press() {
    _action();
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

void Button::buttonDown(const okui::Controller& controller, size_t button) {
    _press();
}

void Button::buttonUp(const okui::Controller& controller, size_t button) {
    _unpress();
}

void Button::mouseDown(MouseButton button, double x, double y) {
    _press();
}

void Button::mouseUp(MouseButton button, double startX, double startY, double x, double y) {
    _unpress();
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
                kv.second.handle = kv.second.fromURL ? loadTextureFromURL(kv.second.resource) : loadTextureResource(kv.second.resource);
            }
        }
    }
}

void Button::_setTexture(std::string resource, State state, bool fromURL) {
    auto& texture = _textures[state];
    texture.handle = nullptr;
    if (window()) {
        texture.handle = fromURL ? loadTextureFromURL(resource) : loadTextureResource(resource);
    }
    texture.resource = std::move(resource);
    texture.fromURL = fromURL;
    invalidateRenderCache();
}

void Button::_press() {
    _state = State::kDepressed;
    invalidateRenderCache();
}

void Button::_unpress() {
    if (_state == State::kDepressed) {
        _action();
    }
    _state = State::kNormal;
    invalidateRenderCache();
}

}}}
