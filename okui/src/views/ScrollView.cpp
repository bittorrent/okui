#include "onair/okui/views/ScrollView.h"

namespace onair {
namespace okui {
namespace views {

void ScrollView::setContentView(onair::okui::View* view) {
    _contentView = view;
    addSubview(_contentView);
    layout();
}

void ScrollView::layout() {
    if (_contentView) {
        _contentView->setBounds(0, 0, _contentView->bounds().width, _contentView->bounds().height);
    }
}

void ScrollView::mouseWheel(int xPos, int yPos, int xWheel, int yWheel) {
    if (_contentView == nullptr) { return; }

    static constexpr auto multiplier = 10.0f;

    auto contentViewBounds = _contentView->bounds();
    contentViewBounds.x += xWheel * multiplier;
    contentViewBounds.y += yWheel * multiplier;

    auto bottom = contentViewBounds.y + contentViewBounds.height,
         right = contentViewBounds.x + contentViewBounds.width;

    if (contentViewBounds.x > 0)                                  { contentViewBounds.x = 0; }
    else if (contentViewBounds.x < 0 && right < bounds().width)   { contentViewBounds.x += std::min(bounds().width - right, 0 - contentViewBounds.x); }
    if (contentViewBounds.y > 0 )                                 { contentViewBounds.y = 0; }
    else if (contentViewBounds.y < 0 && bottom < bounds().height) { contentViewBounds.y += std::min(bounds().height - bottom, 0 - contentViewBounds.y); }

    _contentView->setBounds(contentViewBounds.x, contentViewBounds.y, contentViewBounds.width, contentViewBounds.height);
}

}}}
