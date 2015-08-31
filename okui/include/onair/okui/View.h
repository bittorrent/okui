#pragma once

#include "onair/okui/config.h"

#include "onair/AbstractTaskScheduler.h"

#include "onair/okui/Color.h"
#include "onair/okui/Point.h"
#include "onair/okui/Rectangle.h"
#include "onair/okui/RenderTarget.h"
#include "onair/okui/Responder.h"
#include "onair/okui/ShaderCache.h"
#include "onair/okui/TextureHandle.h"
#include "onair/okui/WeakTexture.h"
#include "onair/okui/opengl/Framebuffer.h"
#include "onair/okui/shaders/BoxShadowShader.h"
#include "onair/okui/shaders/ColorShader.h"
#include "onair/okui/shaders/DistanceFieldShader.h"
#include "onair/okui/shaders/TextureShader.h"

#include <boost/optional.hpp>

#include <list>
#include <typeindex>
#include <unordered_map>

namespace onair {
namespace okui {

class Application;
class BitmapFont;
class Window;

/**
* View rendering can be cached or buffered to a texture. One reason this may be done is to apply post-rendering
* effects such as tinting or reflection. When this is done, the contents of the view are clipped to its bounds.
*/
class View : public Responder {
public:
    View() {}
    View(std::string name) : _name(std::move(name)) {}
    virtual ~View();

    const std::string& name() const { return _name; }
    void setName(std::string name) { _name = std::move(name); }

    void addSubview(View* view);
    void removeSubview(View* view);

    void addSubviews() {}
    template <typename... Views>
    void addSubviews(View* view, Views&&... views) {
        addSubview(view);
        addSubviews(std::forward<Views>(views)...);
    }

    void removeSubviews() {}
    template <typename... Views>
    void removeSubviews(View* view, Views&&... views) {
        removeSubview(view);
        removeSubviews(std::forward<Views>(views)...);
    }

    View* superview() const;
    Window* window() const { return _window; }
    Application* application() const;

    const Rectangle<int>& bounds() const { return _bounds; }

    template <typename... Args>
    void setBounds(Args&&... args) { _setBounds(Rectangle<int>(std::forward<Args>(args)...)); }

    void setInterceptsMouseEvents(bool intercepts, bool childrenIntercept);

    ShaderCache* shaderCache();

    void setIsVisible(bool isVisible = true);
    bool isVisible() const { return _isVisible; }

    void show() { setIsVisible(); }
    void hide() { setIsVisible(false); }

    /**
    * Allows scaling of the view by a particular factor.
    */
    void setScale(double scale) { setScale(scale, scale); }
    void setScale(double scaleX, double scaleY) { _scale.x = scaleX; _scale.y = scaleY; }

    /**
    * Sets the view's background color.
    */
    template <typename... Args>
    void setBackgroundColor(Args&&... args) { _backgroundColor = Color(std::forward<Args>(args)...); }

    const Color& backgroundColor() const { return _backgroundColor; }

    /**
    * Sets the view's tint. If this is anything but opaque white, this is a post-rendering effect that
    * clips the view's contents.
    */
    template <typename... Args>
    void setTintColor(Args&&... args) { _tintColor = Color(std::forward<Args>(args)...); }

    const Color& tintColor() const { return _tintColor; }

    /**
    * Sets the view's opacity. If this is anything but 1, this is a post-rendering effect that
    * clips the view's contents.
    *
    * This effectively sets the alpha component of the view's tint color.
    */
    void setOpacity(double opacity) { _tintColor.a = opacity; }

    /**
    * Returns true if the view's ancestors are visible or if the view has no ancestors.
    */
    bool ancestorsAreVisible() const;

    /**
    * Returns true if the view and its ancestors are visible and in an open window.
    */
    bool isVisibleInOpenWindow() const;

    /**
    * Arranges the view so it's behind all of its current siblings.
    */
    void sendToBack();

    /**
    * Arranges the view so it's in front of all of its current siblings.
    */
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

    /**
    * @param rendersToTexture if true, the view is rendered to a texture, making it available via renderTexture
    */
    void setRendersToTexture(bool rendersToTexture = true) { _rendersToTexture = rendersToTexture; }

    /**
    * If the view is set to render to a texture, this can be used to obtain said texture.
    */
    std::shared_ptr<Texture> renderTexture() const;

    /**
    * @param caches if true, the view will render to a texture and the render() method will only be called when
    *               the cache is invalid
    */
    void setCachesRender(bool cachesRender = true) { _cachesRender = cachesRender; }

