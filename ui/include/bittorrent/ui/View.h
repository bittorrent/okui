#pragma once

#include "bittorrent/ui/config.h"
#include "bittorrent/ui/Point.h"
#include "bittorrent/ui/Rectangle.h"
#include "bittorrent/ui/RenderTarget.h"
#include "bittorrent/ui/Responder.h"
#include "bittorrent/ui/ShaderCache.h"
#include "bittorrent/ui/shaders/BoxShadowShader.h"
#include "bittorrent/ui/shaders/ColorShader.h"
#include "bittorrent/ui/shaders/DistanceFieldShader.h"
#include "bittorrent/ui/shaders/TextureShader.h"

#include <boost/optional.hpp>

#include <list>

namespace bittorrent {
namespace ui {

class Application;
class Window;

class View : public Responder {
public:
    View() {}
    View(const char* name) : _name(name) {}
    virtual ~View();

    const char* name() const { return _name.c_str(); }
    void setName(const char* name) { _name = name; }

    void addSubview(View* view);
    void removeSubview(View* view);

    View* superview() const;
    Window* window() const { return _window; }
    Application* application() const;

    Rectangle<int> bounds() const { return _bounds; }
    void setBounds(int x, int y, int width, int height);

    void setInterceptsMouseEvents(bool intercepts, bool childrenIntercept);

    ShaderCache* shaderCache();

    void setIsVisible(bool isVisible = true);
    bool isVisible() const { return _isVisible; }
        
    /**
    * Allows scaling of the view by a particular factor.
    */
    void setScale(double scale) { setScale(scale, scale); }
    void setScale(double scaleX, double scaleY) { _scale.x = scaleX; _scale.y = scaleY; }

    /**
    * Returns true if the view's ancestors are visible or if the view has no ancestors.
    */
    bool ancestorsAreVisible() const;

    void sendToBack();
    void bringToFront();

    /**
    * Override this if the view can become focused.
    */
    virtual bool canBecomeFocus() { return false; }

    void focus();
    void unfocus();

    /**
    * If the view receives an unhandled tab key down, focus will change to the given
    * view (and shift+tab will do the opposite).
    */
    void setNextFocus(View* view);

    /**
    * Returns the next available visible and focusable view if there is one.
    */
    View* nextAvailableFocus();

    /**
    * Returns the previous available visible and focusable view.
    */
    View* previousAvailableFocus();

    bool isFocus() const;

    bool isDescendantOf(const View* view) const;

    void setClipped(bool isClipped = true) { _clipped = isClipped; }
    bool clipped() const { return _clipped; }

    /**
    * Returns true if the mouse is hovering directly over this view.
    */
    bool hasMouse() const;

    shaders::ColorShader* colorShader() { return shader<shaders::ColorShader>("color shader"); }
    shaders::TextureShader* textureShader() { return shader<shaders::TextureShader>("texture shader"); }
    shaders::BoxShadowShader* boxShadowShader() { return shader<shaders::BoxShadowShader>("box shadow shader"); }
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
    * Override these to handle mouse events. Call the base implementation to pass on the event.
    */
    virtual void mouseDown(MouseButton button, int x, int y);
    virtual void mouseUp(MouseButton button, int x, int y);
    virtual void mouseWheel(int xPos, int yPos, int xWheel, int yWheel);
    virtual void mouseDrag(int x, int y) {}
    virtual void mouseMovement(int x, int y) {}
    virtual void mouseEnter() {}
    virtual void mouseExit() {}

    virtual void focusGained() {}
    virtual void focusLost() {}

    /**
    * Called before the view and all of its ancestors become visible in the window.
    */
    virtual void willAppear() {}
        
    /**
    * Called when the view and all of its ancestors become visible in the window.
    */
    virtual void appeared() {}

    /**
    * Called before the view or one of its ancestors become invisible in the window.
    */
    virtual void willDisappear() {}

    /**
    * Called when the view or one of its ancestors become invisible in the window.
    */
    virtual void disappeared() {}

    /**
    * Renders the view and its subviews.
    *
    * @param area the area within the target to render to. the view will fill this area
    * @param clipBounds the bounds within the target to clip rendering of the view and its children to
    */
    void renderAndRenderSubviews(const RenderTarget* target, const Rectangle<int>& area, boost::optional<Rectangle<int>> clipBounds = boost::none);

    bool dispatchMouseDown(MouseButton button, int x, int y);
    bool dispatchMouseUp(MouseButton button, int x, int y);
    void dispatchMouseMovement(int x, int y);
    bool dispatchMouseWheel(int xPos, int yPos, int xWheel, int yWheel);

    // Responder overrides
    virtual Responder* nextResponder() override;
    virtual void keyDown(KeyCode key, KeyModifiers mod, bool repeat) override;

private:
	View(const View& other) = delete;
	View(const View&& other) = delete;

    friend class Window;

    std::string      _name;
    bool             _isVisible = true;

    bool             _clipped = true;

    Rectangle<int>   _bounds;
    bool             _interceptsMouseEvents = true;
    bool             _childrenInterceptMouseEvents = true;

    View*            _superview = nullptr;
    std::list<View*> _subviews; // ordered back to front
    Window*          _window = nullptr;

    View*            _subviewWithMouse = nullptr;

    View*            _nextFocus = nullptr;
    View*            _previousFocus = nullptr;
    
    Point<double>    _scale{1.0, 1.0};

    void _dispatchFutureVisibilityChange(bool visible);
    void _dispatchVisibilityChange(bool visible);
    void _dispatchWindowChange(Window* window);
    void _mouseExit();
};

}}
