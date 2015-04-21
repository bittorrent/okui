#pragma once

#include "bittorrent/ui/config.h"

#include "bittorrent/ui/ShaderCache.h"
#include "bittorrent/ui/opengl/TextureCache.h"
#include "bittorrent/ui/BitmapFont.h"
#include "bittorrent/ui/Texture.h"
#include "bittorrent/ui/View.h"

#include <unordered_set>

namespace bittorrent {
namespace ui {
    
class Application;
class Platform;

class Window {
public:
    Window(Application* application);
    virtual ~Window();
    
    void open();
    void close();
    
    Application* application() { return _application; }
    
    int x() const { return _x; }
    int y() const { return _x; }

    int width() const { return _width; }
    int height() const { return _height; }
    
    void setPosition(int width, int height);
    void setSize(int width, int height);

    const char* title() const { return _title.c_str(); }
    void setTitle(const char* title);

    View* contentView() { return &_contentView; }
    double renderScale() const { return _renderScale; }

    ShaderCache* shaderCache() { return &_shaderCache; }

    shared_ptr<Texture> loadTextureResource(const char* name);
    shared_ptr<BitmapFont> loadBitmapFontResource(const char* textureName, const char* metadataName);

    View* focus() const { return _focus; }
    void setFocus(View* focus);

    virtual void layout() {}

    void dispatchMouseDown(MouseButton button, int x, int y);
    void dispatchMouseUp(MouseButton button, int x, int y);
    void dispatchMouseMovement(int x, int y);

    virtual void render() {}
        
    void ensureTextures();

private:
    Application* const _application;

    int _x{100}, _y{100};
    int _width{800}, _height{480};
    
    double _renderScale = 1.0;

    std::string _title{"Untitled"};
        
    View _contentView;
    View* _focus = nullptr;
    
    ShaderCache _shaderCache;
    Cache<Texture> _textureCache;
    Cache<BitmapFont> _bitmapFontCache;
    std::unordered_set<shared_ptr<Texture>> _texturesToLoad;
    opengl::TextureCache _openGLTextureCache;

    friend class Platform;
    void _render();
    void _didResize(int width, int height);

    void _updateContentLayout();
};

}}