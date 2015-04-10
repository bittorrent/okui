#pragma once

#include "bittorrent/ui/config.h"

#include "bittorrent/ui/ShaderCache.h"
#include "bittorrent/ui/opengl/TextureCache.h"
#include "bittorrent/ui/BitmapFont.h"
#include "bittorrent/ui/Texture.h"
#include "bittorrent/ui/View.h"

#include <unordered_set>

#include <SDL2/SDL.h>

namespace bittorrent {
namespace ui {
    
class Application;

class Window {
public:
    Window(Application* application);
    virtual ~Window();
    
    void open();
    void close();
    
    Application* application() { return _application; }
    
    int width() { return _width; }
    int height() { return _height; }
    
    void position(int x, int y);
    void resize(int width, int height);

    const char* title() const { return _title.c_str(); }
    void setTitle(const char* title);

    View* contentView() { return &_contentView; }
    double renderScale() const { return _renderScale; }

    ShaderCache* shaderCache() { return &_shaderCache; }

    shared_ptr<Texture> loadTextureResource(const char* name);
    shared_ptr<BitmapFont> loadBitmapFontResource(const char* textureName, const char* metadataName);
        
    virtual void layout() {}

    void dispatchMouseDown(MouseButton button, int x, int y);
    void dispatchMouseUp(MouseButton button, int x, int y);
    void dispatchMouseMovement(int x, int y);

    virtual void render() {}

private:
    friend class Application;

    Application* const _application;
    SDL_Window* _window = nullptr;
    SDL_GLContext _context;

    int _x{100}, _y{100};
    int _width{800}, _height{480};
    
    double _renderScale = 1.0;

    std::string _title{"Untitled"};
        
    View _contentView;
    
    ShaderCache _shaderCache;
    Cache<Texture> _textureCache;
    Cache<BitmapFont> _bitmapFontCache;
    std::unordered_set<shared_ptr<Texture>> _texturesToLoad;
    opengl::TextureCache _openGLTextureCache;

    void _render();
    void _didResize(int width, int height);
    void _updateContentLayout();
};

}}