#include "bittorrent/ui/views/ScrollView.h"

namespace bittorrent {
namespace ui {
namespace views {

void ScrollView::setView(bittorrent::ui::View* view) {
    _topView = view;
    addSubview(_topView);
    layout();
}

void ScrollView::layout() {
    if (_topView != nullptr) {
        _topView->setBounds(0, 0, _topView->bounds().width, _topView->bounds().height);
    }
}

void ScrollView::windowChanged() {
    layout();
}

void ScrollView::mouseWheel(int xPos, int yPos, int xWheel, int yWheel) {
    if (_topView == nullptr) { return; }

    static constexpr auto multiplier = 10.0f;

    auto topViewBounds = _topView->bounds();
    topViewBounds.x += xWheel * multiplier;
    topViewBounds.y += yWheel * multiplier;

    auto bottom = topViewBounds.y + topViewBounds.height,
         right = topViewBounds.x + topViewBounds.width;

    if (topViewBounds.x > 0)                                  { topViewBounds.x = 0; }
    else if (topViewBounds.x < 0 && right < bounds().width)   { topViewBounds.x += std::min(bounds().width - right, 0 - topViewBounds.x); }
    if (topViewBounds.y > 0 )                                 { topViewBounds.y = 0; }
    else if (topViewBounds.y < 0 && bottom < bounds().height) { topViewBounds.y += std::min(bounds().height - bottom, 0 - topViewBounds.y); }

    _topView->setBounds(topViewBounds.x, topViewBounds.y, topViewBounds.width, topViewBounds.height);
}

}}}
