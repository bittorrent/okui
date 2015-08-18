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
    auto requestedWidth = _width;
    application()->openWindow(this, _title.c_str(), _position, _width, _height);
    // application reset _width/_height to what the OS actually set the window to in setWindowSize
    _renderScale = requestedWidth / static_cast<double>(_width);
    _isOpen = true;
    _updateContentLayout();
}

void Window::close() {
    closing();
    application()->closeWindow(this);
    _isOpen = false;
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
    application()->setWindowSize(this, width, height);
    // application reset _width/_height to what the OS actually set the window to in setWindowSize
    _renderScale = width / static_cast<double>(_width);
    _updateContentLayout();
}

void Window::setTitle(const char* title) {
    _title = title;
    application()->setWindowTitle(this, title);
}

void Window::setMenu(const Menu& menu) {
    _menu = menu;
    application()->setWindowMenu(this, menu);
}

TextureHandle Window::loadTextureResource(const char* name) {
    auto hashable = std::string("resource:") + name;

    if (auto hit = _textureCache.get(hashable)) {
        return hit->newHandle();
    }

    auto resource = application()->loadResource(name);
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

Point<int> Window::windowToView(View* view, int x, int y) {
    if (!view) {
        return Point<int>(x, y);
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

void Window::dispatchMouseDown(MouseButton button, int x, int y) {
    x *= _renderScale;
    y *= _renderScale;
    _contentView->dispatchMouseDown(button, x, y);
    _lastMouseDown = Point<int>{x, y};
}

void Window::dispatchMouseUp(MouseButton button, int x, int y) {
    x *= _renderScale;
    y *= _renderScale;
    _contentView->dispatchMouseUp(button, _lastMouseDown.x, _lastMouseDown.y, x, y);
    _draggedViews.clear();
}

void Window::dispatchMouseMovement(int x, int y) {
    x *= _renderScale;
    y *= _renderScale;
    _contentView->dispatchMouseMovement(x, y);
    for (auto& observer : _draggedViews) {
        auto startPoint = windowToView(observer, _lastMouseDown.x, _lastMouseDown.y);
        auto point = windowToView(observer, x, y);
        observer->mouseDrag(startPoint.x, startPoint.y, point.x, point.y);
    }
}

void Window::dispatchMouseWheel(int xPos, int yPos, int xWheel, int yWheel) {
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

void Window::_render() {
    ensureTextures();

    render();

    Rectangle<double> area(0, 0, _renderWidth, _renderHeight);
    RenderTarget target(area.width, area.height);
    _contentView->renderAndRenderSubviews(&target, area);
}

void Window::_didResize(int width, int height) {
    _width = width;
    _height = height;
    _updateContentLayout();
}

void Window::_updateContentLayout() {
    application()->getWindowRenderSize(this, &_renderWidth, &_renderHeight);
    _contentView->setBounds(0, 0, _renderScale * _width, _renderScale * _height);
    layout();
}

}}
