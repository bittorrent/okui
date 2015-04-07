#ifdef BT_USE_UI_VIEWS

#include "bittorrent/ui/Window.h"

#include "bittorrent/ui/Application.h"

namespace bittorrent {
namespace ui {

Window::Window(Application* application) : _application(application) {
    _contentView._window = this;
}

Window::~Window() {
	close();
}

void Window::open() {
	if (_window) { return; }

	_window = SDL_CreateWindow(_title.c_str(), _x, _y, _width, _height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
	_context = SDL_GL_CreateContext(_window);

	_updateContentLayout();
	application()->_windows[SDL_GetWindowID(_window)] = this;
}

void Window::close() {
	if (!_window) { return; }

	application()->_windows.erase(SDL_GetWindowID(_window));

	SDL_GL_DeleteContext(_context);
	SDL_DestroyWindow(_window);

	_window = nullptr;
}

void Window::position(int x, int y) {
	_x = x;
	_y = y;
	if (_window) {
		SDL_SetWindowPosition(_window, x, y);
	}
}

void Window::resize(int width, int height) {
	_width = width;
	_height = height;
	if (_window) {
		SDL_SetWindowSize(_window, width, height);
	}
}

void Window::setTitle(const char* title) {
    _title = title;
    if (_window) {
		SDL_SetWindowTitle(_window, title);
    }
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

void Window::dispatchMouseDown(MouseButton button, int x, int y) {
    _contentView.dispatchMouseDown(button, x, y);
}

void Window::dispatchMouseUp(MouseButton button, int x, int y) {
    _contentView.dispatchMouseUp(button, x, y);
}

void Window::render() {
	SDL_GL_MakeCurrent(_window, _context);
	
	for (auto& texture : _texturesToLoad) {
		texture->load(&_openGLTextureCache);
	}
	_texturesToLoad.clear();
	
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	
	Rectangle<int> viewport(0, 0, _contentView.bounds().width, _contentView.bounds().height);
	_contentView.renderAndRenderSubviews(viewport, renderScale());
	
	SDL_GL_SwapWindow(_window);
}

void Window::_didResize(int width, int height) {
	_width = width;
	_height = height;
	_updateContentLayout();
}

void Window::_updateContentLayout() {
	int w, h;
	SDL_GL_GetDrawableSize(_window, &w, &h);
	_renderScale = (double)w / _width;
	_contentView.setBounds(0, 0, _width, _height);
	layout();
}

}}

#endif