    /**
    * Invalidates the view's render cache.
    */
    void invalidateRenderCache();

    /**
    * @param clipsToBounds if true, the view will not render itself or any subviews outside of its bounds
    */
    void setClipsToBounds(bool clipsToBounds = true) { _clipsToBounds = clipsToBounds; }

    /**
    * @return if true, the view will not render itself or any subviews outside of its bounds
    */
    bool clipsToBounds() const { return _clipsToBounds; }

    /**
    * Returns true if the mouse is hovering directly over this view.
    */
    bool hasMouse() const;

    shaders::ColorShader* colorShader() { return shader<shaders::ColorShader>("color shader"); }
    shaders::TextureShader* textureShader() { return shader<shaders::TextureShader>("texture shader"); }
    shaders::BoxShadowShader* boxShadowShader() { return shader<shaders::BoxShadowShader>("box shadow shader"); }
    shaders::DistanceFieldShader* distanceFieldShader() { return shader<shaders::DistanceFieldShader>("distance field shader"); }

    /**
    * Begins loading a texture associated with the view. When the texture is loaded, the view's
    * render cache will be invalidated. If the texture should not be associated with the view,
    * use Window::loadTexture* instead.
    */
    TextureHandle loadTextureResource(const std::string& name);
    TextureHandle loadTextureFromMemory(std::shared_ptr<const std::string> data);
    TextureHandle loadTextureFromURL(const std::string& url);

