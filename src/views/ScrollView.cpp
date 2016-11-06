#include <okui/views/ScrollView.h>

namespace okui {
namespace views {

namespace {
    constexpr auto kAvgVelocityTime = 0.06;
    constexpr auto kMaxVelocity = 2000.0;
    constexpr auto kDistanceThreshold = 50.0;
    constexpr auto kEndVelocityThreshold = 150.0;
}

ScrollView::ScrollView() {
    addSubview(&_contentView);
}

void ScrollView::setContentOffset(double x, double y) {
    _contentView.setBounds(x, y, _contentView.bounds().width, _contentView.bounds().height);
}

void ScrollView::setContentSize(double width, double height) {
    _contentView.setBounds(_contentView.bounds().x, _contentView.bounds().y, width, height);
}

void ScrollView::mouseWheel(double xPos, double yPos, int xWheel, int yWheel) {
    static constexpr auto multiplier = 10.0f;

    auto contentViewBounds = _contentView.bounds();
    contentViewBounds.x += -xWheel * multiplier;
    contentViewBounds.y += -yWheel * multiplier;

    _mouseDown = true;
    _scroll(contentViewBounds);
}

void ScrollView::mouseDown(MouseButton button, double x, double y) {
    _lastMousePos = Point<double>{x, y};
    _velocityTimer.restart();
    _mouseDown = true;
}

void ScrollView::mouseUp(MouseButton button, double startX, double startY, double x, double y) {
    _mouseDown = false;
    auto elapsed = 0.0;
    auto dX = 0.0;
    auto dY = 0.0;
    auto dT = 0.0;
    auto distanceX = 0.0;
    auto distanceY = 0.0;
    auto count = 0;

    for (auto i = _velocities.rbegin(); i != _velocities.rend(); ++i) {
        if (dT < kAvgVelocityTime || (count < 3 && dT < (kAvgVelocityTime * 2))){
            std::tie(elapsed, dX, dY) = *i;
            distanceX += std::min(std::max(dX / elapsed, -kMaxVelocity), kMaxVelocity);
            distanceY += std::min(std::max(dY / elapsed, -kMaxVelocity), kMaxVelocity);
            dT += elapsed;
            ++count;
        } else {
            break;
        }
    }

    if (!_velocities.empty()) {
        std::tie(elapsed, dX, dY) = *_velocities.rbegin();
        if (std::abs(dX/elapsed) < kEndVelocityThreshold) {
            distanceX = 0;
        }

        if (std::abs(dY/elapsed) < kEndVelocityThreshold) {
            distanceX = 0;
        }
    }

    _velocities.clear();
    _animX.reset(_contentView.bounds().x);
    _animY.reset(_contentView.bounds().y);

    if (count >= 3) {
        distanceX /= count;
        distanceY /= count;

        if (std::abs(distanceX) > kDistanceThreshold) {
            _animX.target(_contentView.bounds().x + distanceX, 1300ms, interpolation::Cubic::EaseOut);
        }

        if (std::abs(distanceY) > kDistanceThreshold) {
            _animY.target(_contentView.bounds().y + distanceY, 1300ms, interpolation::Cubic::EaseOut);
        }
    }

    addUpdateHook("ScrollView", [this] {
        if (_mouseDown || (_animX.current() == _animX.target() && _animY.current() == _animY.target())) {
            removeUpdateHook("ScrollView");
            return;
        }
        auto contentViewBounds = _contentView.bounds();
        contentViewBounds.x = _animX.current();
        contentViewBounds.y = _animY.current();
        _scroll(contentViewBounds);
    });
}

void ScrollView::mouseDrag(double startX, double startY, double x, double y) {
    auto dX = x - _lastMousePos.x,
         dY = y - _lastMousePos.y;

    _lastMousePos = Point<double>{x, y};

    auto contentViewBounds = _contentView.bounds();
    contentViewBounds.x += dX;
    contentViewBounds.y += dY;
    _scroll(contentViewBounds);

    auto elapsed = std::chrono::duration<double>(_velocityTimer.elapsed()).count();
    _velocityTimer.restart();
    _velocities.push_back({elapsed, dX, dY});
}

void ScrollView::_scroll(okui::Rectangle<double> newBounds) {
    auto bottom = newBounds.y + newBounds.height,
         right = newBounds.x + newBounds.width;

    if (newBounds.x > 0)                                  { newBounds.x = 0; }
    else if (newBounds.x < 0 && right < bounds().width)   { newBounds.x += std::min(bounds().width - right, 0 - newBounds.x); }
    if (newBounds.y > 0 )                                 { newBounds.y = 0; }
    else if (newBounds.y < 0 && bottom < bounds().height) { newBounds.y += std::min(bounds().height - bottom, 0 - newBounds.y); }

    _contentView.setBounds(newBounds.x, newBounds.y, newBounds.width, newBounds.height);

    scrolled();
}

} } // namespace okui::views
