#pragma once

#include "okui/config.h"

#include "okui/DialogButton.h"
#include "okui/Direction.h"
#include "okui/Menu.h"
#include "okui/Point.h"
#include "okui/Responder.h"
#include "okui/ShaderCache.h"
#include "okui/BitmapFont.h"
#include "okui/FileTexture.h"
#include "okui/View.h"
#include "okui/TextureCache.h"

#include <future>
#include <unordered_map>
#include <unordered_set>
#include <vector>

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

    bool isMinimized() const;
    void minimize();

    bool isMaximized() const;
    void maximize();

    bool isFullscreen() const;
    void fullscreen();

    void bringToFront();

    /**
    * If the window is currently minimized/maximized/fullscreen, restoreWindow brings it back to the last windowed
    * size and position
    */
    void restoreWindow();

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

    View* contentView() const { return _contentView.get(); }
    double renderScale() const { return _renderScale; }
    void setRenderScale(double scale) { _renderScale = scale; _updateContentLayout(); }

    double framesPerSecond() const { return _framesPerSecond; }

    ShaderCache* shaderCache() { return &_shaderCache; }

    TextureHandle loadTextureResource(const std::string& name);
    TextureHandle loadTextureFromMemory(std::shared_ptr<const std::string> data);
    TextureHandle loadTextureFromURL(const std::string& url);
    std::shared_ptr<BitmapFont> loadBitmapFontResource(const char* textureName, const char* metadataName);

    View* focus() const { return _focus; }
    void setFocus(View* focus);

    bool moveFocus(Direction direction);

    Responder* firstResponder() { return _focus ? dynamic_cast<Responder*>(_focus) : dynamic_cast<Responder*>(this); }

    /**
    * If there's no focus and tab or a direction input is received, the initial focus will be focused.
    */
    View* initialFocus() const { return _initialFocus; }
    void setInitialFocus(View* focus) { _initialFocus = focus; }

    /**
    * Converts a window coordinate to a view coordinate.
    */
    Point<double> windowToView(View* view, double x, double y);

    void openDialog(
        const char* title,
        const char* message,
        const std::vector<DialogButton>& buttons,
        std::function<void(int)> action = std::function<void(int)>()
    );

    template <typename T>
    auto set(T&& object) {
        return contentView()->set(std::forward<T>(object), Relation::kHierarchy);
    }

    template <typename T>
    auto get() {
        return contentView()->get<T>();
    }

    /**
    * Asynchronously schedules a function to be invoked using the application's task scheduler.
    *
    * If the window is destroyed, the invocation will be canceled.
    */
    template <typename... Args>
    auto async(Args&&... args) -> decltype(std::declval<scraps::AbstractTaskScheduler>().async(std::forward<Args>(args)...)) {
        return contentView()->async(std::forward<Args>(args)...);
    }

    /**
    * Asynchronously schedules a function to be invoked after a delay using the application's task scheduler.
    *
    * If the window is destroyed, the invocation will be canceled.
    */
    template <typename... Args>
    auto asyncAfter(Args&&... args) -> decltype(std::declval<scraps::AbstractTaskScheduler>().asyncAfter(std::forward<Args>(args)...)) {
        return contentView()->asyncAfter(std::forward<Args>(args)...);
    }

    /**
    * Asynchronously schedules a function to be invoked at a time using the application's task scheduler.
    *
    * If the window is destroyed, the invocation will be canceled.
    */
    template <typename... Args>
    auto asyncAt(Args&&... args) -> decltype(std::declval<scraps::AbstractTaskScheduler>().asyncAt(std::forward<Args>(args)...)) {
        return contentView()->asyncAt(std::forward<Args>(args)...);
    }

    void beginDragging(View* view);
    void endDragging(View* view);

    void subscribeToUpdates(View* view);
    void unsubscribeFromUpdates(View* view);

    /**
    * Override this to perform updates for each frame. This is invoked exactly once per frame, before any views
    * begin rendering.
    */
    virtual void update() {}

    /**
    * Override this to lay out views whenever the window is resized.
    */
    virtual void layout() {}

    virtual void render() {}

    virtual void willOpen() {}
    virtual void didOpen() {}
    virtual void willClose() {}
    virtual void didClose() {}

    void dispatchMouseDown(MouseButton button, double x, double y);
    void dispatchMouseUp(MouseButton button, double x, double y);
    void dispatchMouseMovement(double x, double y);
    void dispatchMouseWheel(double xPos, double yPos, int xWheel, int yWheel);

    void ensureTextures();

    // Responder overrides
    virtual Responder* nextResponder() override;
    virtual void keyDown(KeyCode key, KeyModifiers mod, bool repeat) override;

private:
    friend class Application;

    struct TextureDownload {
        std::future<std::shared_ptr<const std::string>> download;
        TextureHandle handle;
    };

    void _update();
    void _render();
    void _didResize(int width, int height);
    void _updateContentLayout();
    void _decompressTexture(const std::string& hashable);

    std::string                  _title = "Untitled";
    Application*                 _application = nullptr;
    std::unique_ptr<View>        _contentView = std::make_unique<View>("Window::_contentView");
    View*                        _focus = nullptr;
    View*                        _initialFocus = nullptr;
    Menu                         _menu;
    bool                         _isOpen = false;

    WindowPosition               _position;
    int                          _width = 800;
    int                          _height = 480;
    int                          _renderWidth = 800;
    int                          _renderHeight = 480;
    double                       _renderScale = 1.0; // user defined scale
    double                       _deviceRenderScale = 1.0; // scale determimed by the device used to handle high dpi displays

    ShaderCache                  _shaderCache;
    scraps::Cache<TextureHandle> _textureCache;
    scraps::Cache<BitmapFont>    _bitmapFontCache;

    std::unordered_map<std::string, TextureDownload> _textureDownloads;

    std::mutex                   _texturesToLoadMutex;
    std::vector<std::string>     _texturesToLoad;

    Point<double>                _lastMouseDown{0.0, 0.0};
    std::unordered_set<View*>    _draggedViews;
    std::unordered_set<View*>    _updatingViews;
    std::unordered_set<View*>    _viewsToSubscribeToUpdates;
    std::unordered_set<View*>    _viewsToUnsubscribeFromUpdates;

    double _framesPerSecond = 0.0;
    std::chrono::high_resolution_clock::time_point _lastRenderTime = std::chrono::high_resolution_clock::now();
    std::chrono::high_resolution_clock::time_point _lastUpdateTime = std::chrono::high_resolution_clock::now();
};

} // namespace okui
