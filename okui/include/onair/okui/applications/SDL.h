#pragma once

#include "onair/okui/config.h"

#include "onair/okui/Controller.h"
#include "onair/okui/FileResourceManager.h"
#include "onair/okui/Application.h"
#include "onair/okui/Rectangle.h"
#include "onair/okui/Point.h"
#include "onair/okui/applications/SDLKeycode.h"

#include "scraps/Timer.h"

#include <unordered_map>

#include <SDL2/SDL.h>

#if SCRAPS_MAC_OS_X
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wextern-c-compat"
#include <SDL2/SDL_syswm.h>
#pragma clang diagnostic pop

@interface OKUISDLApplication : NSApplication
@property onair::okui::Application* application;
@end
#endif // SCRAPS_MAC_OS_X

namespace onair {
namespace okui {
namespace applications {

class SDL : public Application {
public:
    SDL();
    ~SDL();

    virtual void run() override;
    virtual void quit() override;

    virtual void openWindow(Window* window, const char* title, const WindowPosition& windowPosition, int width, int height) override;
    virtual void closeWindow(Window* window) override;

    virtual void getWindowRenderSize(Window* window, int* width, int* height) override;
    virtual void getWindowSize(Window* window, int* width, int* height) override;
    virtual void getWindowPosition(Window* window, int* x, int* y) override;

    virtual void setWindowPosition(Window* window, const WindowPosition& pos) override;
    virtual void setWindowSize(Window* window, int width, int height) override;
    virtual void setWindowTitle(Window* window, const char* title) override;

    virtual Window* activeWindow() override { return _activeWindow; }

    virtual std::string userStoragePath() const override;

    virtual void setClipboardText(const char* text) override;
    virtual std::string getClipboardText() override;

    virtual void startTextInput() override;
    virtual void stopTextInput() override;

    virtual std::string operatingSystem() const override;
    virtual void setScreenSaverEnabled(bool enabled = true) override { enabled ? SDL_EnableScreenSaver() : SDL_DisableScreenSaver(); }

    virtual void setCursorType(CursorType type) override;
    virtual void showCursor(bool visible = true) override;
    virtual bool isCursorVisible() const override;

#if SCRAPS_MAC_OS_X
    virtual NSWindow* nativeWindow(Window* window) const override;
#endif

#if SCRAPS_ANDROID
    /**
    * @return a new local reference to the activity
    */
    virtual jobject nativeActivity(JNIEnv** envOut = nullptr) const override;
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

    struct Controller : onair::okui::Controller {
        explicit Controller(SDL_Joystick* joystick) : joystick{joystick} {}
        ~Controller();

        virtual std::string name() const override;
        virtual std::string guid() const override;

        virtual size_t buttons() const override { return static_cast<size_t>(SDL_JoystickNumButtons(joystick)); }

        virtual size_t axes() const override { return static_cast<size_t>(SDL_JoystickNumAxes(joystick)); }
        virtual Controller::AxisType axisType(size_t axis) const override;

        SDL_Joystick* joystick;
    };

    Window* _window(uint32_t id) const;
    SDL_Window* _sdlWindow(Window* window) const;

    void _handleMouseMotionEvent (const SDL_MouseMotionEvent& e);
    void _handleMouseButtonEvent (const SDL_MouseButtonEvent& e);
    void _handleMouseWheelEvent  (const SDL_MouseWheelEvent& e);
    void _handleFingerEvent      (const SDL_TouchFingerEvent& e);
    void _handleWindowEvent      (const SDL_WindowEvent& e);
    void _handleKeyboardEvent    (const SDL_KeyboardEvent& e);
    void _handleTextInputEvent   (const SDL_TextInputEvent& e);

    void _addJoystick(int index);
    void _removeJoystick(SDL_JoystickID id);
    void _handleJoystickAxisEvent(const SDL_JoyAxisEvent& event);
    void _handleJoystickButtonEvent(const SDL_JoyButtonEvent& event);

    std::unordered_map<Window*, uint32_t> _windowIds;
    std::unordered_map<uint32_t, WindowInfo> _windows;
    Window* _activeWindow = nullptr;
    std::unique_ptr<SDL_Cursor, CursorDeleter> _cursor;
    bool _backgrounded = false;
    std::unordered_map<SDL_JoystickID, std::unique_ptr<Controller>> _controllers;

