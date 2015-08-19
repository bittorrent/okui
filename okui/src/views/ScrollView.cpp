#include "onair/okui/views/ScrollView.h"
#include "onair/okui/Easings.h"

namespace onair {
namespace okui {
namespace views {

namespace {
    constexpr auto kAvgVelocityTime = 0.06;
    constexpr auto kDistanceTreshold = 50.0;
}

ScrollView::ScrollView() {
    addSubview(&_contentView);
}

void ScrollView::setContentOffset(int x, int y) {
    _contentView.setBounds(x, y, _contentView.bounds().width, _contentView.bounds().height);
}

void ScrollView::setContentSize(int width, int height) {
    _contentView.setBounds(_contentView.bounds().x, _contentView.bounds().y, width, height);
}

void ScrollView::mouseWheel(int xPos, int yPos, int xWheel, int yWheel) {
    static constexpr auto multiplier = 10.0f;

    auto contentViewBounds = _contentView.bounds();
    contentViewBounds.x += xWheel * multiplier;
    contentViewBounds.y += yWheel * multiplier;

    _scroll(contentViewBounds);
}

void ScrollView::mouseDown(MouseButton button, int x, int y) {
    _lastMousePos = Point<int>{x, y};
    _velocityTimer.restart();
    _mouseDown = true;
}

void ScrollView::mouseUp(MouseButton button, int startX, int startY, int x, int y) {
    _mouseDown = false;
    auto elapsed = 0.0;
    auto dX = 0.0;
    auto dY = 0.0;
    auto dT = 0.0;
    auto distanceX = 0.0;
    auto distanceY = 0.0;

    for (auto i = _velocities.rbegin(); i != _velocities.rend() && dT < kAvgVelocityTime; ++i) {
        std::tie(elapsed, dX, dY) = *i;
        distanceX += dX * elapsed;
        distanceY += dY * elapsed;
        dT += elapsed;

        if (std::abs(dY) < 5 || std::abs(dX) < 5) {
            break;
        }
    }

    _velocities.clear();

    if (dT < kAvgVelocityTime) {
        dT = kAvgVelocityTime;
    }

    distanceX /= dT * dT;
    distanceY /= dT * dT;

    _animX.reset(_contentView.bounds().x);
    _animY.reset(_contentView.bounds().y);

    if (std::abs(distanceX) > kDistanceTreshold) {
        _animX.target(_contentView.bounds().x + distanceX, 1300_ms, easings::Cubic::EaseOut);
    }

    if (std::abs(distanceY) > kDistanceTreshold) {
        _animY.target(_contentView.bounds().y + distanceY, 1300_ms, easings::Cubic::EaseOut);
    }
}

void ScrollView::mouseDrag(int startX, int startY, int x, int y) {
    auto dX = x - _lastMousePos.x,
         dY = y - _lastMousePos.y;

    _lastMousePos = Point<int>{x, y};

    auto contentViewBounds = _contentView.bounds();
    contentViewBounds.x += dX;
    contentViewBounds.y += dY;
    _scroll(contentViewBounds);

    auto elapsed = std::chrono::duration<double>(_velocityTimer.elapsed()).count();
    _velocityTimer.restart();
    _velocities.push_back({elapsed, dX, dY});
}

void ScrollView::update() {
    if (!_mouseDown) {
        auto contentViewBounds = _contentView.bounds();
        contentViewBounds.x = _animX.current();
        contentViewBounds.y = _animY.current();
        _scroll(contentViewBounds);
    }
}

void ScrollView::_scroll(okui::Rectangle<int> newBounds) {
    auto bottom = newBounds.y + newBounds.height,
         right = newBounds.x + newBounds.width;

    if (newBounds.x > 0)                                  { newBounds.x = 0; }
    else if (newBounds.x < 0 && right < bounds().width)   { newBounds.x += std::min(bounds().width - right, 0 - newBounds.x); }
    if (newBounds.y > 0 )                                 { newBounds.y = 0; }
    else if (newBounds.y < 0 && bottom < bounds().height) { newBounds.y += std::min(bounds().height - bottom, 0 - newBounds.y); }

    _contentView.setBounds(newBounds.x, newBounds.y, newBounds.width, newBounds.height);

    scrolled();
}

}}}
