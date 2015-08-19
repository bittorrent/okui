#pragma once

#include "onair/okui/config.h"
#include "onair/okui/View.h"
#include "onair/Timer.h"
#include "onair/okui/Animation.h"

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

    virtual void mouseDown(MouseButton button, int x, int y) override;
    virtual void mouseUp(MouseButton button, int startX, int startY, int x, int y) override;
    virtual void mouseDrag(int startX, int startY, int x, int y) override;
    virtual void mouseWheel(int xPos, int yPos, int xWheel, int yWheel) override;

    virtual void update() override;

private:
    void _scroll(okui::Rectangle<int> newBounds);

    Point<int> _lastMousePos{-1, -1};
    onair::okui::View _contentView;
    onair::SteadyTimer _velocityTimer;

    onair::okui::Animation<double> _animX{0};
    onair::okui::Animation<double> _animY{0};

    bool _mouseDown = false;

    std::vector<std::tuple<double, double, double>> _velocities;
};

}}}