    /**
    * Get or create a shader cached via the window's shader cache.
    */
    template <typename T>
    T* shader(const char* identifier) {
        auto s = shaderCache()->get(std::string(identifier));
        if (!s) {
            s = shaderCache()->add(std::unique_ptr<Shader>(new T()), std::string(identifier), ShaderCache::Policy::kKeepForever);
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
    Point<int> localToSuperview(const Point<int>& p);

    /**
    * Converts a point from super coordinates to local coordinates.
    */
    Point<int> superviewToLocal(int x, int y);
    Point<int> superviewToLocal(const Point<int>& p);

    enum class Relation {
        kApplication, 
        kWindow, 
        kAncestor, 
        kDescendant, 
        kSibling,
    };
    
    /**
    * Returns true if this view has the given relation to the given view.
    *
    * For example, hasRelation(kDescendant, superview()) should return true.
    */
    bool hasRelation(Relation relation, const View* view) const;

    /**
    * Posts a message to listeners with the given relation. The view and its potential listeners must have an 
    * associated application for the message to be delivered.
    *
    * For example, post(message, Relation::kAncestor) posts the message to listeners that are ancestors of this view.
    */
    template <typename T>
    void post(T& message, Relation relation = Relation::kWindow) {
        _post(std::type_index(typeid(typename std::decay<T>::type)), &message, relation);
    }

    template <typename T>
    struct ListenerAction : ListenerAction<decltype(&T::operator())> {};

    template <typename C, typename R, typename... Args>
    struct ListenerAction<R(C::*)(Args...) const> {
        using ArgumentTuple = std::tuple<Args...>;
        using ArgumentCount = std::tuple_size<ArgumentTuple>;
        using MessageType = typename std::decay<typename std::tuple_element<0, ArgumentTuple>::type>::type;
    };

    /**
    * Listens for messages from senders with the given relation.
    *
    * For example, listen([](const Message& message) {}, Relation::kAncestor) listens for messages from posters that 
    * are ancestors of this view.
    *
    * @param action an action that takes a constant reference to the message type to listen for and optionally a View pointer to the sender
    */
    template <typename Action>
    auto listen(Action&& action, Relation relation = Relation::kWindow) -> typename std::enable_if<ListenerAction<Action>::ArgumentCount::value == 1, void>::type {
        using MessageType = typename ListenerAction<Action>::MessageType;
        _listen(std::type_index(typeid(MessageType)), [action = std::forward<Action>(action)](const void* message, View* sender) { action(*reinterpret_cast<const MessageType*>(message)); }, relation);
    }

    template <typename Action>
    auto listen(Action&& action, Relation relation = Relation::kWindow) -> typename std::enable_if<ListenerAction<Action>::ArgumentCount::value == 2, void>::type {
        using MessageType = typename ListenerAction<Action>::MessageType;
        _listen(std::type_index(typeid(MessageType)), [action = std::forward<Action>(action)](const void* message, View* sender) { action(*reinterpret_cast<const MessageType*>(message), sender); }, relation);
    }

    /**
    * Makes the given object available to all descendants via get().
    *
    * @param key to provide multiple objects of the same type, you can specify a hash to use as a key
    */
    template <typename T>
    void provide(T* provision, size_t key = 0) {
        _provisions[std::type_index(typeid(T)).hash_code() ^ key] = provision;
    }

    /**
    * Gets an object provided by an ancestor.
    */
    template <typename T>
    T* get(size_t key = 0) const {
        return reinterpret_cast<T*>(_get(std::type_index(typeid(T)).hash_code() ^ key));
    }

    /**
    * Asynchronously schedules a function to be invoked using the application's task scheduler.
    *
    * If the view is destroyed, the invocation will be canceled.
    */
    template <typename... Args>
    auto async(Args&&... args) -> decltype(std::declval<AbstractTaskScheduler>().async(std::forward<Args>(args)...)) {
        return _taskScheduler()->async(_taskScope, std::forward<Args>(args)...);
    }

    /**
    * Asynchronously schedules a function to be invoked after a delay using the application's task scheduler.
    *
    * If the view is destroyed, the invocation will be canceled.
    */
    template <typename... Args>
    auto asyncAfter(Args&&... args) -> decltype(std::declval<AbstractTaskScheduler>().asyncAfter(std::forward<Args>(args)...)) {
        return _taskScheduler()->asyncAfter(_taskScope, std::forward<Args>(args)...);
    }

    /**
    * Override this to perform updates for each frame. This is invoked exactly once per frame, before any views
    * begin rendering.
    */
    virtual void update() {}

    /**
    * Override this to do drawing.
    */
    virtual void render() {}

    /**
    * Override this to implement custom post-render effects. For this method to be called, the view must be
    * set to render to texture. The default implementation provides the built-in effects such as tint and
    * opacity. If this is overridden, you will need to provide those effects in your implementation in
    * order to use them.
    */
    virtual void postRender(std::shared_ptr<Texture> texture, const AffineTransformation& transformation);

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
    virtual void mouseUp(MouseButton button, int startX, int startY, int x, int y);
    virtual void mouseWheel(int xPos, int yPos, int xWheel, int yWheel);
    virtual void mouseDrag(int startX, int startY, int x, int y) {}
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
    * Calls update on this view and its subviews. This is normally done by the window each
    * frame before rendering is done.
    */
    void updateAndUpdateSubviews();

    /**
    * Renders the view and its subviews.
    *
    * @param area the area within the target to render to. the view will fill this area
    * @param clipBounds the bounds within the target to clip rendering of the view and its children to
    */
    void renderAndRenderSubviews(const RenderTarget* target, const Rectangle<int>& area, boost::optional<Rectangle<int>> clipBounds = boost::none);

    bool dispatchMouseDown(MouseButton button, int x, int y);
    bool dispatchMouseUp(MouseButton button, int startX, int startY, int x, int y);
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

    bool             _rendersToTexture = false;
    bool             _cachesRender = false;
    bool             _hasCachedRender = false;
    bool             _clipsToBounds = true;

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

    Color            _backgroundColor{0.0, 0.0};
    Color            _tintColor{1.0};

    std::unique_ptr<opengl::Framebuffer> _renderCache;
    opengl::Framebuffer::Attachment* _renderCacheColorAttachment = nullptr;
    std::shared_ptr<WeakTexture> _renderCacheTexture = std::make_shared<WeakTexture>();
    
    struct Listener {
        Listener(std::type_index index, std::function<void(const void*, View*)> action, Relation relation)
            : index{index}, action{std::move(action)}, relation{relation} {}

        std::type_index index;
        std::function<void(const void*, View*)> action;
        Relation relation;
    };
    std::list<Listener> _listeners;
    
    std::unordered_map<size_t, void*> _provisions;
    
    AbstractTaskScheduler::TaskScope _taskScope;

    void _setBounds(const Rectangle<int>& bounds);

    void _dispatchFutureVisibilityChange(bool visible);
    void _dispatchVisibilityChange(bool visible);
    void _dispatchWindowChange(Window* window);
    void _mouseExit();

    bool _requiresTextureRendering();
    void _renderAndRenderSubviews(const RenderTarget* target, const Rectangle<int>& area, boost::optional<Rectangle<int>> clipBounds = boost::none);
    
    void _post(std::type_index index, const void* ptr, Relation relation);
    void _listen(std::type_index index, std::function<void(const void*, View*)> action, Relation relation);
    void* _get(size_t hash) const;
    
    AbstractTaskScheduler* _taskScheduler() const;
};

}}
