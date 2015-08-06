#include "onair/okui/views/ScrollView.h"

namespace onair {
namespace okui {
namespace views {

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
}

void ScrollView::mouseDrag(int startX, int startY, int x, int y) {
    auto dX = x - _lastMousePos.x,
         dY = y - _lastMousePos.y;

    auto contentViewBounds = _contentView.bounds();
    contentViewBounds.x += dX;
    contentViewBounds.y += dY;

    _scroll(contentViewBounds);

    _lastMousePos = Point<int>{x, y};
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
