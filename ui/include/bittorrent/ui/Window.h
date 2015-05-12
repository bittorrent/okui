#pragma once
#include "bittorrent/ui/config.h"
#include "bittorrent/ui/Keycode.h"
#include "bittorrent/ui/Point.h"
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
    void setRenderScale(double scale) { _renderScale = scale; }

    ShaderCache* shaderCache() { return &_shaderCache; }

    std::shared_ptr<Texture> loadTextureResource(const char* name);
    std::shared_ptr<Texture> loadTextureFromMemory(std::shared_ptr<const std::string> data);
    std::shared_ptr<BitmapFont> loadBitmapFontResource(const char* textureName, const char* metadataName);

    View* focus() const { return _focus; }
    void setFocus(View* focus);

    /**
    * If tab is hit when there's no focus, the specified view will be focused.
    */
    void setInitialFocus(View* focus) { _initialFocus = focus; }

    /**
    * Converts a window coordinate to a view coordinate.
    */
    Point<int> windowToView(View* view, int x, int y);

    void beginDragging(View* view);
    void endDragging(View* view);

    virtual void layout() {}

    void dispatchMouseDown(MouseButton button, int x, int y);
    void dispatchMouseUp(MouseButton button, int x, int y);
    void dispatchMouseMovement(int x, int y);
    void dispatchMouseWheel(int xPos, int yPos, int xWheel, int yWheel);
    void dispatchTextInput(const std::string& text);
    void dispatchKeyDown(Keycode key, KeyModifiers mod, bool repeat);
    void dispatchKeyUp(Keycode key, KeyModifiers mod, bool repeat);

    /**
    * Override these to handle keyboard events. Call the base implementation to pass on the event.
    */
    virtual void keyDown(Keycode key, KeyModifiers mod, bool repeat);
    virtual void keyUp(Keycode key, KeyModifiers mod, bool repeat) {}

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
    View* _initialFocus = nullptr;

    ShaderCache _shaderCache;
    Cache<Texture> _textureCache;
    Cache<BitmapFont> _bitmapFontCache;
    std::unordered_set<std::shared_ptr<Texture>> _texturesToLoad;
    opengl::TextureCache _openGLTextureCache;

    std::unordered_set<View*> _draggedViews;

    friend class Platform;
    void _render();
    void _didResize(int width, int height);

    void _updateContentLayout();
};

}}
