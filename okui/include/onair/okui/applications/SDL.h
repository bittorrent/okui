#pragma once

#include "onair/okui/config.h"
#include "onair/okui/FileResourceManager.h"

#include "onair/okui/applications/Native.h"
#include "onair/okui/applications/SDLKeycode.h"

#include "onair/Timer.h"

#include <unordered_map>

#include <SDL2/SDL.h>

#if ONAIR_MAC_OS_X
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wextern-c-compat"
#include <SDL2/SDL_syswm.h>
#pragma clang diagnostic pop

@interface OKUISDLApplication : NSApplication
@property onair::okui::Application* app;
@end
#endif // ONAIR_MAC_OS_X

namespace onair {
namespace okui {
namespace applications {

class SDL : public Native {
public:
    SDL(std::string name, std::string organization, ResourceManager* resourceManager = nullptr, bool shouldInitialize = true);
    ~SDL();

    virtual void initialize() override;

    virtual void run() override;
    virtual void quit() override;

    virtual void openWindow(Window* window, const char* title, const WindowPosition& windowPosition, int width, int height) override;
    virtual void closeWindow(Window* window) override;

    virtual void getWindowRenderSize(Window* window, int* width, int* height) override;
    virtual void getWindowSize(Window* window, int* width, int* height) override;

    virtual void setWindowPosition(Window* window, const WindowPosition& pos) override;
    virtual void setWindowSize(Window* window, int width, int height) override;
    virtual void setWindowTitle(Window* window, const char* title) override;

    virtual Window* activeWindow() override { return _activeWindow; }

#if !ONAIR_APPLE // TODO: fix Application heirarchy
    virtual std::string userStoragePath() const override;
#endif

    virtual void setClipboardText(const char* text) override;
    virtual std::string getClipboardText() override;

    virtual void startTextInput() override;
    virtual void stopTextInput() override;

    virtual std::string operatingSystem() const override;
    virtual void setScreenSaverEnabled(bool enabled = true) override { enabled ? SDL_EnableScreenSaver() : SDL_DisableScreenSaver(); }

    virtual void setCursorType(CursorType type) override;

#if ONAIR_MAC_OS_X
    NSWindow* nativeWindow(Window* window) const override;
#endif

private:
    struct WindowInfo {
        WindowInfo() = default;
        WindowInfo(Window* window, SDL_Window* sdlWindow, SDL_GLContext& context)
            : window(window), sdlWindow(sdlWindow), context(context) {}

        Window* window = nullptr;
        SDL_Window* sdlWindow = nullptr;
        SDL_GLContext context;
    };

    struct SDLWindowPosition {
        SDLWindowPosition() = default;
        SDLWindowPosition(const WindowPosition& pos);

        int x = 0;
        int y = 0;
    };

    struct CursorDeleter {
        void operator()(SDL_Cursor* p) {
            SDL_FreeCursor(p);
        }
    };

    Window* _window(uint32_t id) const;
    SDL_Window* _sdlWindow(Window* window) const;

    void _handleMouseMotionEvent (const SDL_MouseMotionEvent& e);
    void _handleMouseButtonEvent (const SDL_MouseButtonEvent& e);
    void _handleMouseWheelEvent  (const SDL_MouseWheelEvent& e);
    void _handleWindowEvent      (const SDL_WindowEvent& e);
    void _handleKeyboardEvent    (const SDL_KeyboardEvent& e);
    void _handleTextInputEvent   (const SDL_TextInputEvent& e);
    void _handleMultiGestureEvent(const SDL_MultiGestureEvent& e);

    std::unordered_map<Window*, uint32_t> _windowIds;
    std::unordered_map<uint32_t, WindowInfo> _windows;
    Window* _activeWindow = nullptr;
    std::unique_ptr<SDL_Cursor, CursorDeleter> _cursor;
    bool _backgrounded = false;

    std::unique_ptr<ResourceManager> _resourceManager;

    static MouseButton sMouseButton(uint8_t id);
};

inline SDL::SDL(std::string name, std::string organization, ResourceManager* resourceManager, bool shouldInitialize)
    : Native(std::move(name), std::move(organization), resourceManager, false)
{
#if ONAIR_MAC_OS_X
    // make sure we use our application class instead of sdl's
    ((OKUISDLApplication*)[OKUISDLApplication sharedApplication]).app = this;
    [NSApp finishLaunching];
#endif

    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        ONAIR_LOGF_ERROR("error initializing sdl: %s", SDL_GetError());
    }

#if OPENGL_ES
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
#else
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
#endif

#if ONAIR_ANDROID
    auto env = reinterpret_cast<JNIEnv*>(SDL_AndroidGetJNIEnv());
    auto activity = reinterpret_cast<jobject>(SDL_AndroidGetActivity());
    if (activity) {
        setActivity(env, activity);
        env->DeleteLocalRef(activity);
    } else {
        ONAIR_LOGF_ERROR("unable to get android activity");
    }
#endif

