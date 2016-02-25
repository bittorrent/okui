#include "onair/okui/views/PopoutFocus.h"

namespace onair {
namespace okui {
namespace views {

void PopoutFocus::focusGained() {
    _popoutAnimation.target(1.0, 70ms, interpolation::Exponential::EaseOut);
    _startUpdates();
}

void PopoutFocus::focusLost() {
    _popoutAnimation.target(0.0, 400ms, interpolation::Exponential::EaseOut);
    _startUpdates();
}

void PopoutFocus::setScaling(Point<double> scaling) {
    _scaling = scaling;
    _updateBounds();
}

void PopoutFocus::setAnchor(Point<double> parentPosition, Point<double> selfPercentage) {
    _parentAnchorPosition = parentPosition;
    _selfAnchorPercentage = selfPercentage;
    _updateBounds();
}

void PopoutFocus::setSize(Point<double> size) {
    _size = size;
    _updateBounds();
}

void PopoutFocus::_startUpdates() {
    addUpdateHook("PopoutFocus", [this] {
        _updateBounds();
    });
}

void PopoutFocus::_updateBounds() {
    auto popout = _popoutAnimation.current();
    if (popout <= 0.0) {
        removeUpdateHook("PopoutFocus");
    }
    auto width = _size.x * (1.0 + popout * _scaling.x);
    auto height = _size.y * (1.0 + popout * _scaling.y);
    auto x = _parentAnchorPosition.x - _selfAnchorPercentage.x * width;
    auto y = _parentAnchorPosition.y - _selfAnchorPercentage.y * height;
    auto& tpFocus = touchpadFocus();
    auto xOffset = std::max(-1.0, std::min(1.0, tpFocus.pressure().x / tpFocus.pressureThreshold() * popout * 1.25));
    auto yOffset = std::max(-1.0, std::min(1.0, tpFocus.pressure().y / tpFocus.pressureThreshold() * popout * 1.25));
    x += xOffset * _scaling.x * _size.x * 0.3;
    y += yOffset * _scaling.y * _size.y * 0.3;
    setBounds(x, y, width, height);
}

}}}
