#pragma once
#include "onair/okui/config.h"
#include "onair/okui/View.h"

namespace onair {
namespace okui {
namespace views {

class ScrollView : public onair::okui::View {
public:
    ScrollView() {}

    void setView(onair::okui::View* view);

    virtual void layout() override;
    virtual void windowChanged() override;

    virtual void mouseWheel(int xPos, int yPos, int xWheel, int yWheel) override;

private:
    onair::okui::View* _topView;
};

}}}
