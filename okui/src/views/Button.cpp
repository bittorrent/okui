#include "onair/okui/views/Button.h"

#include "onair/okui/Application.h"

namespace onair {
namespace okui {
namespace views {

namespace {

constexpr auto kDepressedBrightness = 0.8;

constexpr auto DepressedColor(Color c) {
    auto color = c * kDepressedBrightness;
    color.a = c.a;
    return color;
}

}

void Button::setAction(std::function<void()> action) {
    _action = std::move(action);
}

void Button::setAction(Command command, CommandContext context) {
    _action = [=] { application()->command(command, context); };
}

void Button::setTextureResource(std::string resource, State state) {
    _stateImage(state).setTextureResource(std::move(resource));
}

void Button::setTextureFromURL(std::string url, State state) {
    _stateImage(state).setTextureFromURL(std::move(url));
}

void Button::setTextureColor(Color color, State state) {
    _stateImage(state).setTextureColor(color);
    if (state == State::kNormal) { _normalColor = color; }
}

void Button::setTextureDistanceField(double edge, State state) {
    _stateImage(state).setTextureDistanceField(edge);
}

void Button::press() {
    if (_action) { _action(); }
}

void Button::buttonDown(const okui::Controller& controller, size_t button) {
    if (_action) { _action(); }
}

void Button::mouseDown(MouseButton button, double x, double y) {
    _changeState(State::kDepressed);
    _mouseDown = true;
}

void Button::mouseUp(MouseButton button, double startX, double startY, double x, double y) {
    if (_mouseDown) {
        _mouseDown = false;
        _changeState(State::kNormal);
        if (_action) { _action(); }
    }
}

void Button::mouseExit() {
    _changeState(State::kNormal);
    _mouseDown = false;
}

void Button::keyDown(KeyCode key, KeyModifiers mod, bool repeat) {
    if (key == KeyCode::kSpace || key == KeyCode::kReturn) {
        if (_action) { _action(); }
        return;
    }

    onair::okui::View::keyDown(key, mod, repeat);
}

void Button::layout() {
    for (auto& i : _images) {
        i.second.setBoundsRelative(0, 0, 1, 1);
    }
}

Image& Button::_stateImage(State state) {
    auto imageIt = _images.find(state);

    if (imageIt == _images.end()){
        auto& image = _images[state];
        addHiddenSubview(&image);
        image.setBoundsRelative(0, 0, 1, 1);
        if (_state == state) {
            _changeState(state);
        }

        return image;
    }

    return imageIt->second;
}

void Button::_changeState(State state) {
    for (auto& i : _images) {
        i.second.setIsVisible(i.first == state);
    }

    auto normalImage = _images.find(State::kNormal);

    switch (state) {
        case State::kNormal:
            if (normalImage != _images.end()) {
                normalImage->second.setTextureColor(_normalColor);
            }
            break;
        case State::kDepressed:
            if (!_images.count(State::kDepressed) && normalImage != _images.end()) {
                normalImage->second.setIsVisible(true);
                normalImage->second.setTextureColor(DepressedColor(_normalColor));
            }
            break;
        default:
            break;
    }
}

}}}
