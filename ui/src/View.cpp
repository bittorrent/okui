#include "bittorrent/ui/View.h"

#include "bittorrent/ui/opengl/OpenGL.h"
#include "bittorrent/ui/Window.h"

namespace bittorrent {
namespace ui {

View::~View() {
    if (superview()) {
        superview()->removeSubview(this);
    }
}

void View::addSubview(View* view) {
    if (view->superview() == this) {
        return;
    }

    auto viewWindow = view->window();

    if (view->superview()) {
        view->superview()->removeSubview(view);
    }
    view->_superview = this;
    _subviews.push_back(view);

    if (viewWindow != view->window()) {
        if (viewWindow) {
            viewWindow->endDragging(view);
        }
        view->_dispatchWindowChange();
    }
}

void View::removeSubview(View* view) {
    if (view->superview() != this) {
        return;
    }

    if (_subviewWithMouse == view) {
        _subviewWithMouse = nullptr;
    }

    auto viewWindow = view->window();

    view->_superview = nullptr;
    for (auto it = _subviews.begin(); it != _subviews.end(); ++it) {
        if (*it == view) {
            _subviews.erase(it);
            break;
        }
    }

    if (viewWindow != view->window()) {
        if (viewWindow) {
            viewWindow->endDragging(view);
        }
        view->_dispatchWindowChange();
    }
}

void View::setBounds(int x, int y, int width, int height) {
    auto willResize = (_bounds.width != width || _bounds.height != height);

    _bounds.x = x;
    _bounds.y = y;
    _bounds.width = width;
    _bounds.height = height;

    if (willResize) {
        layout();
    }
}

void View::setInterceptsMouseEvents(bool intercepts, bool childrenIntercept) {
    _interceptsMouseEvents = intercepts;
    _childrenInterceptMouseEvents = childrenIntercept;
}

ShaderCache* View::shaderCache() {
    return window()->shaderCache();
}

void View::sendToBack() {
    auto& siblings = superview()->_subviews;
    for (auto it = siblings.begin(); it != siblings.end(); ++it) {
        if (*it == this) {
            siblings.erase(it);
            siblings.push_front(this);
            return;
        }
    }
}

void View::bringToFront() {
    auto& siblings = superview()->_subviews;
    for (auto it = siblings.begin(); it != siblings.end(); ++it) {
        if (*it == this) {
            siblings.erase(it);
            siblings.push_back(this);
            return;
        }
    }
}

void View::focus() {
    window()->setFocus(this);
}

bool View::isDescendantOf(View* view) {
    if (superview() == view) {
        return true;
    }
    return superview() && superview()->isDescendantOf(view);
}

bool View::hasMouse() const {
    return superview() && superview()->_subviewWithMouse == this;
}

AffineTransformation View::renderTransformation() {
    return AffineTransformation(-1.0, -1.0, 0.0, 0.0, 2.0 / _bounds.width, 2.0 / _bounds.height);
}

View* View::superview() const {
    return _superview;
}

Window* View::window() const {
    return _superview ? _superview->window() : _window;
}

Application* View::application() const {
    return window() ? window()->application() : nullptr;
}

Point<int> View::localToSuperview(int x, int y) {
    return Point<int>(bounds().x + x, bounds().y + y);
}

Point<int> View::superviewToLocal(int x, int y) {
    return Point<int>(x - bounds().x, y - bounds().y);
}

bool View::hitTest(int x, int y) {
    return (x >= 0 && x < bounds().width && y >= 0 && y < bounds().height);
}

void View::mouseDown(MouseButton button, int x, int y) {
    if (superview() && superview()->_interceptsMouseEvents) {
        auto point = localToSuperview(x, y);
        superview()->mouseDown(button, point.x, point.y);
        window()->beginDragging(superview());
    }
}

void View::mouseUp(MouseButton button, int x, int y) {
    if (superview() && superview()->_interceptsMouseEvents) {
        auto point = localToSuperview(x, y);
        superview()->mouseUp(button, point.x, point.y);
    }
}

void View::mouseWheel(int xPos, int yPos, int xWheel, int yWheel) {
    if (superview() && superview()->_interceptsMouseEvents) {
        auto point = localToSuperview(xPos, yPos);
        superview()->mouseWheel(point.x, point.y, xWheel, yWheel);
    }
}

void View::renderAndRenderSubviews(Rectangle<int> viewport, double scale, boost::optional<Rectangle<int>> parentClippedBounds) {
    if (!isVisible()) {
        return;
    }

    glViewport(viewport.x, viewport.y, viewport.width, viewport.height);
    glEnable(GL_BLEND);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE);

