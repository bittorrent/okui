#pragma once
#include "bittorrent/ui/config.h"
#include "bittorrent/ui/Platform.h"

#include "bittorrent/ui/platforms/SDLKeycode.h"

#include <unordered_map>

#include <SDL2/SDL.h>

namespace bittorrent {
namespace ui {

class Window;

namespace platforms {

class SDL : public Platform {
public:
    SDL();
    ~SDL();

    virtual void run() override;
    virtual void async(std::function<void()> task) override;

    virtual void openWindow(Window* window, const char* title, int x, int y, int width, int height) override;
    virtual void closeWindow(Window* window) override;

    virtual void getWindowRenderSize(Window* window, int* width, int* height) override;

    virtual void setWindowPosition(Window* window, int x, int y) override;
    virtual void setWindowSize(Window* window, int width, int height) override;
    virtual void setWindowTitle(Window* window, const char* title) override;

    virtual void setClipboardText(const char* text) override;
    virtual std::string getClipboardText() override;

    virtual void startTextInput() override;
    virtual void stopTextInput() override;

private:
    struct WindowInfo {
        WindowInfo() {}
        WindowInfo(Window* window, SDL_Window* sdlWindow, SDL_GLContext& context)
            : window(window), sdlWindow(sdlWindow), context(context) {}

        Window* window = nullptr;
        SDL_Window* sdlWindow = nullptr;
        SDL_GLContext context;
    };

    SDL_Window* _sdlWindow(Window* window);

    void _handleMouseMotionEvent (const SDL_MouseMotionEvent& e);
    void _handleMouseButtonEvent (const SDL_MouseButtonEvent& e);
    void _handleMouseWheelEvent  (const SDL_MouseWheelEvent& e);
    void _handleWindowEvent      (const SDL_WindowEvent& e);
    void _handleKeyboardEvent    (const SDL_KeyboardEvent& e);
    void _handleTextInputEvent   (const SDL_TextInputEvent& e);

    std::unordered_map<Window*, uint32_t> _windowIds;
    std::unordered_map<uint32_t, WindowInfo> _windows;

    static MouseButton sMouseButton(uint8_t id);

    std::mutex _asyncMutex;
    std::vector<std::function<void()>> _asyncTasks;
};

inline SDL::SDL() {
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        BT_LOG_ERROR("error initializing sdl: %s", SDL_GetError());
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

        if (SDL_WaitEventTimeout(&e, std::max<int>(eventTimeoutMS, 1))) {
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

        std::vector<std::function<void()>> asyncTasks;
        {
            std::lock_guard<std::mutex> lock(_asyncMutex);
            asyncTasks.swap(_asyncTasks);
        }

        for (auto& task : asyncTasks) {
            task();
        }

        auto now = std::chrono::steady_clock::now();

        if (now - lastFrameTime >= minFrameInterval) {
            for (auto& kv : _windows) {
                SDL_GL_MakeCurrent(kv.second.sdlWindow, kv.second.context);
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

inline void SDL::async(std::function<void()> task) {
    std::lock_guard<std::mutex> lock(_asyncMutex);
    _asyncTasks.emplace_back(task);
}

inline void SDL::openWindow(Window* window, const char* title, int x, int y, int width, int height) {
    if (_windowIds.count(window)) {
        return;
    }

    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    auto sdlWindow = SDL_CreateWindow(title, x, y, width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
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

inline void SDL::setWindowPosition(Window* window, int x, int y) {
    if (auto w = _sdlWindow(window)) {
        SDL_SetWindowPosition(w, x, y);
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

inline SDL_Window* SDL::_sdlWindow(Window* window) {
    auto it = _windowIds.find(window);
    return it == _windowIds.end() ? nullptr : _windows[it->second].sdlWindow;
}

inline void SDL::_handleMouseMotionEvent(const SDL_MouseMotionEvent& event) {
    auto window = _windows[event.windowID].window;
    switch (event.type) {
        case SDL_MOUSEMOTION:
            window->dispatchMouseMovement(event.x, window->height() - event.y);
            break;
        default:
            break;
    }
}

inline void SDL::_handleMouseButtonEvent(const SDL_MouseButtonEvent& event) {
    auto window = _windows[event.windowID].window;
    switch (event.type) {
        case SDL_MOUSEBUTTONDOWN:
            window->dispatchMouseDown(sMouseButton(event.button), event.x, window->height() - event.y);
            break;
        case SDL_MOUSEBUTTONUP:
            window->dispatchMouseUp(sMouseButton(event.button), event.x, window->height() - event.y);
            break;
        default:
            break;
    }
}

inline void SDL::_handleMouseWheelEvent(const SDL_MouseWheelEvent& event) {
    auto window = _windows[event.windowID].window;
    switch (event.type) {
        case SDL_MOUSEWHEEL: {
            int xPos = 0, yPos = 0;
            SDL_GetMouseState(&xPos, &yPos);
            window->dispatchMouseWheel(xPos, window->height() - yPos, event.x, event.y);
            break;
        }
        default:
            break;
    }
}

inline void SDL::_handleWindowEvent(const SDL_WindowEvent& event) {
    auto window = _windows[event.windowID].window;
    switch (event.event) {
        case SDL_WINDOWEVENT_RESIZED:
            _didResize(window, event.data1, event.data2);
            break;
        default:
            break;
    }
}

inline void SDL::_handleKeyboardEvent(const SDL_KeyboardEvent& event) {
    auto window = _windows[event.windowID].window;
    switch (event.type) {
        case SDL_KEYDOWN:
            window->dispatchKeyDown(ConvertKeycode(event.keysym.sym), ConvertKeyModifier(event.keysym.mod), event.repeat);
            break;
        case SDL_KEYUP:
            window->dispatchKeyUp(ConvertKeycode(event.keysym.sym), ConvertKeyModifier(event.keysym.mod), event.repeat);
            break;
        default:
            break;
    }
}

inline void SDL::_handleTextInputEvent(const SDL_TextInputEvent& event) {
    auto window = _windows[event.windowID].window;
    switch (event.type) {
        case SDL_TEXTINPUT:
            window->dispatchTextInput(event.text);
            break;
        default:
            break;
    }
}


inline MouseButton SDL::sMouseButton(uint8_t id) {
    switch (id) {
        case SDL_BUTTON_LEFT:
            return MouseButton::kLeft;
        case SDL_BUTTON_MIDDLE:
            return MouseButton::kMiddle;
        case SDL_BUTTON_RIGHT:
            return MouseButton::kRight;
        case SDL_BUTTON_X1:
            return MouseButton::kX1;
        case SDL_BUTTON_X2:
            return MouseButton::kX2;
        default:
            return MouseButton::kLeft;
    }
    return MouseButton::kLeft;
}

}}}
