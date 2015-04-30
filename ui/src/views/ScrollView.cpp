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
    if (_topView) {
        _topView->setBounds(0, _topView->bounds().height, bounds().width, bounds().height);
    }
}

void ScrollView::windowChanged() {
    layout();
}

void ScrollView::mouseWheel(int xPos, int yPos, int xWheel, int yWheel) {
    static constexpr auto multiplier = 10.0f;

    auto topViewBounds = _topView->bounds();
    topViewBounds.x += xWheel * multiplier;
    topViewBounds.y += -yWheel * multiplier;

    if (topViewBounds.x + topViewBounds.width < bounds().width) { topViewBounds.x = bounds().width - topViewBounds.width; }
    if (topViewBounds.x > 0) { topViewBounds.x = 0; }
    if (topViewBounds.y + topViewBounds.height < bounds().height) { topViewBounds.y = bounds().height - topViewBounds.height; }
    if (topViewBounds.y > 0) { topViewBounds.y = 0; }

    _topView->setBounds(topViewBounds.x, topViewBounds.y, topViewBounds.width, topViewBounds.height);
}

}}}