    std::unique_ptr<ResourceManager> _resourceManager;

    static MouseButton sMouseButton(uint8_t id);
};

inline SDL::SDL() {
#if SCRAPS_MAC_OS_X
    // make sure we use our application class instead of sdl's
    ((OKUISDLApplication*)[OKUISDLApplication sharedApplication]).application = this;
    [NSApp finishLaunching];
#endif

    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        SCRAPS_LOGF_ERROR("error initializing sdl: %s", SDL_GetError());
    }

#if SCRAPS_TVOS
    SDL_SetHint(SDL_HINT_APPLE_TV_CONTROLLER_UI_EVENTS, "1");
#endif

#if OPENGL_ES
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
#else
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
#endif

    if (auto path = SDL_GetBasePath()) {
        _resourceManager = std::make_unique<FileResourceManager>(path);
        SDL_free(path);
        setResourceManager(_resourceManager.get());
    }
}

inline SDL::~SDL() {
    _controllers.clear();
    SDL_Quit();
}

inline void SDL::run() {
    SDL_SetThreadPriority(SDL_THREAD_PRIORITY_HIGH);

    SDL_Event e;
    bool shouldQuit = false;

    static constexpr auto minFrameInterval = 1000ms/60;
    scraps::SteadyTimer timer;
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
                    case SDL_FINGERUP:
                    case SDL_FINGERDOWN:
                    case SDL_FINGERMOTION:            { _handleFingerEvent(e.tfinger); break; }
                    case SDL_WINDOWEVENT:             { _handleWindowEvent(e.window); break; }
                    case SDL_KEYUP:
                    case SDL_KEYDOWN:                 { _handleKeyboardEvent(e.key); break; }
                    case SDL_TEXTINPUT:               { _handleTextInputEvent(e.text); break; }
                    case SDL_APP_TERMINATING:         { terminating(); break; }
                    case SDL_APP_LOWMEMORY:           { lowMemory(); break; }
                    case SDL_APP_WILLENTERBACKGROUND: { enteringBackground(); break; }
                    case SDL_APP_DIDENTERBACKGROUND:  { enteredBackground(); _backgrounded = true; break; }
                    case SDL_APP_WILLENTERFOREGROUND: { enteringForeground(); break; }
                    case SDL_APP_DIDENTERFOREGROUND:  { enteredForeground(); _backgrounded = false; break; }
                    case SDL_JOYDEVICEADDED:          { _addJoystick(e.jdevice.which); break; }
                    case SDL_JOYDEVICEREMOVED:        { _removeJoystick(e.jdevice.which); break; }
                    case SDL_JOYAXISMOTION:           { _handleJoystickAxisEvent(e.jaxis); break; }
                    case SDL_JOYBUTTONUP:
                    case SDL_JOYBUTTONDOWN:           { _handleJoystickButtonEvent(e.jbutton); break; }
                    default:                          { break; }
                }
            } else if ((minFrameInterval - timer.elapsed()) > 2ms) {
                std::this_thread::sleep_for(1ms);
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

    if (!_activeWindow) {
        _activeWindow = window;
    }

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

    if (_activeWindow == window) {
        _activeWindow = nullptr;
    }
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

inline void SDL::getWindowPosition(Window* window, int* x, int* y) {
    if (auto w = _sdlWindow(window)) {
        SDL_GetWindowPosition(w, x, y);
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

inline std::string SDL::userStoragePath() const {
    auto path = SDL_GetPrefPath(organization().c_str(), name().c_str());
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

inline std::string SDL::operatingSystem() const {
    return SDL_GetPlatform();
}

#if SCRAPS_MAC_OS_X
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
        SCRAPS_ASSERT(info.subsystem == SDL_SYSWM_COCOA);
        if (info.subsystem != SDL_SYSWM_COCOA) {
            return nil;
        }
        return info.info.cocoa.window;
    }
    return nil;
}
#endif

#if SCRAPS_ANDROID
inline jobject SDL::nativeActivity(JNIEnv** envOut) const {
    auto env = reinterpret_cast<JNIEnv*>(SDL_AndroidGetJNIEnv());
    auto activity = reinterpret_cast<jobject>(SDL_AndroidGetActivity());
    if (activity) {
        if (envOut) {
            *envOut = env;
        }
        return activity;
    }
    SCRAPS_LOGF_ERROR("unable to get android activity");
    return nullptr;
}
#endif

inline SDL::Controller::~Controller() {
    if (SDL_JoystickGetAttached(joystick)) {
        SDL_JoystickClose(joystick);
    }
}

inline std::string SDL::Controller::name() const {
    auto name = SDL_JoystickName(joystick);
    return name ? name : "";
}

inline std::string SDL::Controller::guid() const {
    char ret[40] = {};
    SDL_JoystickGetGUIDString(SDL_JoystickGetGUID(joystick), ret, sizeof(ret));
    return ret;
}

inline Controller::AxisType SDL::Controller::axisType(size_t axis) const {
    if (axis == 0) {
        return AxisType::kJoystickX;
    } else if (axis == 1) {
        return AxisType::kJoystickY;
    }
    return AxisType::kUnknown;
}

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
    if (scraps::platform::kIsTVOS) { return; }

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
    if (scraps::platform::kIsTVOS) { return; }

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
    if (scraps::platform::kIsTVOS) { return; }

    auto window = _window(event.windowID);
    if (!window) { return; }

    switch (event.type) {
        case SDL_MOUSEWHEEL: {
            int xPos = 0, yPos = 0;
            SDL_GetMouseState(&xPos, &yPos);
            auto xMotion = -event.x;
            auto yMotion = event.y;
            if (event.direction == SDL_MOUSEWHEEL_FLIPPED) {
                xMotion *= -1;
                yMotion *= -1;
            }
            window->dispatchMouseWheel(xPos, yPos, xMotion, yMotion);
            break;
        }
        default:
            break;
    }
}

inline void SDL::_handleFingerEvent(const SDL_TouchFingerEvent& event) {
    switch (event.type) {
        case SDL_FINGERUP:
            return firstResponder()->touchUp(static_cast<size_t>(event.fingerId), {event.x, event.y}, event.pressure);
        case SDL_FINGERDOWN:
            return firstResponder()->touchDown(static_cast<size_t>(event.fingerId), {event.x, event.y}, event.pressure);
        case SDL_FINGERMOTION:
            return firstResponder()->touchMovement(static_cast<size_t>(event.fingerId), {event.x, event.y}, {event.dx, event.dy}, event.pressure);
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

inline void SDL::_addJoystick(int index) {
    if (auto joystick = SDL_JoystickOpen(index)) {
        _controllers[SDL_JoystickInstanceID(joystick)] = std::make_unique<Controller>(joystick);
    }
}

inline void SDL::_removeJoystick(SDL_JoystickID id) {
    _controllers.erase(id);
}

inline void SDL::_handleJoystickAxisEvent(const SDL_JoyAxisEvent& event) {
    auto it = _controllers.find(event.which);
    if (it == _controllers.end()) {
        return;
    }
    firstResponder()->analogInput(*it->second, event.axis, (event.value + 32768.0) / 65535.0 * 2.0 - 1.0);
}

inline void SDL::_handleJoystickButtonEvent(const SDL_JoyButtonEvent& event) {
    auto it = _controllers.find(event.which);
    if (it == _controllers.end()) {
        return;
    }
    if (event.type == SDL_JOYBUTTONDOWN) {
        firstResponder()->buttonDown(*it->second, event.button);
    } else {
        firstResponder()->buttonUp(*it->second, event.button);
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

inline void SDL::showCursor(bool visible) {
    auto ret = SDL_ShowCursor(visible);

    if (ret < 0) {
        SCRAPS_LOG_ERROR("showCursor error {}, {}", ret, SDL_GetError());
    }
}

inline bool SDL::isCursorVisible() const {
    auto ret = SDL_ShowCursor(-1);

    if (ret < 0) {
        SCRAPS_LOG_ERROR("isCursorVisible error {}, {}", ret, SDL_GetError());
    }

    return ret;
}

}}}
