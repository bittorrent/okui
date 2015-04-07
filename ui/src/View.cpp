#ifdef BT_USE_UI_VIEWS

#include "bittorrent/ui/View.h"

#include "bittorrent/ui/opengl/OpenGL.h"
#include "bittorrent/ui/Window.h"

namespace bittorrent {
namespace ui {

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
        view->_dispatchWindowChange();
    }
}

void View::removeSubview(View* view) {
    if (view->superview() != this) {
        return;
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
    return window()->application();
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
    }
}

void View::mouseUp(MouseButton button, int x, int y) {
    if (superview() && superview()->_interceptsMouseEvents) {
        auto point = localToSuperview(x, y);
        superview()->mouseUp(button, point.x, point.y);
    }
}

void View::renderAndRenderSubviews(Rectangle<int> viewport, double scale) {
    glViewport(viewport.x, viewport.y, viewport.width, viewport.height);

    render();

    for (auto& subview : _subviews) {
        Rectangle<int> subviewport(viewport.x + scale * subview->_bounds.x, viewport.y + scale * subview->_bounds.y, scale * subview->_bounds.width, scale * subview->_bounds.height);
        subview->renderAndRenderSubviews(subviewport, scale);
    }
}

bool View::dispatchMouseDown(MouseButton button, int x, int y) {
    if (_childrenInterceptMouseEvents) {
        for (auto it = _subviews.rbegin(); it != _subviews.rend(); ++it) {
            auto point = (*it)->superviewToLocal(x, y);
            if ((*it)->hitTest(point.x, point.y) && (*it)->dispatchMouseDown(button, point.x, point.y)) {
                return true;
            }
        }
    }
    if (_interceptsMouseEvents) {
        mouseDown(button, x, y);
        return true;
    }
    return false;
}

bool View::dispatchMouseUp(MouseButton button, int x, int y) {
    if (_childrenInterceptMouseEvents) {
        for (auto it = _subviews.rbegin(); it != _subviews.rend(); ++it) {
            auto point = (*it)->superviewToLocal(x, y);
            if ((*it)->hitTest(point.x, point.y) && (*it)->dispatchMouseUp(button, point.x, point.y)) {
                return true;
            }
        }
    }
    if (_interceptsMouseEvents) {
        mouseUp(button, x, y);
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

}}

#endif