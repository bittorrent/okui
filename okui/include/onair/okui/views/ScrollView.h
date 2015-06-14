#pragma once
#include "onair/okui/config.h"
#include "onair/okui/View.h"

namespace onair {
namespace okui {
namespace views {

class ScrollView : public onair::okui::View {
public:
    ScrollView() {}
    ScrollView(onair::okui::View* contentView) { setContentView(contentView); }

    void setContentView(onair::okui::View* view);

    virtual void layout() override;
    virtual void mouseWheel(int xPos, int yPos, int xWheel, int yWheel) override;

private:
    onair::okui::View* _contentView;
};

}}}
