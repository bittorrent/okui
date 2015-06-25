#pragma once

#include "onair/okui/config.h"
#include "onair/okui/Platform.h"

#include "onair/okui/platforms/Native.h"
#include "onair/okui/platforms/SDLKeycode.h"

#include <unordered_map>

#include <SDL2/SDL.h>

#if ONAIR_MAC_OS_X
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wextern-c-compat"
#include <SDL2/SDL_syswm.h>
#pragma clang diagnostic pop

@interface OKUISDLApplication : NSApplication
@property onair::okui::Platform* platform;
@end
#endif // ONAIR_MAC_OS_X

namespace onair {
namespace okui {
namespace platforms {

class SDL : public Native {
public:
    SDL();
    ~SDL();

    virtual void run() override;
    virtual void quit() override;

    virtual void openWindow(Window* window, const char* title, const WindowPosition& windowPosition, int width, int height) override;
    virtual void closeWindow(Window* window) override;

    virtual void getWindowRenderSize(Window* window, int* width, int* height) override;

    virtual void setWindowPosition(Window* window, const WindowPosition& pos) override;
    virtual void setWindowSize(Window* window, int width, int height) override;
    virtual void setWindowTitle(Window* window, const char* title) override;

    virtual Window* activeWindow() override { return _activeWindow; }

    virtual std::string userStoragePath(const char* application, const char* organization) const override;

    virtual void setClipboardText(const char* text) override;
    virtual std::string getClipboardText() override;

    virtual void startTextInput() override;
    virtual void stopTextInput() override;

    virtual std::string operatingSystem() const override { return SDL_GetPlatform(); }
    virtual void setScreenSaverEnabled(bool enabled = true) override { enabled ? SDL_EnableScreenSaver() : SDL_DisableScreenSaver(); }

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

    Window* _window(uint32_t id) const;
    SDL_Window* _sdlWindow(Window* window) const;

    void _handleMouseMotionEvent (const SDL_MouseMotionEvent& e);
    void _handleMouseButtonEvent (const SDL_MouseButtonEvent& e);
    void _handleMouseWheelEvent  (const SDL_MouseWheelEvent& e);
    void _handleWindowEvent      (const SDL_WindowEvent& e);
    void _handleKeyboardEvent    (const SDL_KeyboardEvent& e);
    void _handleTextInputEvent   (const SDL_TextInputEvent& e);

    std::unordered_map<Window*, uint32_t> _windowIds;
    std::unordered_map<uint32_t, WindowInfo> _windows;
    Window* _activeWindow = nullptr;

    static MouseButton sMouseButton(uint8_t id);
};

inline SDL::SDL() {
#if ONAIR_MAC_OS_X
    // make sure we use our application class instead of sdl's
    ((OKUISDLApplication*)[OKUISDLApplication sharedApplication]).platform = this;
    [NSApp finishLaunching];
#endif
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        ONAIR_LOG_ERROR("error initializing sdl: %s", SDL_GetError());
    }
}

inline SDL::~SDL() {
    SDL_Quit();
}

inline void SDL::run() {
    SDL_Event e;
    bool shouldQuit = false;

    constexpr auto frameRateLimit = 60;
    constexpr auto minFrameInterval = std::chrono::microseconds(1000000 / frameRateLimit);
    std::chrono::steady_clock::time_point lastFrameTime;

    while (!shouldQuit) {
#if __APPLE__
        @autoreleasepool {
#endif

        auto eventTimeoutMS = std::chrono::duration_cast<std::chrono::milliseconds>(minFrameInterval - (std::chrono::steady_clock::now() - lastFrameTime)).count();

        if (SDL_WaitEventTimeout(&e, std::max(static_cast<int>(eventTimeoutMS), 1))) {
            switch (e.type) {
                case SDL_QUIT:            { shouldQuit = true; break; }
                case SDL_MOUSEMOTION:     { _handleMouseMotionEvent(e.motion); break; }
                case SDL_MOUSEBUTTONUP:
                case SDL_MOUSEBUTTONDOWN: { _handleMouseButtonEvent(e.button); break; }
                case SDL_MOUSEWHEEL:      { _handleMouseWheelEvent(e.wheel); break; }
                case SDL_WINDOWEVENT:     { _handleWindowEvent(e.window); break; }
                case SDL_KEYUP:
                case SDL_KEYDOWN:         { _handleKeyboardEvent(e.key); break;}
                case SDL_TEXTINPUT:       { _handleTextInputEvent(e.text); break; }
                default:                  break;
            }
        }

        taskQueue()->run();

        if (shouldQuit) { break; }

        auto now = std::chrono::steady_clock::now();

        if (now - lastFrameTime >= minFrameInterval) {
            for (auto& kv : _windows) {
                SDL_GL_MakeCurrent(kv.second.sdlWindow, kv.second.context);
                glDisable(GL_SCISSOR_TEST);
                glClearColor(0.0, 0.0, 0.0, 0.0);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
                _render(kv.second.window);
                SDL_GL_SwapWindow(kv.second.sdlWindow);
            }
            lastFrameTime = now;
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

inline void SDL::setWindowPosition(Window* window, const WindowPosition& position) {
    if (auto w = _sdlWindow(window)) {
        auto pos = SDLWindowPosition{position};
        SDL_SetWindowPosition(w, pos.x, pos.y);
    }
}

inline void SDL::setWindowSize(Window* window, int width, int height) {
    if (auto w = _sdlWindow(window)) {
        SDL_SetWindowSize(w, width, height);
    }
}

inline void SDL::setWindowTitle(Window* window, const char* title) {
    if (auto w = _sdlWindow(window)) {
        SDL_SetWindowTitle(w, title);
    }
}

inline std::string SDL::userStoragePath(const char* application, const char* organization) const {
    auto path = SDL_GetPrefPath(organization, application);
    std::string ret(path);
    SDL_free(path);
    return ret;
}

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

}}}
