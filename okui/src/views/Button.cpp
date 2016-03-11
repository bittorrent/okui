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
    _stateImageView(state).setTextureResource(std::move(resource));
}

void Button::setTextureFromURL(std::string url, State state) {
    _stateImageView(state).setTextureFromURL(std::move(url));
}

void Button::setTextureColor(Color color, State state) {
    _stateImageView(state).setTextureColor(color);
    if (state == State::kNormal) { _normalColor = color; }
}

void Button::setTextureDistanceField(double edge, State state) {
    _stateImageView(state).setTextureDistanceField(edge);
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
    for (auto& i : _imageViews) {
        i.second.setBoundsRelative(0, 0, 1, 1);
    }
}

ImageView& Button::_stateImageView(State state) {
    auto imageIt = _imageViews.find(state);

    if (imageIt == _imageViews.end()){
        auto& image = _imageViews[state];
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
    for (auto& i : _imageViews) {
        i.second.setIsVisible(i.first == state);
    }

    auto normalImageView = _imageViews.find(State::kNormal);

    switch (state) {
        case State::kNormal:
            if (normalImageView != _imageViews.end()) {
                normalImageView->second.setTextureColor(_normalColor);
            }
            break;
        case State::kDepressed:
            if (!_imageViews.count(State::kDepressed) && normalImageView != _imageViews.end()) {
                normalImageView->second.setIsVisible(true);
                normalImageView->second.setTextureColor(DepressedColor(_normalColor));
            }
            break;
        default:
            break;
    }
}

}}}
