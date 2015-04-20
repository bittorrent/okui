#include "bittorrent/ui/Window.h"

#include "bittorrent/ui/Application.h"
#include "bittorrent/ui/Platform.h"

namespace bittorrent {
namespace ui {

Window::Window(Application* application) : _application(application) {
    _contentView._window = this;
}

Window::~Window() {
    close();
}

void Window::open() {
    application()->platform()->openWindow(this, _title.c_str(), _x, _y, _width, _height);
    _updateContentLayout();
}

void Window::close() {
    application()->platform()->closeWindow(this);
}

void Window::position(int x, int y) {
    _x = x;
    _y = y;
    application()->platform()->setWindowPosition(this, x, y);
}

void Window::resize(int width, int height) {
    _width = width;
    _height = height;
    application()->platform()->setWindowSize(this, width, height);
}

void Window::setTitle(const char* title) {
    _title = title;
    application()->platform()->setWindowTitle(this, title);
}

shared_ptr<Texture> Window::loadTextureResource(const char* name) {
    auto hashable = std::string("resource:") + name;
    
    if (auto hit = _textureCache.get(hashable)) {
        return hit;
    }
    
    auto resource = application()->loadResource(name);
    auto ret = _textureCache.add(Texture(resource), hashable);
    _texturesToLoad.insert(ret);
    return ret;
}

shared_ptr<BitmapFont> Window::loadBitmapFontResource(const char* textureName, const char* metadataName) {
    auto hashable = std::string("resource:") + textureName + "$!@#" + metadataName;
    
    if (auto hit = _bitmapFontCache.get(hashable)) {
        return hit;
    }
    
    auto texture = loadTextureResource(textureName);
    auto metadata = application()->loadResource(metadataName);
    return _bitmapFontCache.add(BitmapFont(texture, *metadata), hashable);
}

void Window::setFocus(View* focus) {
    if (_focus == focus) { return; }
    if (_focus) {
        _focus->focusLost();
    }
    _focus = focus;
    if (_focus) {
        _focus->focusGained();
    }
}

void Window::dispatchMouseDown(MouseButton button, int x, int y) {
    _contentView.dispatchMouseDown(button, x, y);
}

void Window::dispatchMouseUp(MouseButton button, int x, int y) {
    _contentView.dispatchMouseUp(button, x, y);
}

void Window::dispatchMouseMovement(int x, int y) {
    _contentView.dispatchMouseMovement(x, y);
}

void Window::ensureTextures() {
    for (auto& texture : _texturesToLoad) {
        texture->load(&_openGLTextureCache);
    }
    _texturesToLoad.clear();
}

void Window::_render() {    
    ensureTextures();

    render();
    
    Rectangle<int> viewport(0, 0, _contentView.bounds().width, _contentView.bounds().height);
    _contentView.renderAndRenderSubviews(viewport, renderScale());    
}

void Window::_didResize(int width, int height) {
    _width = width;
    _height = height;
    _updateContentLayout();
}

void Window::_updateContentLayout() {
    int w, h;
    application()->platform()->getWindowRenderSize(this, &w, &h);
    _renderScale = (double)w / _width;
    _contentView.setBounds(0, 0, _width, _height);
    layout();
}

}}
