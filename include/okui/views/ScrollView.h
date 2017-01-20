/**
* Copyright 2017 BitTorrent Inc.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*    http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/
#pragma once

#include <okui/config.h>

#include <okui/View.h>
#include <okui/Animation.h>

#include <scraps/Timer.h>

namespace okui::views {

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

} // namespace okui::views
