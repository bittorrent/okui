#pragma once

#include "bittorrent/ui/config.h"

#include "bittorrent/ui/Window.h"

#include <cstdint>

namespace bittorrent {
namespace ui {

class Platform {
public:
    virtual ~Platform() {}

    virtual void run() = 0;
    
    virtual void openWindow(Window* window, const char* title, int x, int y, int width, int height) = 0;
    virtual void closeWindow(Window* window) = 0;
    
    virtual void getWindowRenderSize(Window* window, int* width, int* height) = 0;

    virtual void setWindowPosition(Window* window, int x, int y) = 0;
    virtual void setWindowSize(Window* window, int width, int height) = 0;
    virtual void setWindowTitle(Window* window, const char* title) = 0;

protected:
    void _render(Window* window) { window->_render(); }
    void _didResize(Window* window, int width, int height) { window->_didResize(width, height); }
};

}}