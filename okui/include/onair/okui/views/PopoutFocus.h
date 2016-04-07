#pragma once

#include "onair/okui/config.h"

#include "onair/okui/Animation.h"
#include "onair/okui/Point.h"

namespace onair {
namespace okui {
namespace views {

/**
* PopoutFocus implements a focusable view that pops out towards the viewer.
*/
template <typename View>
class PopoutFocus : public View {
public:
    virtual bool canBecomeFocus() override { return true; }
    virtual void focusGained() override;
    virtual void focusLost() override;
    virtual void disappeared() override;
    virtual void mouseMovement(double x, double y) override;

    void setScaling(double scaling) { setScaling({scaling, scaling}); }
    void setScaling(double x, double y) { setScaling({x, y}); }
    void setScaling(Point<double> scaling);

    void setAnchor(Point<double> parentPosition, Point<double> selfPercentage);

    void setSize(double width, double height) { setSize({width, height}); }
    void setSize(Point<double> size);

private:
    void _updateBounds();

    Animation<double> _popoutAnimation{0.0};
    Point<double>     _scaling{0.45, 0.45};
    Point<double>     _parentAnchorPosition;
    Point<double>     _selfAnchorPercentage;
    Point<double>     _size;
};

template <typename View>
void PopoutFocus<View>::focusGained() {
    _popoutAnimation.target(1.0, 70ms, interpolation::Exponential::EaseOut);
    this->addUpdateHook("PopoutFocus", [&] { _updateBounds(); });
    View::focusGained();
}

template <typename View>
void PopoutFocus<View>::focusLost() {
    _popoutAnimation.target(0.0, 400ms, interpolation::Exponential::EaseOut);
    this->addUpdateHook("PopoutFocus", [&] { _updateBounds(); });
    View::focusLost();
}

template <typename View>
void PopoutFocus<View>::disappeared() {
    _popoutAnimation.reset(0.0);
    _updateBounds();
    View::disappeared();
}

template <typename View>
void PopoutFocus<View>::mouseMovement(double x, double y) {
    this->focus();
    View::mouseMovement(x, y);
}

template <typename View>
void PopoutFocus<View>::setScaling(Point<double> scaling) {
    _scaling = scaling;
    _updateBounds();
}

template <typename View>
void PopoutFocus<View>::setAnchor(Point<double> parentPosition, Point<double> selfPercentage) {
    _parentAnchorPosition = parentPosition;
    _selfAnchorPercentage = selfPercentage;
    _updateBounds();
}

template <typename View>
void PopoutFocus<View>::setSize(Point<double> size) {
    _size = size;
    _updateBounds();
}

template <typename View>
void PopoutFocus<View>::_updateBounds() {
    auto popout = _popoutAnimation.current();
    if (popout <= 0.0) {
        this->removeUpdateHook("PopoutFocus");
    }
    auto width = _size.x * (1.0 + popout * _scaling.x);
    auto height = _size.y * (1.0 + popout * _scaling.y);
    auto x = _parentAnchorPosition.x - _selfAnchorPercentage.x * width;
    auto y = _parentAnchorPosition.y - _selfAnchorPercentage.y * height;
    auto& tpFocus = this->touchpadFocus();
    auto xOffset = std::max(-1.0, std::min(1.0, tpFocus.pressure().x / tpFocus.pressureThreshold() * popout * 1.25));
    auto yOffset = std::max(-1.0, std::min(1.0, tpFocus.pressure().y / tpFocus.pressureThreshold() * popout * 1.25));
    x += xOffset * _scaling.x * _size.x * 0.3;
    y += yOffset * _scaling.y * _size.y * 0.3;
    this->setBounds(x, y, width, height);
}

}}}
