#pragma once
#include "bittorrent/ui/config.h"
#include "bittorrent/ui/View.h"

namespace bittorrent {
namespace ui {
namespace views {

class ScrollView : public bittorrent::ui::View {
public:
    ScrollView() {}

    void setView(bittorrent::ui::View* view);

    virtual void layout() override;
    virtual void windowChanged() override;

    virtual void mouseWheel(int xPos, int yPos, int xWheel, int yWheel) override;

private:
    bittorrent::ui::View* _topView;
};

}}}
