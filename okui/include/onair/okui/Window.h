#pragma once
#include "onair/okui/config.h"
#include "onair/okui/Menu.h"
#include "onair/okui/Point.h"
#include "onair/okui/Responder.h"
#include "onair/okui/ShaderCache.h"
#include "onair/okui/opengl/TextureCache.h"
#include "onair/okui/BitmapFont.h"
#include "onair/okui/PNGTexture.h"
#include "onair/okui/View.h"

#include <future>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace onair {
namespace okui {

class Application;

struct WindowPosition {
    enum class Mode {
        kUndefined,  // the os puts the window where it wants to
        kCentered,   // centered on the desktop
        kAbsolute,   // absolute position
    };

    WindowPosition() = default;
    WindowPosition(Mode mode) : mode{mode} {}
    WindowPosition(int x, int y) : mode{Mode::kAbsolute}, x{x}, y{y} {}

    Mode mode = Mode::kUndefined;
    int x = 0;
    int y = 0;
};

class Window : public Responder {
public:
    Window(Application* application);
    virtual ~Window();

    void open();
    void close();

    bool isOpen() const { return _isOpen; }

    Application* application() { return _application; }

    const WindowPosition& position() const { return _position; }

    int width() const { return _width; }
    int height() const { return _height; }

    void setPosition(const WindowPosition& pos);
    void setPosition(int x, int y);
    void setSize(int width, int height);

    const std::string& title() const { return _title; }
    void setTitle(std::string title);

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

    TextureHandle loadTextureResource(const std::string& name);
    TextureHandle loadTextureFromMemory(std::shared_ptr<const std::string> data);
    TextureHandle loadTextureFromURL(const std::string& url);
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
    void _update() { _contentView->updateAndUpdateSubviews(); }
    void _render();
    void _didResize(int width, int height);
    void _updateContentLayout();

    friend class Application;

    Application* _application;

    bool _isOpen = false;

    WindowPosition _position;
    int _width = 800;
    int _height = 480;
    int _renderWidth = 800;
    int _renderHeight = 480;
    double _renderScale = 1.0;

    std::string _title{"Untitled"};
    Menu _menu;

    std::unique_ptr<View> _contentView;
    View* _focus = nullptr;
    View* _initialFocus = nullptr;

    ShaderCache _shaderCache;
    Cache<TextureHandle> _textureCache;
    Cache<BitmapFont> _bitmapFontCache;
    std::vector<TextureHandle> _texturesToLoad;
    opengl::TextureCache _openGLTextureCache;

    struct TextureDownload {
        bool isComplete = false;
        std::weak_ptr<PNGTexture> texture;
        TextureHandle handle;
        std::future<std::shared_ptr<const std::string>> download;
    };

    std::unordered_map<std::string, TextureDownload> _textureDownloads;

    Point<int> _lastMouseDown{0, 0};
    std::unordered_set<View*> _draggedViews;
};

}}
