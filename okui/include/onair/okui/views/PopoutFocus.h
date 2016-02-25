#pragma once

#include "onair/okui/Animation.h"
#include "onair/okui/View.h"

namespace onair {
namespace okui {
namespace views {

/**
* PopoutFocus implements a focusable view that pops out towards the viewer.
*/
class PopoutFocus : public View {
public:
    virtual bool canBecomeFocus() override { return true; }
    virtual void focusGained() override;
    virtual void focusLost() override;

    void setScaling(double scaling) { setScaling({scaling, scaling}); }
    void setScaling(double x, double y) { setScaling({x, y}); }
    void setScaling(Point<double> scaling);

    void setAnchor(Point<double> parentPosition, Point<double> selfPercentage);

    void setSize(double width, double height) { setSize({width, height}); }
    void setSize(Point<double> size);

private:
    Animation<double> _popoutAnimation{0.0};

    Point<double> _scaling{0.45, 0.45};
    Point<double> _parentAnchorPosition;
    Point<double> _selfAnchorPercentage;
    Point<double> _size;

    void _startUpdates();
    void _updateBounds();
};

}}}
