#ifdef BT_USE_UI_VIEWS

#include "bittorrent/ui/Application.h"

#include "bittorrent/ui/Window.h"

#include <SDL2/SDL.h>

namespace bittorrent {
namespace ui {

Application::Application(ResourceManager* resourceManager) : _resourceManager(resourceManager) {
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        BT_LOG_ERROR("error initializing sdl: %s", SDL_GetError());
    }

    _backgroundThread = std::thread([this]{
        SetThreadName("Application RunLoop");
        _backgroundRunLoop.run();
    });
}

Application::~Application() {
    SDL_Quit();

    _backgroundRunLoop.cancel();
    _backgroundThread.join();

    _backgroundRunLoop.flush();
}

void Application::backgroundTask(std::function<void()> task) {
    _backgroundRunLoop.async(task);
}

void Application::run() {
    SDL_Event e;
    bool shouldQuit = false;
    while (!shouldQuit) {
#if __APPLE__
        @autoreleasepool {
#endif

        while (SDL_PollEvent(&e)) {
            switch (e.type) {
                case SDL_QUIT:
                    shouldQuit = true;
                    break;
                case SDL_MOUSEMOTION: {
                    auto& event = e.motion;
                    auto window = _windows[event.windowID];
                    switch (event.type) {
                        case SDL_MOUSEMOTION:
                            window->dispatchMouseMovement(event.x, window->height() - event.y);
                            break;
                        default:
                            break;
                    }
                    break;
                }
                case SDL_MOUSEBUTTONDOWN:
                case SDL_MOUSEBUTTONUP: {
                    auto& event = e.button;
                    auto window = _windows[event.windowID];
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
                    break;
                }
                case SDL_WINDOWEVENT: {
                    auto& event = e.window;
                    auto window = _windows[event.windowID];
                    switch (event.event) {
                        case SDL_WINDOWEVENT_RESIZED:
                            window->_didResize(event.data1, event.data2);
                            break;
                        default:
                            break;
                    }
                    break;
                }
                default:
                    break;
            }
        }

        for (auto& kv : _windows) {
            kv.second->_render();
        }

#if __APPLE__
        } // @autoreleasepool
#endif
    }
}

MouseButton Application::sMouseButton(uint8_t id) {
    switch (id) {
        case SDL_BUTTON_LEFT:
            return kMouseButtonLeft;
        case SDL_BUTTON_MIDDLE:
            return kMouseButtonMiddle;
        case SDL_BUTTON_RIGHT:
            return kMouseButtonRight;
        case SDL_BUTTON_X1:
            return kMouseButtonX1;
        case SDL_BUTTON_X2:
            return kMouseButtonX2;
        default:
            return kMouseButtonLeft;
    }
    return kMouseButtonLeft;
}

}}

#endif
