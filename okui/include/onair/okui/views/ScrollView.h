#pragma once

#include "onair/okui/config.h"
#include "onair/okui/View.h"

namespace onair {
namespace okui {
namespace views {

class ScrollView : public onair::okui::View {
public:
    ScrollView();

    onair::okui::View* contentView() { return &_contentView; }

    /**
    * Returns the offset that the content view is currently scrolled to.
    */
    Point<int> contentOffset() const { return _contentView.bounds().origin(); }

    /**
    * Sets the content offset.
    */
    void setContentOffset(int x, int y);

    /**
    * Returns the current content size.
    */
    Point<int> contentSize() const { return _contentView.bounds().size(); }

    /**
    * Sets the content size.
    */
    void setContentSize(int width, int height);

    /**
    * Override this to take action when the view scrolls.
    */
    virtual void scrolled() {}

    virtual void mouseWheel(int xPos, int yPos, int xWheel, int yWheel) override;

private:
    onair::okui::View _contentView;
};

}}}