    if (shouldInitialize) {
        initialize();
    }
}

inline SDL::~SDL() {
    SDL_Quit();
}

inline void SDL::initialize() {
    if (!resourceManager()) {
        if (auto path = SDL_GetBasePath()) {
            _resourceManager.reset(new FileResourceManager(path));
            ONAIR_LOGF_DEBUG("using resources in %s", path);
            setResourceManager(_resourceManager.get());
            SDL_free(path);
        }
    }

    Native::initialize();
}

inline void SDL::run() {
    SDL_SetThreadPriority(SDL_THREAD_PRIORITY_HIGH);

    SDL_Event e;
    bool shouldQuit = false;

    static constexpr auto minFrameInterval = 1000_ms/60;
    onair::SteadyTimer timer;
    timer.start();

    while (!shouldQuit) {
#if __APPLE__
        @autoreleasepool {
#endif

        while(!shouldQuit) {
            if (SDL_PollEvent(&e)) {
                switch (e.type) {
                    case SDL_QUIT:                    { shouldQuit = true; break; }
                    case SDL_MOUSEMOTION:             { _handleMouseMotionEvent(e.motion); break; }
                    case SDL_MOUSEBUTTONUP:
                    case SDL_MOUSEBUTTONDOWN:         { _handleMouseButtonEvent(e.button); break; }
                    case SDL_MOUSEWHEEL:              { _handleMouseWheelEvent(e.wheel); break; }
                    case SDL_WINDOWEVENT:             { _handleWindowEvent(e.window); break; }
                    case SDL_KEYUP:
                    case SDL_KEYDOWN:                 { _handleKeyboardEvent(e.key); break; }
                    case SDL_TEXTEDITING:             { break; /* TODO for better localization support */ }
                    case SDL_TEXTINPUT:               { _handleTextInputEvent(e.text); break; }
                    case SDL_APP_TERMINATING:         { terminating(); break; }
                    case SDL_APP_LOWMEMORY:           { lowMemory(); break; }
                    case SDL_APP_WILLENTERBACKGROUND: { enteringBackground(); break; }
                    case SDL_APP_DIDENTERBACKGROUND:  { enteredBackground(); _backgrounded = true; break; }
                    case SDL_APP_WILLENTERFOREGROUND: { enteringForeground(); break; }
                    case SDL_APP_DIDENTERFOREGROUND:  { enteredForeground(); _backgrounded = false; break; }
                    case SDL_MULTIGESTURE:            { _handleMultiGestureEvent(e.mgesture); break; }
                    default:                          { break; }
                }
            } else if ((minFrameInterval - timer.elapsed()) > 2_ms) {
                std::this_thread::sleep_for(1_ms);
            } else {
                break;
            }
        }

        if (shouldQuit) { break; }

        timer.restart();

        taskScheduler()->run();

        for (auto& kv : _windows) {
            _update(kv.second.window);

            if (!_backgrounded) {
                SDL_GL_MakeCurrent(kv.second.sdlWindow, kv.second.context);
                glDisable(GL_SCISSOR_TEST);
                glClearColor(0.0, 0.0, 0.0, 0.0);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
                _render(kv.second.window);
                SDL_GL_SwapWindow(kv.second.sdlWindow);
            }
        }

#if __APPLE__
        } // @autoreleasepool
#endif
    }
}

inline void SDL::quit() {
    SDL_Event event;
    event.type = SDL_QUIT;
    SDL_PushEvent(&event);
}

inline void SDL::openWindow(Window* window, const char* title, const WindowPosition& position, int width, int height) {
    if (_windowIds.count(window)) {
        return;
    }

    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    auto pos = SDLWindowPosition{position};
    auto sdlWindow = SDL_CreateWindow(title, pos.x, pos.y, width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    auto context = SDL_GL_CreateContext(sdlWindow);
    auto id = SDL_GetWindowID(sdlWindow);

    _windowIds[window] = id;
    _windows[id] = WindowInfo(window, sdlWindow, context);

    //  If the OS wasn't able to use the size we passed, assign the actual size that was created in that window
    _assignWindowSize(window);
}

inline void SDL::closeWindow(Window* window) {
    auto it = _windowIds.find(window);
    if (it == _windowIds.end()) {
        return;
    }

    auto id = it->second;
    auto& info = _windows[id];

    SDL_GL_DeleteContext(info.context);
    SDL_DestroyWindow(info.sdlWindow);

    _windows.erase(id);
    _windowIds.erase(window);
}

inline void SDL::getWindowRenderSize(Window* window, int* width, int* height) {
    if (auto w = _sdlWindow(window)) {
        SDL_GL_GetDrawableSize(w, width, height);
    }
}

inline void SDL::getWindowSize(Window* window, int* width, int* height) {
    if (auto w = _sdlWindow(window)) {
        SDL_GetWindowSize(w, width, height);
    }
}

inline void SDL::setWindowPosition(Window* window, const WindowPosition& position) {
    if (auto w = _sdlWindow(window)) {
        auto pos = SDLWindowPosition{position};
        SDL_SetWindowPosition(w, pos.x, pos.y);
    }
}

inline void SDL::setWindowSize(Window* window, int width, int height) {
    if (auto w = _sdlWindow(window)) {
        SDL_SetWindowSize(w, width, height);

        //  If the OS wasn't able to use the size we passed, assign the actual size the window was set to
        //  in window->_width and window->_height
        _assignWindowSize(window);
    }
}

inline void SDL::setWindowTitle(Window* window, const char* title) {
    if (auto w = _sdlWindow(window)) {
        SDL_SetWindowTitle(w, title);
    }
}

#if !ONAIR_APPLE // TODO: fix Application heirarchy
inline std::string SDL::userStoragePath() const {
    auto path = SDL_GetPrefPath(organization().c_str(), name().c_str());
    std::string ret(path);
    SDL_free(path);
    return ret;
}
#endif

inline void SDL::setClipboardText(const char* text) {
     SDL_SetClipboardText(text);
}

inline std::string SDL::getClipboardText() {
    auto text = SDL_GetClipboardText();
    auto string = std::string{text};
    SDL_free(text);
    return string;
}

inline void SDL::startTextInput() {
    SDL_StartTextInput();
}

inline void SDL::stopTextInput() {
    SDL_StopTextInput();
}

inline std::string SDL::operatingSystem() const {
    auto native = Native::operatingSystem();
    return native.empty() ? SDL_GetPlatform() : native;
}

#if ONAIR_MAC_OS_X
inline NSWindow* SDL::nativeWindow(Window* window) const {
    if (auto w = _sdlWindow(window)) {
        // XXX: SDL_SysWMinfo's default constructor and destructor are deleted...
        union Hack {
            Hack() {}
            ~Hack() {}
            SDL_SysWMinfo info;
        };
        Hack hack;
        auto& info = hack.info;
        SDL_VERSION(&info.version);
        if (SDL_GetWindowWMInfo(w, &info) != SDL_TRUE) {
            return nil;
        }
        ONAIR_ASSERT(info.subsystem == SDL_SYSWM_COCOA);
        if (info.subsystem != SDL_SYSWM_COCOA) {
            return nil;
        }
        return info.info.cocoa.window;
    }
    return nil;
}
#endif

inline Window* SDL::_window(uint32_t id) const {
    auto it = _windows.find(id);
    return it == _windows.end() ? nullptr : it->second.window;
}

inline SDL_Window* SDL::_sdlWindow(Window* window) const {
    auto it = _windowIds.find(window);
    if (it == _windowIds.end()) {
        return nullptr;
    }
    return _windows.find(it->second)->second.sdlWindow;
}

inline void SDL::_handleMouseMotionEvent(const SDL_MouseMotionEvent& event) {
    auto window = _window(event.windowID);
    if (!window) { return; }

    switch (event.type) {
        case SDL_MOUSEMOTION:
            window->dispatchMouseMovement(event.x, event.y);
            break;
        default:
            break;
    }
}

inline void SDL::_handleMouseButtonEvent(const SDL_MouseButtonEvent& event) {
    auto window = _window(event.windowID);
    if (!window) { return; }

    switch (event.type) {
        case SDL_MOUSEBUTTONDOWN:
            window->dispatchMouseDown(sMouseButton(event.button), event.x, event.y);
            break;
        case SDL_MOUSEBUTTONUP:
            window->dispatchMouseUp(sMouseButton(event.button), event.x, event.y);
            break;
        default:
            break;
    }
}

inline void SDL::_handleMouseWheelEvent(const SDL_MouseWheelEvent& event) {
    auto window = _window(event.windowID);
    if (!window) { return; }

    switch (event.type) {
        case SDL_MOUSEWHEEL: {
            int xPos = 0, yPos = 0;
            SDL_GetMouseState(&xPos, &yPos);
            // TODO: invert scrolling per-os
            window->dispatchMouseWheel(xPos, yPos, -event.x, event.y);
            break;
        }
        default:
            break;
    }
}

inline void SDL::_handleWindowEvent(const SDL_WindowEvent& event) {
    auto window = _window(event.windowID);
    if (!window) { return; }

    switch (event.event) {
        case SDL_WINDOWEVENT_RESIZED:
            _didResize(window, event.data1, event.data2);
            break;
        case SDL_WINDOWEVENT_FOCUS_GAINED:
            _activeWindow = window;
            break;
        case SDL_WINDOWEVENT_FOCUS_LOST:
            if (_activeWindow == window) {
                _activeWindow = nullptr;
            }
            break;
        case SDL_WINDOWEVENT_CLOSE:
            window->close();
            break;
        default:
            break;
    }
}

inline void SDL::_handleKeyboardEvent(const SDL_KeyboardEvent& event) {
    auto window = _window(event.windowID);
    if (!window) { return; }

    switch (event.type) {
        case SDL_KEYDOWN:
            window->firstResponder()->keyDown(ConvertKeyCode(event.keysym.sym), ConvertKeyModifiers(event.keysym.mod), event.repeat);
            break;
        case SDL_KEYUP:
            window->firstResponder()->keyUp(ConvertKeyCode(event.keysym.sym), ConvertKeyModifiers(event.keysym.mod), event.repeat);
            break;
        default:
            break;
    }
}

inline void SDL::_handleTextInputEvent(const SDL_TextInputEvent& event) {
    auto window = _window(event.windowID);
    if (!window) { return; }

    switch (event.type) {
        case SDL_TEXTINPUT:
            window->firstResponder()->textInput(event.text);
            break;
        default:
            break;
    }
}

inline void SDL::_handleMultiGestureEvent(const SDL_MultiGestureEvent& e) {
    // TODO: handle gestures with mulitple fingers (this does not trigger for 1 finger)
}

inline SDL::SDLWindowPosition::SDLWindowPosition(const WindowPosition& pos) {
    switch(pos.mode) {
        case WindowPosition::Mode::kCentered:
            x = SDL_WINDOWPOS_CENTERED;
            y = SDL_WINDOWPOS_CENTERED;
            break;
        case WindowPosition::Mode::kAbsolute:
            x = pos.x;
            y = pos.y;
            break;
        case WindowPosition::Mode::kUndefined: // fall through
        default:
            x = SDL_WINDOWPOS_UNDEFINED;
            y = SDL_WINDOWPOS_UNDEFINED;
            break;
    }
}

inline MouseButton SDL::sMouseButton(uint8_t id) {
    switch (id) {
        case SDL_BUTTON_LEFT:   return MouseButton::kLeft;
        case SDL_BUTTON_MIDDLE: return MouseButton::kMiddle;
        case SDL_BUTTON_RIGHT:  return MouseButton::kRight;
        case SDL_BUTTON_X1:     return MouseButton::kX1;
        case SDL_BUTTON_X2:     return MouseButton::kX2;
        default:                return MouseButton::kLeft;
    }
}

inline void SDL::setCursorType(CursorType type) {
    SDL_SystemCursor id = SDL_SYSTEM_CURSOR_ARROW;

    switch (type) {
        case  CursorType::kArrow:       id = SDL_SYSTEM_CURSOR_ARROW;     break;
        case  CursorType::kText:        id = SDL_SYSTEM_CURSOR_IBEAM;     break;
        case  CursorType::kWait:        id = SDL_SYSTEM_CURSOR_WAIT;      break;
        case  CursorType::kCrosshair:   id = SDL_SYSTEM_CURSOR_CROSSHAIR; break;
        case  CursorType::kWaitArrow:   id = SDL_SYSTEM_CURSOR_WAITARROW; break;
        case  CursorType::kResizeNWSE:  id = SDL_SYSTEM_CURSOR_SIZENWSE;  break;
        case  CursorType::kResizeNESW:  id = SDL_SYSTEM_CURSOR_SIZENESW;  break;
        case  CursorType::kResizeWE:    id = SDL_SYSTEM_CURSOR_SIZEWE;    break;
        case  CursorType::kResizeNS:    id = SDL_SYSTEM_CURSOR_SIZENS;    break;
        case  CursorType::kResizeAll:   id = SDL_SYSTEM_CURSOR_SIZEALL;   break;
        case  CursorType::kNo:          id = SDL_SYSTEM_CURSOR_NO;        break;
        case  CursorType::kHand:        id = SDL_SYSTEM_CURSOR_HAND;      break;
        default:                        id = SDL_SYSTEM_CURSOR_ARROW;     break;
    }

    _cursor.reset(SDL_CreateSystemCursor(id));
    SDL_SetCursor(_cursor.get());
}

}}}
