#pragma once
#include "onair/okui/config.h"
#include "onair/okui/Menu.h"
#include "onair/okui/Point.h"
#include "onair/okui/Responder.h"
#include "onair/okui/ShaderCache.h"
#include "onair/okui/opengl/TextureCache.h"
#include "onair/okui/BitmapFont.h"
#include "onair/okui/Texture.h"
#include "onair/okui/View.h"

#include <future>
#include <unordered_map>
#include <unordered_set>

namespace onair {
namespace okui {

class Application;
class Platform;

class Window : public Responder {
public:
    Window(Application* application);
    virtual ~Window();

    void open();
    void close();

    Application* application() { return _application; }

    int x() const { return _x; }
    int y() const { return _y; }

    int width() const { return _width; }
    int height() const { return _height; }

    void setPosition(int width, int height);
    void setSize(int width, int height);

    const char* title() const { return _title.c_str(); }
    void setTitle(const char* title);

    /**
    * Sets the menu for the window.
    * 
    * For good portability, you should also set an application-wide menu that can be used on platforms that prefer 
    * them over window-specific menus (or simply don't support window-specific menus). (see Application::setMenu).
    */
    void setMenu(const Menu& menu);
    const Menu& menu() const { return _menu; }

    View* contentView() { return _contentView.get(); }
    double renderScale() const { return _renderScale; }
    void setRenderScale(double scale) { _renderScale = scale; }

    ShaderCache* shaderCache() { return &_shaderCache; }

    std::shared_ptr<Texture> loadTextureResource(const char* name);
    std::shared_ptr<Texture> loadTextureFromMemory(std::shared_ptr<const std::string> data);
    std::shared_ptr<Texture> loadTextureFromURL(const std::string& url);
    std::shared_ptr<BitmapFont> loadBitmapFontResource(const char* textureName, const char* metadataName);

    View* focus() const { return _focus; }
    void setFocus(View* focus);
    
    Responder* firstResponder() { return _focus ? dynamic_cast<Responder*>(_focus) : dynamic_cast<Responder*>(this); }

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

    virtual void render() {}

    virtual void closing() {}

    void ensureTextures();

    // Responder overrides
    virtual Responder* nextResponder() override;
    virtual void keyDown(KeyCode key, KeyModifiers mod, bool repeat) override;

private:
    Application* const _application;

    int _x{100}, _y{100};
    int _width{800}, _height{480};

    double _renderScale = 1.0;

    std::string _title{"Untitled"};
    Menu _menu;

    std::unique_ptr<View> _contentView;
    View* _focus = nullptr;
    View* _initialFocus = nullptr;

    ShaderCache _shaderCache;
    Cache<Texture> _textureCache;
    Cache<BitmapFont> _bitmapFontCache;
    std::unordered_set<std::shared_ptr<Texture>> _texturesToLoad;
    opengl::TextureCache _openGLTextureCache;

    struct TextureDownload {
        bool isComplete = false;
        std::weak_ptr<Texture> texture;
        std::future<std::shared_ptr<const std::string>> download;
    };

    std::unordered_map<std::string, TextureDownload> _textureDownloads;

    std::unordered_set<View*> _draggedViews;

    friend class Platform;
    void _render();
    void _didResize(int width, int height);

    void _updateContentLayout();
};

}}
