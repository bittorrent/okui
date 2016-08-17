#include "onair/okui/Window.h"
#include "onair/okui/Application.h"

namespace onair {
namespace okui {

Window::Window(Application* application)
    : _application{application}
    , _deviceRenderScale{application->renderScale()}
{
    _application->addWindow(this);
}

Window::~Window() {
    // the content view should be destroyed before the window's other members
    _contentView.reset();

    _application->closeWindow(this);
    _application->removeWindow(this);
}

void Window::open() {
    if (_isOpen) { return; }
    willOpen();
    _contentView->_dispatchFutureVisibilityChange(true);
    application()->openWindow(this, _title.c_str(), _position, _width, _height);
    _isOpen = true;
    _contentView->_dispatchWindowChange(this);
    _updateContentLayout();
    _contentView->_dispatchVisibilityChange(true);
    didOpen();
}

void Window::close() {
    if (!_isOpen) { return; }
    willClose();
    _contentView->_dispatchFutureVisibilityChange(false);
    application()->closeWindow(this);
    _isOpen = false;
    _contentView->_dispatchVisibilityChange(false);
    _contentView->_dispatchWindowChange(nullptr);
    didClose();
}

void Window::setPosition(const WindowPosition& pos) {
    _position = pos;
    application()->setWindowPosition(this, pos);
}

void Window::setPosition(int x, int y) {
    setPosition({x, y});
}

void Window::setSize(int width, int height) {
    _width = width;
    _height = height;
    application()->setWindowSize(this, _width, _height);
    _updateContentLayout();
}

void Window::setTitle(std::string title) {
    _title = std::move(title);
    application()->setWindowTitle(this, _title.c_str());
}

void Window::setMenu(const Menu& menu) {
    _menu = menu;
    application()->setWindowMenu(this, menu);
}

TextureHandle Window::loadTextureResource(const std::string& name) {
    auto hashable = std::string("resource:") + name;

    if (auto hit = _textureCache.get(hashable)) {
        return hit;
    }

    auto resource = application()->loadResource(name.c_str());
    if (!resource) {
        return nullptr;
    }

    auto handle = _textureCache.add(TextureHandle{std::make_shared<FileTexture>(resource, name.c_str())}, hashable);
    _texturesToLoad.emplace_back(handle.newHandle());
    return handle;
}

TextureHandle Window::loadTextureFromMemory(std::shared_ptr<const std::string> data) {
    auto hashable = std::string("memory:") + std::to_string(reinterpret_cast<uintptr_t>(data->data())) + ":" + std::to_string(data->size());

    if (auto hit = _textureCache.get(hashable)) {
        return hit;
    }

    auto handle = _textureCache.add(TextureHandle{std::make_shared<FileTexture>(data)}, hashable);
    _texturesToLoad.emplace_back(handle.newHandle());
    return handle;
}

TextureHandle Window::loadTextureFromURL(const std::string& url) {
    auto it = _textureDownloads.find(url);
    if (it != _textureDownloads.end()) {
        if (auto texture = it->second.texture.lock()) {
            return TextureHandle{texture, it->second.handle};
        }
    }

    auto texture = std::make_shared<FileTexture>();
    auto& download = _textureDownloads[url];
    download.texture = texture;
    download.handle = TextureHandle{};
    download.download = application()->download(url);
    download.isComplete = false;
    return TextureHandle{texture, download.handle};
}

std::shared_ptr<BitmapFont> Window::loadBitmapFontResource(const char* textureName, const char* metadataName) {
    auto hashable = std::string("resource:") + textureName + "$!@#" + metadataName;

    if (auto hit = _bitmapFontCache.get(hashable)) {
        return hit;
    }

    auto texture = loadTextureResource(textureName);
    if (!texture) {
        return nullptr;
    }
    auto metadata = application()->loadResource(metadataName);
    return _bitmapFontCache.add(BitmapFont(std::move(texture), *metadata), hashable);
}

void Window::setFocus(View* focus) {
    if (focus && focus->preferredFocus()) {
        focus->preferredFocus()->focus();
        return;
    }

    if (_focus == focus || (focus && !focus->canBecomeFocus())) {
        return;
    }

    auto previousFocus = _focus;
    _focus = focus;

    for (auto view = previousFocus; view; view = view->superview()) {
        if (!focus || (focus != view && !focus->isDescendantOf(view))) {
            view->focusLost();
        }
        view->focusChanged();
    }

    for (auto view = _focus; view; view = view->superview()) {
        if (!previousFocus || (previousFocus != view && !previousFocus->isDescendantOf(view))) {
            view->focusGained();
            view->focusChanged();
        }
    }
}

bool Window::moveFocus(Direction direction) {
    if (!focus()) {
        if (initialFocus() && initialFocus()->isVisible() && initialFocus()->canBecomeFocus()) {
            initialFocus()->focus();
            return true;
        }
        return false;
    }

    std::vector<std::tuple<View*, Rectangle<double>>> focusableRegions;
    contentView()->_updateFocusableRegions(focusableRegions);
    auto previousFocus = focus();
    auto previousFocusWindowBounds = previousFocus->windowBounds();
    View* next = nullptr;
    double nextDistance = 0.0;
    double nextOverlap = 0.0;
    double nextPerpendicular = 0.0;
    for (auto& region : focusableRegions) {
        auto r = std::get<Rectangle<double>>(region);
        auto xOverlap = std::max<double>(previousFocusWindowBounds.width - std::max<double>(previousFocusWindowBounds.maxX() - r.maxX(), 0) - std::max<double>(r.minX() - previousFocusWindowBounds.minX(), 0), 0);
        auto yOverlap = std::max<double>(previousFocusWindowBounds.height - std::max<double>(previousFocusWindowBounds.maxY() - r.maxY(), 0) - std::max<double>(r.minY() - previousFocusWindowBounds.minY(), 0), 0);
        double overlap = 0.0;
        double perpendicular = 0.0;
        switch (direction) {
            case Direction::kRight:
                overlap = yOverlap;
                perpendicular = r.minY();
                if (!overlap || r.maxX() <= previousFocusWindowBounds.maxX()) { continue; }
                break;
            case Direction::kLeft:
                overlap = yOverlap;
                perpendicular = r.minY();
                if (!overlap || r.x >= previousFocusWindowBounds.x) { continue; }
                break;
            case Direction::kUp:
                overlap = xOverlap;
                perpendicular = r.minX();
                if (!overlap || r.y >= previousFocusWindowBounds.y) { continue; }
                break;
            case Direction::kDown:
                overlap = xOverlap;
                perpendicular = r.minX();
                if (!overlap || r.maxY() <= previousFocusWindowBounds.maxY()) { continue; }
                break;
            case Direction::kUpLeft:
                if (r.x >= previousFocusWindowBounds.x || r.y >= previousFocusWindowBounds.y) { continue; }
                break;
            case Direction::kUpRight:
                if (r.maxX() <= previousFocusWindowBounds.maxX() || r.y >= previousFocusWindowBounds.y) { continue; }
                break;
            case Direction::kDownLeft:
                if (r.x >= previousFocusWindowBounds.x || r.maxY() <= previousFocusWindowBounds.maxY()) { continue; }
                break;
            case Direction::kDownRight:
                if (r.maxX() <= previousFocusWindowBounds.maxX() || r.maxY() <= previousFocusWindowBounds.maxY()) { continue; }
                break;
            default:
                continue;
        }
        auto distance = r.distance(previousFocusWindowBounds);
        if (!next || distance < nextDistance || (distance == nextDistance && (overlap > nextOverlap || (overlap == nextOverlap && perpendicular < nextPerpendicular)))) {
            next = std::get<View*>(region);
            nextDistance = distance;
            nextOverlap = overlap;
            nextPerpendicular = perpendicular;
        }
    }
    if (next) {
        next->focus();
        return true;
    }

    return false;
}

void Window::openDialog(const char* title, const char* message, const std::vector<DialogButton>& buttons, std::function<void(int)> action) {
    return application()->openDialog(this, title, message, buttons, std::move(action));
}

Point<double> Window::windowToView(View* view, double x, double y) {
    if (!view) {
        return {x, y};
    }
    auto superPoint = windowToView(view->superview(), x, y);
    return view->superviewToLocal(superPoint.x, superPoint.y);
}

void Window::beginDragging(View* view) {
    SCRAPS_ASSERT(view != nullptr);
    _draggedViews.insert(view);
}

void Window::endDragging(View* view) {
    _draggedViews.erase(view);
}

void Window::subscribeToUpdates(View* view) {
    SCRAPS_ASSERT(view != nullptr);
    _viewsToSubscribeToUpdates.insert(view);
    _viewsToUnsubscribeFromUpdates.erase(view);
}

void Window::unsubscribeFromUpdates(View* view) {
    SCRAPS_ASSERT(view != nullptr);
    _viewsToSubscribeToUpdates.erase(view);
    _viewsToUnsubscribeFromUpdates.insert(view);
}

void Window::dispatchMouseDown(MouseButton button, double x, double y) {
    auto scale = (1/_renderScale) * (1/_deviceRenderScale);
    x *= scale;
    y *= scale;
    _contentView->dispatchMouseDown(button, x, y);
    _lastMouseDown = Point<double>{x, y};
}

void Window::dispatchMouseUp(MouseButton button, double x, double y) {
    auto scale = (1/_renderScale) * (1/_deviceRenderScale);
    x *= scale;
    y *= scale;
    _contentView->dispatchMouseUp(button, _lastMouseDown.x, _lastMouseDown.y, x, y);
    _draggedViews.clear();
}

void Window::dispatchMouseMovement(double x, double y) {
    auto scale = (1/_renderScale) * (1/_deviceRenderScale);
    x *= scale;
    y *= scale;
    _contentView->dispatchMouseMovement(x, y);
    for (auto& observer : _draggedViews) {
        SCRAPS_ASSERT(observer != nullptr);
        auto startPoint = windowToView(observer, _lastMouseDown.x, _lastMouseDown.y);
        auto point = windowToView(observer, x, y);
        observer->mouseDrag(startPoint.x, startPoint.y, point.x, point.y);
    }
}

void Window::dispatchMouseWheel(double xPos, double yPos, int xWheel, int yWheel) {
    auto scale = (1/_renderScale) * (1/_deviceRenderScale);
    xPos *= scale;
    yPos *= scale;
    _contentView->dispatchMouseWheel(xPos, yPos, xWheel, yWheel);
}

Responder* Window::nextResponder() {
    return application();
}

void Window::keyDown(KeyCode key, KeyModifiers mod, bool repeat) {
    if (key == KeyCode::kTab && initialFocus() && !focus()) {
        if (mod & KeyModifier::kShift) {
            if (auto view = initialFocus()->previousAvailableFocus()) {
                view->focus();
                return;
            }
        }
        if (initialFocus()->isVisible() && initialFocus()->canBecomeFocus()) {
            initialFocus()->focus();
            return;
        } else if (auto view = initialFocus()->nextAvailableFocus()) {
            view->focus();
            return;
        }
    }

    Responder::keyDown(key, mod, repeat);
}

void Window::ensureTextures() {
    for (auto it = _textureDownloads.begin(); it != _textureDownloads.end();) {
        auto& download = it->second;
        auto texture = download.texture.lock();
        if (!texture) {
            it = _textureDownloads.erase(it);
            continue;
        }
        if (!download.isComplete) {
            auto status = download.download.wait_for(0ms);
            if (status == std::future_status::ready) {
                if (auto data = download.download.get()) {
                    texture->setData(data);
                    if (texture->load(&_openGLTextureCache)) {
                        download.handle.invokeLoadCallbacks();
                    } else {
                        SCRAPS_LOG_ERROR("error loading texture: {}", it->first);
                    }
                }
                download.isComplete = true;
            }
        }
        ++it;
    }
    for (auto& texture : _texturesToLoad) {
        texture->load(&_openGLTextureCache);
        texture.invokeLoadCallbacks();
    }
    _texturesToLoad.clear();
}

void Window::_update() {
    auto now = std::chrono::high_resolution_clock::now();
    update();
    auto elapsed = now - _lastUpdateTime;
    for (auto view : _viewsToSubscribeToUpdates) {
        _updatingViews.insert(view);
    }
    _viewsToSubscribeToUpdates.clear();
    for (auto view : _viewsToUnsubscribeFromUpdates) {
        _updatingViews.erase(view);
    }
    _viewsToUnsubscribeFromUpdates.clear();
    for (auto view : _updatingViews) {
        if (!_viewsToUnsubscribeFromUpdates.count(view)) {
            view->dispatchUpdate(elapsed);
        }
    }
    _lastUpdateTime = now;
}

void Window::_render() {
    auto now = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::duration<double>>(now - _lastRenderTime).count();
    constexpr auto hysteresis = 0.5;
    _framesPerSecond = _framesPerSecond * hysteresis + 1.0 / elapsed * (1.0 - hysteresis);
    _lastRenderTime = now;

    ensureTextures();

    glDisable(GL_SCISSOR_TEST);
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    render();

    RenderTarget target(_renderWidth, _renderHeight);
    _contentView->renderAndRenderSubviews(&target, {0, 0, _renderWidth, _renderHeight});

    SCRAPS_GL_ERROR_CHECK();
}

void Window::_didResize(int width, int height) {
    _width = width;
    _height = height;
    _updateContentLayout();
}

void Window::_updateContentLayout() {
    application()->getWindowRenderSize(this, &_renderWidth, &_renderHeight);
    auto scale = (1/_renderScale) * (1/_deviceRenderScale);
    _contentView->setBounds(0, 0, _width*scale, _height*scale);
    layout();
}

} } // namespace onair::okui
