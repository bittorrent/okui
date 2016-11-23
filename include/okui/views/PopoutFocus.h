#pragma once

#include <okui/config.h>

#include <okui/Animation.h>
#include <okui/Point.h>

namespace okui::views {

/**
* PopoutFocus implements a view that pops out towards the viewer when focused. PopoutFocus must
* either derive from or be ancestor of a view that can receive focus.
*/
template <typename BaseView>
class PopoutFocus : public BaseView {
public:
    template <typename... Args>
    PopoutFocus(Args&&... args) : BaseView{std::forward<Args>(args)...} {}

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

    const Point<double>& size() const { return _size; }

    const Animation<double>& popoutAnimation() const { return _popoutAnimation; }

private:
    void _updateBounds();

    Animation<double> _popoutAnimation{0.0};
    Point<double>     _scaling{0.45, 0.45};
    Point<double>     _parentAnchorPosition;
    Point<double>     _selfAnchorPercentage;
    Point<double>     _size;
};

template <typename BaseView>
void PopoutFocus<BaseView>::focusGained() {
    _popoutAnimation.target(1.0, 70ms, interpolation::Exponential::EaseOut);
    this->addUpdateHook("PopoutFocus", [&] { _updateBounds(); });
    BaseView::focusGained();
}

template <typename BaseView>
void PopoutFocus<BaseView>::focusLost() {
    _popoutAnimation.target(0.0, 400ms, interpolation::Exponential::EaseOut);
    this->addUpdateHook("PopoutFocus", [&] { _updateBounds(); });
    BaseView::focusLost();
}

template <typename BaseView>
void PopoutFocus<BaseView>::disappeared() {
    _popoutAnimation.reset(0.0);
    _updateBounds();
    BaseView::disappeared();
}

template <typename BaseView>
void PopoutFocus<BaseView>::mouseMovement(double x, double y) {
    this->focus();
    BaseView::mouseMovement(x, y);
}

template <typename BaseView>
void PopoutFocus<BaseView>::setScaling(Point<double> scaling) {
    _scaling = scaling;
    _updateBounds();
}

template <typename BaseView>
void PopoutFocus<BaseView>::setAnchor(Point<double> parentPosition, Point<double> selfPercentage) {
    if (_parentAnchorPosition == parentPosition && _selfAnchorPercentage == selfPercentage) { return; }
    _parentAnchorPosition = parentPosition;
    _selfAnchorPercentage = selfPercentage;
    _updateBounds();
}

template <typename BaseView>
void PopoutFocus<BaseView>::setSize(Point<double> size) {
    if (_size == size) { return; }
    _size = size;
    _updateBounds();
}

template <typename BaseView>
void PopoutFocus<BaseView>::_updateBounds() {
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

} // namespace okui::views
