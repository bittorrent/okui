#include "onair/okui/Window.h"
#include "onair/okui/Application.h"

namespace onair {
namespace okui {

Window::Window(Application* application) : _application(application) {
    _contentView.reset(new View());
    _contentView->_window = this;
}

Window::~Window() {
    close();

    // the content view should be destroyed before the window's other members
    _contentView.reset();
}

void Window::open() {
    _contentView->_dispatchFutureVisibilityChange(true);
    application()->openWindow(this, _title.c_str(), _position, _width, _height);
    _isOpen = true;
    _updateContentLayout();
    _contentView->_dispatchVisibilityChange(true);
}

void Window::close() {
    closing();
    _contentView->_dispatchFutureVisibilityChange(false);
    application()->closeWindow(this);
    _isOpen = false;
    _contentView->_dispatchVisibilityChange(false);
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
        return hit->newHandle();
    }

    auto resource = application()->loadResource(name.c_str());
    if (!resource) {
        return nullptr;
    }
    auto handle = _textureCache.add(TextureHandle{std::make_shared<PNGTexture>(resource)}, hashable);
    _texturesToLoad.emplace_back(handle->newHandle());
    return handle->newHandle();
}

TextureHandle Window::loadTextureFromMemory(std::shared_ptr<const std::string> data) {
    auto hashable = std::string("memory:") + std::to_string(reinterpret_cast<uintptr_t>(data->data())) + ":" + std::to_string(data->size());

    if (auto hit = _textureCache.get(hashable)) {
        return hit->newHandle();
    }

    auto handle = _textureCache.add(TextureHandle{std::make_shared<PNGTexture>(data)}, hashable);
    _texturesToLoad.emplace_back(handle->newHandle());
    return handle->newHandle();
}

TextureHandle Window::loadTextureFromURL(const std::string& url) {
    auto it = _textureDownloads.find(url);
    if (it != _textureDownloads.end()) {
        if (auto texture = it->second.texture.lock()) {
            return TextureHandle{texture, it->second.handle};
        }
    }

    auto texture = std::make_shared<PNGTexture>();
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
    if (_focus == focus || (focus && !focus->canBecomeFocus())) { return; }
    if (_focus) {
        _focus->focusLost();
    }
    _focus = focus;
    if (_focus) {
        _focus->focusGained();
    }
}

Point<double> Window::windowToView(View* view, double x, double y) {
    if (!view) {
        return {x, y};
    }
    auto superPoint = windowToView(view->superview(), x, y);
    return view->superviewToLocal(superPoint.x, superPoint.y);
}

void Window::beginDragging(View* view) {
    _draggedViews.insert(view);
}

void Window::endDragging(View* view) {
    _draggedViews.erase(view);
}

void Window::dispatchMouseDown(MouseButton button, double x, double y) {
    x *= _renderScale;
    y *= _renderScale;
    _contentView->dispatchMouseDown(button, x, y);
    _lastMouseDown = Point<double>{x, y};
}

void Window::dispatchMouseUp(MouseButton button, double x, double y) {
    x *= _renderScale;
    y *= _renderScale;
    _contentView->dispatchMouseUp(button, _lastMouseDown.x, _lastMouseDown.y, x, y);
    _draggedViews.clear();
}

void Window::dispatchMouseMovement(double x, double y) {
    x *= _renderScale;
    y *= _renderScale;
    _contentView->dispatchMouseMovement(x, y);
    for (auto& observer : _draggedViews) {
        auto startPoint = windowToView(observer, _lastMouseDown.x, _lastMouseDown.y);
        auto point = windowToView(observer, x, y);
        observer->mouseDrag(startPoint.x, startPoint.y, point.x, point.y);
    }
}

void Window::dispatchMouseWheel(double xPos, double yPos, int xWheel, int yWheel) {
    xPos *= _renderScale;
    yPos *= _renderScale;
    _contentView->dispatchMouseWheel(xPos, yPos, xWheel, yWheel);
}

Responder* Window::nextResponder() {
    return application();
}

void Window::keyDown(KeyCode key, KeyModifiers mod, bool repeat) {
    if (key == KeyCode::kTab && _initialFocus && !_focus) {
        if (mod & KeyModifier::kShift) {
            if (auto view = _initialFocus->previousAvailableFocus()) {
                view->focus();
                return;
            }
        }
        if (_initialFocus->isVisible() && _initialFocus->canBecomeFocus()) {
            _initialFocus->focus();
            return;
        } else if (auto view = _initialFocus->nextAvailableFocus()) {
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
            auto status = download.download.wait_for(0_ms);
            if (status == std::future_status::ready) {
                if (auto data = download.download.get()) {
                    texture->setData(data);
                    texture->load(&_openGLTextureCache);
                    download.handle.invokeLoadCallbacks();
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
    update();
    _contentView->updateAndUpdateSubviews();
}

void Window::_render() {
    auto now = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::duration<double>>(now - _lastRenderTime).count();
    constexpr auto hysteresis = 0.5;
    _framesPerSecond = _framesPerSecond * hysteresis + 1.0 / elapsed * (1.0 - hysteresis);
    _lastRenderTime = now;

    ensureTextures();

    render();

    RenderTarget target(_renderWidth, _renderHeight);
    _contentView->renderAndRenderSubviews(&target, {0, 0, _renderWidth, _renderHeight});

    ONAIR_OKUI_GL_ERROR_CHECK();
}

void Window::_didResize(int width, int height) {
    _width = width;
    _height = height;
    _updateContentLayout();
}

void Window::_updateContentLayout() {
    application()->getWindowRenderSize(this, &_renderWidth, &_renderHeight);
    _contentView->setBounds(0, 0, _width * _renderScale, _height * _renderScale);
    layout();
}

}}
