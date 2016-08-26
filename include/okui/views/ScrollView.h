#pragma once

#include "okui/config.h"

#include "okui/View.h"
#include "okui/Animation.h"

#include "scraps/Timer.h"

namespace okui {
namespace views {

class ScrollView : public okui::View {
public:
    ScrollView();

    okui::View* contentView() { return &_contentView; }

    /**
    * Returns the offset that the content view is currently scrolled to.
    */
    Point<double> contentOffset() const { return _contentView.bounds().position(); }

    /**
    * Sets the content offset.
    */
    void setContentOffset(double x, double y);

    /**
    * Returns the current content size.
    */
    Point<double> contentSize() const { return _contentView.bounds().size(); }

    /**
    * Sets the content size.
    */
    void setContentSize(double width, double height);

    /**
    * Override this to take action when the view scrolls.
    */
    virtual void scrolled() {}

    virtual void mouseDown(MouseButton button, double x, double y) override;
    virtual void mouseUp(MouseButton button, double startX, double startY, double x, double y) override;
    virtual void mouseDrag(double startX, double startY, double x, double y) override;
    virtual void mouseWheel(double xPos, double yPos, int xWheel, int yWheel) override;

private:
    void _scroll(okui::Rectangle<double> newBounds);

    Point<double> _lastMousePos{-1.0, -1.0};
    okui::View _contentView;
    scraps::SteadyTimer _velocityTimer;

    okui::Animation<double> _animX{0};
    okui::Animation<double> _animY{0};

    bool _mouseDown = false;

    std::vector<std::tuple<double, double, double>> _velocities;
};

} } // namespace okui::views