    if (_clipped) {
        auto clipBounds = viewport;
        if (parentClippedBounds)  {
            Point<int> topLeft    {std::max(parentClippedBounds->x, clipBounds.x),
                                   std::max(parentClippedBounds->y, clipBounds.y)};
            Point<int> bottomRight{std::min(parentClippedBounds->x + parentClippedBounds->width, clipBounds.x + clipBounds.width),
                                   std::min(parentClippedBounds->y + parentClippedBounds->height, clipBounds.y + clipBounds.height)};

            if (bottomRight.x < topLeft.x) { topLeft.x = bottomRight.x; }
            if (bottomRight.y < topLeft.y) { topLeft.y = bottomRight.y; }

            clipBounds.x = topLeft.x;
            clipBounds.y = topLeft.y;
            clipBounds.width = bottomRight.x - topLeft.x;
            clipBounds.height = bottomRight.y - topLeft.y;
        }
        parentClippedBounds = clipBounds;
        glScissor(clipBounds.x, clipBounds.y, clipBounds.width, clipBounds.height);
        glEnable(GL_SCISSOR_TEST);
    }

    render();

    for (auto& subview : _subviews) {
        Rectangle<int> subviewport(viewport.x + scale * subview->_bounds.x, viewport.y + scale * subview->_bounds.y, scale * subview->_bounds.width, scale * subview->_bounds.height);
        subview->renderAndRenderSubviews(subviewport, scale, parentClippedBounds);
    }

    if (_clipped) {
        glDisable(GL_SCISSOR_TEST);
    }
}

bool View::dispatchMouseDown(MouseButton button, int x, int y) {
    if (!isVisible()) {
        return false;
    }
    if (_childrenInterceptMouseEvents) {
        for (auto it = _subviews.rbegin(); it != _subviews.rend(); ++it) {
            auto point = (*it)->superviewToLocal(x, y);
            if ((!(*it)->clipped() || (*it)->hitTest(point.x, point.y)) && (*it)->dispatchMouseDown(button, point.x, point.y)) {
                return true;
            }
        }
    }
    if (_interceptsMouseEvents && hitTest(x, y)) {
        if (!isDescendantOf(window()->focus())) {
            window()->setFocus(nullptr);
        }
        mouseDown(button, x, y);
        window()->beginDragging(this);
        return true;
    }
    return false;
}

bool View::dispatchMouseUp(MouseButton button, int x, int y) {
    if (!isVisible()) {
        return false;
    }
    if (_childrenInterceptMouseEvents) {
        for (auto it = _subviews.rbegin(); it != _subviews.rend(); ++it) {
            auto point = (*it)->superviewToLocal(x, y);
            if ((!(*it)->clipped() || (*it)->hitTest(point.x, point.y)) && (*it)->dispatchMouseUp(button, point.x, point.y)) {
                return true;
            }
        }
    }
    if (_interceptsMouseEvents && hitTest(x, y)) {
        mouseUp(button, x, y);
        return true;
    }
    return false;
}

void View::dispatchMouseMovement(int x, int y) {
    if (!isVisible()) {
        return;
    }

    View* subview = nullptr;

    if (_childrenInterceptMouseEvents) {
        for (auto it = _subviews.rbegin(); it != _subviews.rend(); ++it) {
            auto point = (*it)->superviewToLocal(x, y);
            if ((*it)->hitTest(point.x, point.y)) {
                if (!subview) {
                    subview = *it;
                }
                (*it)->dispatchMouseMovement(point.x, point.y);
            } else if (!(*it)->clipped()) {
                (*it)->dispatchMouseMovement(point.x, point.y);
            }
        }
    }

    if (subview != _subviewWithMouse) {
        if (_subviewWithMouse) {
            _subviewWithMouse->_mouseExit();
        }
        _subviewWithMouse = subview;
        if (_subviewWithMouse) {
            _subviewWithMouse->mouseEnter();
        }
    }

    if (_interceptsMouseEvents && hitTest(x, y)) {
        mouseMovement(x, y);
    }
}

bool View::dispatchMouseWheel(int xPos, int yPos, int xWheel, int yWheel) {
    if (!isVisible()) {
        return false;
    }

    if (_childrenInterceptMouseEvents) {
        for (auto it = _subviews.rbegin(); it != _subviews.rend(); ++it) {
            auto point = (*it)->superviewToLocal(xPos, yPos);
            if ((!(*it)->clipped() || (*it)->hitTest(point.x, point.y)) &&
                (*it)->dispatchMouseWheel(point.x, point.y, xWheel, yWheel)) {
                return true;
            }
        }
    }

    if (_interceptsMouseEvents && hitTest(xPos, yPos)) {
        mouseWheel(xPos, yPos, xWheel, yWheel);
        return true;
    }

    return false;
}

void View::_dispatchWindowChange() {
    windowChanged();

    for (auto& subview : _subviews) {
        subview->_dispatchWindowChange();
    }
}

void View::_mouseExit() {
    if (_subviewWithMouse && _clipped) {
        _subviewWithMouse->_mouseExit();
        _subviewWithMouse = nullptr;
    }
    mouseExit();
}

}}
