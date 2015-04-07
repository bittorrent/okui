#pragma once

#include "bittorrent/ui/config.h"

#include "bittorrent/ui/MouseButton.h"
#include "bittorrent/ui/Point.h"
#include "bittorrent/ui/Rectangle.h"
#include "bittorrent/ui/ShaderCache.h"
#include "bittorrent/ui/shaders/ColorShader.h"
#include "bittorrent/ui/shaders/DistanceFieldShader.h"
#include "bittorrent/ui/shaders/TextureShader.h"

#include <list>

namespace bittorrent {
namespace ui {

class Application;
class Window;

class View {
public:
    View() {}
    View(const char* name) : _name(name) {}

    const char* name() const { return _name.c_str(); }
    void setName(const char* name) { _name = name; }

    void addSubview(View* view);
    void removeSubview(View* view);
    
    View* superview() const;
    Window* window() const;
    Application* application() const;
    
    Rectangle<int> bounds() const { return _bounds; }
    void setBounds(int x, int y, int width, int height);
    
    void setInterceptsMouseEvents(bool intercepts, bool childrenIntercept);

    ShaderCache* shaderCache();

    shaders::ColorShader* colorShader() { return shader<shaders::ColorShader>("color shader"); }
    shaders::TextureShader* textureShader() { return shader<shaders::TextureShader>("texture shader"); }
    shaders::DistanceFieldShader* distanceFieldShader() { return shader<shaders::DistanceFieldShader>("distance field shader"); }

    /**
    * Get or create a shader cached via the window's shader cache.
    */
    template <typename T>
    T* shader(const char* indentifier) {
        auto s = shaderCache()->get(std::string(indentifier));
        if (!s) {
            s = shaderCache()->add(std::unique_ptr<Shader>(new T()), std::string(indentifier), ShaderCache::Policy::kKeepForever);
        }
        auto ret = dynamic_cast<T*>(s->get());
        ret->setTransformation(renderTransformation());
        return ret;
    }
    
    AffineTransformation renderTransformation();
    
    /**
    * Converts a point from local coordinates to superview coordinates.
    */
    Point<int> localToSuperview(int x, int y);
    
    /**
    * Converts a point from super coordinates to local coordinates.
    */
    Point<int> superviewToLocal(int x, int y);
    
    /**
    * Override this to do drawing.
    */
    virtual void render() {}

    /**
    * Override this to lay out subviews whenever the view is resized.
    */
    virtual void layout() {}

    /**
    * Override this to do any sort of setup that requires the view to be attached to a window.
    */
    virtual void windowChanged() {}

    /**
    * Override this if you want odd-shaped views to have accurate hit boxes. 
    */
    virtual bool hitTest(int x, int y);

    /**
    * Override these to handle mouse events.
    */
    virtual void mouseDown(MouseButton button, int x, int y);
    virtual void mouseUp(MouseButton button, int x, int y);

    void renderAndRenderSubviews(Rectangle<int> viewport, double scale);
    bool dispatchMouseDown(MouseButton button, int x, int y);
    bool dispatchMouseUp(MouseButton button, int x, int y);

private:
    friend class Window;
    
    std::string _name;

    Rectangle<int> _bounds;
    bool _interceptsMouseEvents = true;
    bool _childrenInterceptMouseEvents = true;
    
    View* _superview = nullptr;
    std::list<View*> _subviews; // ordered back to front
    Window* _window = nullptr;

    void _dispatchWindowChange();
};

}}