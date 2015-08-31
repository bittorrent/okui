#include "onair/okui/View.h"

#include "onair/okui/Application.h"
#include "onair/okui/BitmapFont.h"
#include "onair/okui/opengl/OpenGL.h"
#include "onair/okui/shapes/Rectangle.h"
#include "onair/okui/Window.h"

namespace onair {
namespace okui {

View::~View() {
    if (application()) {
        application()->removeListeners(this);
    }

    if (_nextFocus) {
        _nextFocus->_previousFocus = _previousFocus;
    }

    if (_previousFocus) {
        _previousFocus->_nextFocus = _nextFocus;
    }

    while (!_subviews.empty()) {
        removeSubview(*_subviews.begin());
    }

    if (superview()) {
        superview()->removeSubview(this);
    }
}

void View::addSubview(View* view) {
    if (view->superview() == this) {
        return;
    }

    bool isViewAppearance = !view->isVisibleInOpenWindow() && view->isVisible() && isVisibleInOpenWindow();

    if (isViewAppearance) {
        view->_dispatchFutureVisibilityChange(true);
    }

    if (view->superview()) {
        if (view->superview()->window() && view->superview()->ancestorsAreVisible()) {
            isViewAppearance = false;
        }
        view->superview()->removeSubview(view);
    }
    view->_superview = this;
    _subviews.push_back(view);

    if (_window != view->window()) {
        if (view->window()) {
            view->window()->endDragging(view);
        }
        view->_dispatchWindowChange(_window);
    }

    if (isViewAppearance) {
        view->_dispatchVisibilityChange(true);
    }

    invalidateRenderCache();
}

void View::removeSubview(View* view) {
    if (view->superview() != this) {
        return;
    }

    view->unfocus();

    bool isViewDisappearance = view->isVisibleInOpenWindow();

    if (isViewDisappearance) {
        view->_dispatchFutureVisibilityChange(false);
    }

    if (_subviewWithMouse == view) {
        _subviewWithMouse = nullptr;
    }

    view->_superview = nullptr;

    auto viewIt = std::find(_subviews.begin(), _subviews.end(), view);
    if (viewIt != _subviews.end()) {
        _subviews.erase(viewIt);
    }

    if (view->window() != nullptr) {
        view->window()->endDragging(view);
        view->_dispatchWindowChange(nullptr);
    }

    if (isViewDisappearance) {
        view->_dispatchVisibilityChange(false);
    }

    invalidateRenderCache();
}

void View::setIsVisible(bool isVisible) {
    if (_isVisible == isVisible) { return; }

    if (superview() && superview()->isVisibleInOpenWindow()) {
        _dispatchFutureVisibilityChange(isVisible);
    }

    _isVisible = isVisible;

    if (superview() && superview()->isVisibleInOpenWindow()) {
        _dispatchVisibilityChange(isVisible);
    }

    if (superview()) {
        superview()->invalidateRenderCache();
    }
}

bool View::ancestorsAreVisible() const {
    return superview() ? (superview()->isVisible() && superview()->ancestorsAreVisible()) : true;
}

bool View::isVisibleInOpenWindow() const {
    return isVisible() && ancestorsAreVisible() && window() && window()->isOpen();
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
    if (isVisible()) {
        superview()->invalidateRenderCache();
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
    if (isVisible()) {
        superview()->invalidateRenderCache();
    }
}

void View::focus() {
    if (window()) {
        window()->setFocus(this);
    }
}

void View::unfocus() {
    if (window() && isFocus()) {
        window()->setFocus(nullptr);
    }
}

void View::setNextFocus(View* view) {
    if (_nextFocus == view) { return; }

    if (_nextFocus) {
        if (_nextFocus->_previousFocus && _nextFocus->_previousFocus != this) {
            _nextFocus->_previousFocus->_nextFocus = nullptr;
        }
        _nextFocus->_previousFocus = nullptr;
    }

    _nextFocus = view;

    if (_nextFocus) {
        _nextFocus->_previousFocus = this;
    }
}

View* View::nextAvailableFocus() {
    auto view = _nextFocus;
    while (view && view != this) {
        if (view->isVisible() && view->canBecomeFocus()) {
            return view;
        }
        view = view->_nextFocus;
    }
    return nullptr;
}

View* View::previousAvailableFocus() {
    auto view = _previousFocus;
    while (view && view != this) {
        if (view->isVisible() && view->canBecomeFocus()) {
            return view;
        }
        view = view->_previousFocus;
    }
    return nullptr;
}

bool View::isFocus() const {
    return window() && window()->focus() == this;
}

bool View::isDescendantOf(const View* view) const {
    if (superview() == view) {
        return true;
    }
    return superview() && superview()->isDescendantOf(view);
}

bool View::hasMouse() const {
    return superview() && superview()->_subviewWithMouse == this;
}

std::shared_ptr<Texture> View::renderTexture() const {
    return _renderCacheTexture;
}

void View::invalidateRenderCache() {
    _hasCachedRender = false;
    if (isVisible() && superview()) {
        superview()->invalidateRenderCache();
    }
}

TextureHandle View::loadTextureResource(const std::string& name) {
    if (!window()) { return nullptr; }
    auto handle = window()->loadTextureResource(name);
    handle.onLoad([this]{ invalidateRenderCache(); });
    return handle;
}

TextureHandle View::loadTextureFromMemory(std::shared_ptr<const std::string> data) {
    if (!window()) { return nullptr; }
    auto handle = window()->loadTextureFromMemory(data);
    handle.onLoad([this]{ invalidateRenderCache(); });
    return handle;
}

TextureHandle View::loadTextureFromURL(const std::string& url) {
    if (!window()) { return nullptr; }
    auto handle = window()->loadTextureFromURL(url);
    handle.onLoad([this]{ invalidateRenderCache(); });
    return handle;
}

AffineTransformation View::renderTransformation() {
    // 2/width and 2/height because clip space is -1 to 1 (2x2), so scale down into clip space
    // Scale y is negative and translate (-1, 1) to get the top left to be (0, 0)
    return AffineTransformation{-1, 1, 0, 0, 2.0/_bounds.width, -2.0/_bounds.height};
}

View* View::superview() const {
    return _superview;
}

Application* View::application() const {
    return window() ? window()->application() : nullptr;
}

Point<int> View::localToSuperview(int x, int y) {
    return Point<int>(bounds().x + x, bounds().y + y);
}

Point<int> View::localToSuperview(const Point<int>& p) {
    return localToSuperview(p.x, p.y);
}

Point<int> View::superviewToLocal(int x, int y) {
    return Point<int>(x - bounds().x, y - bounds().y);
}

Point<int> View::superviewToLocal(const Point<int>& p) {
    return superviewToLocal(p.x, p.y);
}

bool View::hitTest(int x, int y) {
    return x >= 0 && x < bounds().width &&
           y >= 0 && y < bounds().height;
}

void View::mouseDown(MouseButton button, int x, int y) {
    if (superview() && superview()->_interceptsMouseEvents) {
        auto point = localToSuperview(x, y);
        superview()->mouseDown(button, point.x, point.y);
        window()->beginDragging(superview());
    }
}

void View::mouseUp(MouseButton button, int startX, int startY, int x, int y) {
    if (superview() && superview()->_interceptsMouseEvents) {
        auto startPoint = localToSuperview(startX, startY);
        auto point = localToSuperview(x, y);
        superview()->mouseUp(button, startPoint.x, startPoint.y, point.x, point.y);
    }
}

void View::mouseWheel(int xPos, int yPos, int xWheel, int yWheel) {
    if (superview() && superview()->_interceptsMouseEvents) {
        auto point = localToSuperview(xPos, yPos);
        superview()->mouseWheel(point.x, point.y, xWheel, yWheel);
    }
}

Responder* View::nextResponder() {
    return superview() ? dynamic_cast<Responder*>(superview()) : dynamic_cast<Responder*>(window());
}

void View::keyDown(KeyCode key, KeyModifiers mod, bool repeat) {
    if (key == KeyCode::kTab) {
        if (_previousFocus && (mod & KeyModifier::kShift)) {
            if (auto view = previousAvailableFocus()) {
                view->focus();
            }
            return;
        } else if (_nextFocus) {
            if (auto view = nextAvailableFocus()) {
                view->focus();
            }
            return;
        }
    }

    Responder::keyDown(key, mod, repeat);
}

bool View::hasRelation(View::Relation relation, const View* view) const {
    switch (relation) {
        case Relation::kApplication:
            return application() && application() == view->application();
        case Relation::kWindow:
            return window() && window() == view->window();
        case Relation::kDescendant:
            return isDescendantOf(view);
        case Relation::kAncestor:
            return view->isDescendantOf(this);
        case Relation::kSibling:
            return superview() && superview() == view->superview();
    }
    assert(false);
    return false;
}

void View::updateAndUpdateSubviews() {
    if (!isVisible()) {
        return;
    }

    update();

    for (auto& subview : _subviews) {
        subview->updateAndUpdateSubviews();
    }
}

void View::renderAndRenderSubviews(const RenderTarget* target, const Rectangle<int>& area, boost::optional<Rectangle<int>> clipBounds) {
    if (!isVisible() || !area.width || !area.height) { return; }

    if (!_requiresTextureRendering() && !_cachesRender) {
        // render directly
        _renderAndRenderSubviews(target, area, clipBounds);
        _renderCacheTexture->set();
        return;
    }

    // make sure the render cache is up-to-date

    GLint previousFramebuffer = 0;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &previousFramebuffer);

    if (!_renderCacheColorAttachment || _renderCacheColorAttachment->width() != area.width || _renderCacheColorAttachment->height() != area.height) {
        // update the framebuffer
        _renderCache.reset(new opengl::Framebuffer());
        _renderCacheColorAttachment = _renderCache->addColorAttachment(area.width, area.height);
        // TODO: stencil attachment
        ONAIR_ASSERT(_renderCache->isComplete());
        _renderCacheTexture->set(_renderCacheColorAttachment->texture(), area.width, area.height);
        _hasCachedRender = false;
    }

    if (!_cachesRender || !_hasCachedRender) {
        // render to _renderCache
        _renderCache->bind();
        glClearColor(0.0, 0.0, 0.0, 0.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        Rectangle<int> cacheArea(0, 0, area.width, area.height);
        RenderTarget cacheTarget(area.width, area.height);
        _renderAndRenderSubviews(&cacheTarget, cacheArea);
        _hasCachedRender = true;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, previousFramebuffer);

    // do the actual rendering

    glViewport(area.x, target->height() - area.maxY(), area.width, area.height);
    glEnable(GL_BLEND);
    glBlendFuncSeparate(GL_ONE, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE);

    clipBounds = clipBounds ? clipBounds->intersection(area) : area;
    glEnable(GL_SCISSOR_TEST);
    glScissor(clipBounds->x, target->height() - clipBounds->maxY(), clipBounds->width, clipBounds->height);

    AffineTransformation transformation{-1, -1, 0, 0, 2.0/_bounds.width, 2.0/_bounds.height};
    postRender(_renderCacheTexture, transformation);

    glDisable(GL_SCISSOR_TEST);
}

void View::postRender(std::shared_ptr<Texture> texture, const AffineTransformation& transformation) {
    auto shader = textureShader();
    shader->setTransformation(transformation);
    shader->setColor(_tintColor.r, _tintColor.g, _tintColor.b, _tintColor.a);
    shader->drawScaledFill(*texture, Rectangle<double>(0.0, 0.0, bounds().width, bounds().height));
    shader->flush();
}

bool View::dispatchMouseDown(MouseButton button, int x, int y) {
    if (!isVisible()) { return false; }

    if (_childrenInterceptMouseEvents) {
        for (auto it = _subviews.rbegin(); it != _subviews.rend(); ++it) {
            auto point = (*it)->superviewToLocal(x, y);
            if ((!(*it)->clipsToBounds() || (*it)->hitTest(point.x, point.y)) && (*it)->dispatchMouseDown(button, point.x, point.y)) {
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

bool View::dispatchMouseUp(MouseButton button, int startX, int startY, int x, int y) {
    if (!isVisible()) { return false; }

    if (_childrenInterceptMouseEvents) {
        for (auto it = _subviews.rbegin(); it != _subviews.rend(); ++it) {
            auto startPoint = (*it)->superviewToLocal(startX, startY);
            auto point = (*it)->superviewToLocal(x, y);
            if ((!(*it)->clipsToBounds() || (*it)->hitTest(point.x, point.y)) && (*it)->dispatchMouseUp(button, startPoint.x, startPoint.y, point.x, point.y)) {
                return true;
            }
        }
    }
    if (_interceptsMouseEvents && hitTest(x, y)) {
        mouseUp(button, startX, startY, x, y);
        return true;
    }
    return false;
}

void View::dispatchMouseMovement(int x, int y) {
    if (!isVisible()) { return; }

    View* subview = nullptr;

    if (_childrenInterceptMouseEvents) {
        for (auto it = _subviews.rbegin(); it != _subviews.rend(); ++it) {
            auto point = (*it)->superviewToLocal(x, y);
            if ((*it)->hitTest(point.x, point.y)) {
                if (!subview) {
                    subview = *it;
                }
                (*it)->dispatchMouseMovement(point.x, point.y);
            } else if (!(*it)->clipsToBounds()) {
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
    if (!isVisible()) { return false; }

    if (_childrenInterceptMouseEvents) {
        for (auto it = _subviews.rbegin(); it != _subviews.rend(); ++it) {
            auto point = (*it)->superviewToLocal(xPos, yPos);
            if ((!(*it)->clipsToBounds() || (*it)->hitTest(point.x, point.y)) &&
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

void View::_setBounds(const Rectangle<int>& bounds) {
    auto willMove = (_bounds.x != bounds.x || _bounds.y != bounds.y);
    auto willResize = (_bounds.width != bounds.width || _bounds.height != bounds.height);

    if (!willMove && !willResize) {
        return;
    }

    _bounds = std::move(bounds);

    if (willResize) {
        layout();
        invalidateRenderCache();
    }

    if (isVisible() && superview()) {
        superview()->invalidateRenderCache();
    }
}

void View::_dispatchFutureVisibilityChange(bool visible) {
    if (visible) {
        willAppear();
    } else {
        willDisappear();
    }

    for (auto& subview : _subviews) {
        if (subview->isVisible()) {
            subview->_dispatchFutureVisibilityChange(visible);
        }
    }
}

void View::_dispatchVisibilityChange(bool visible) {
    if (visible) {
        appeared();
    } else {
        disappeared();
    }

    for (auto& subview : _subviews) {
        if (subview->isVisible()) {
            subview->_dispatchVisibilityChange(visible);
        }
    }
}

void View::_dispatchWindowChange(Window* window) {
    if (application()) {
        application()->removeListeners(this);
    }
    
    _window = window;
    
    if (application()) {
        for (auto& listener : _listeners) {
            application()->addListener(this, listener.index, &listener.action, listener.relation);
        }
    }

    windowChanged();

    for (auto& subview : _subviews) {
        subview->_dispatchWindowChange(window);
    }
}

void View::_mouseExit() {
    if (_subviewWithMouse && _clipsToBounds) {
        _subviewWithMouse->_mouseExit();
        _subviewWithMouse = nullptr;
    }
    mouseExit();
}

bool View::_requiresTextureRendering() {
    return _rendersToTexture || _tintColor.r < 1.0 || _tintColor.g < 1.0 || _tintColor.b < 1.0 || _tintColor.a < 1.0;
}

void View::_renderAndRenderSubviews(const RenderTarget* target, const Rectangle<int>& area, boost::optional<Rectangle<int>> clipBounds) {
    glViewport(area.x, target->height() - area.maxY(), area.width, area.height);
    glEnable(GL_BLEND);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE);

    if (_clipsToBounds) {
        clipBounds = clipBounds ? clipBounds->intersection(area) : area;
    }

    if (clipBounds) {
        glEnable(GL_SCISSOR_TEST);
        glScissor(clipBounds->x, target->height() - clipBounds->maxY(), clipBounds->width, clipBounds->height);
    } else {
        glDisable(GL_SCISSOR_TEST);
    }

    if (_backgroundColor.a) {
        auto backgroundShader = colorShader();
        backgroundShader->setColor(_backgroundColor.r, _backgroundColor.g, _backgroundColor.b, _backgroundColor.a);
        shapes::Rectangle(0.0, 0.0, bounds().width, bounds().height).draw(backgroundShader);
        backgroundShader->flush();
    }

    render();

    auto xScale = (_bounds.width ? (double)area.width / _bounds.width : 1.0);
    auto yScale = (_bounds.height ? (double)area.height / _bounds.height : 1.0);

    for (auto& subview : _subviews) {
        Rectangle<int> subarea(std::round(area.x + xScale * subview->_bounds.x),
                               std::round(area.y + yScale * subview->_bounds.y),
                               std::round(xScale * subview->_scale.x * subview->_bounds.width),
                               std::round(yScale * subview->_scale.y * subview->_bounds.height));
        subview->renderAndRenderSubviews(target, subarea, clipBounds);
    }

    if (clipBounds) {
        glDisable(GL_SCISSOR_TEST);
    }
}

void View::_post(std::type_index index, const void* ptr, View::Relation relation) {
    if (application()) {
        application()->post(this, index, ptr, relation);
    }
}

void View::_listen(std::type_index index, std::function<void(const void*, View*)> action, View::Relation relation) {
    _listeners.emplace_back(index, std::move(action), relation);
    if (application()) {
        application()->addListener(this, index, &_listeners.back().action, relation);
    }
}

void* View::_get(size_t hash) const {
    auto it = _provisions.find(hash);
    if (it != _provisions.end()) {
        return it->second;
    }
    if (superview()) {
        return superview()->_get(hash);
    }
    return nullptr;
}

AbstractTaskScheduler* View::_taskScheduler() const {
    return application()->taskScheduler();
}

}}
