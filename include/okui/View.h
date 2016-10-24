#pragma once

#include "okui/config.h"

#include "okui/opengl/Framebuffer.h"
#include "okui/shaders/ColorShader.h"
#include "okui/shaders/DistanceFieldShader.h"
#include "okui/shaders/TextureShader.h"
#include "okui/Application.h"
#include "okui/Color.h"
#include "okui/Point.h"
#include "okui/Rectangle.h"
#include "okui/RenderTarget.h"
#include "okui/Relation.h"
#include "okui/Responder.h"
#include "okui/ShaderCache.h"
#include "okui/TextureHandle.h"
#include "okui/TouchpadFocus.h"
#include "okui/WeakTexture.h"

#include "scraps/AbstractTaskScheduler.h"
#include "scraps/TreeNode.h"
#include "scraps/utility.h"

#include "stdts/optional.h"

#include <gsl.h>

#include <list>
#include <typeindex>
#include <unordered_map>

namespace okui {

class Application;
class BitmapFont;
class Window;

/**
* View rendering can be cached or buffered to a texture. One reason this may be done is to apply post-rendering
* effects such as tinting or reflection. When this is done, the contents of the view are clipped to its bounds.
*/
class View : public Responder, private scraps::TreeNode<View> {
public:
    using Relation = okui::Relation;

    View() = default;
    explicit View(std::string name) : _name{std::move(name)} {}
    View(const View&) = delete;
    View(View&&) = delete;
    View& operator=(const View&) = delete;
    View& operator=(View&&) = delete;

    /**
    * It is an error to destroy a focused view. Remove the view first if needed.
    */
    virtual ~View();

    std::string name() const       { return _name.empty() ? scraps::Demangle(typeid(*this).name()) : _name; }
    void setName(std::string name) { _name = std::move(name); }

    void addSubview(View* view);
    template <typename... Views>
    void addSubviews(View* view, Views&&... views)  { _addSubviews(view, std::forward<Views>(views)...); }
    void addHiddenSubview(View* view);
    void removeSubview(View* view);
    template <typename... Views>
    void removeSubviews(View* view, Views&&... views) { _removeSubviews(view, std::forward<Views>(views)...); }
    void removeSubviews();

    Window* window() const                    { return _window; }
    Application* application() const;

    const View* superview() const             { return parent(); }
    View* superview()                         { return parent(); }
    const std::list<View*>& subviews() const  { return children(); }

    const Rectangle<double>& bounds() const   { return _bounds; }
    math::Vector<double, 2> size() const      { return _bounds.size(); }
    Point<double> position() const            { return _bounds.position(); }

    double width() const                      { return _bounds.width; }
    double height() const                     { return _bounds.height; }

    template <typename... Args>
    void setBounds(Args&&... args)            { _setBounds(Rectangle<double>(std::forward<Args>(args)...)); }

    void setPosition(double x, double y)      { _setBounds({x, y, _bounds.width, _bounds.height}); }
    void setPosition(const Point<double>& p)  { setPosition(p.x, p.y); }

    void setSize(double width, double height) { _setBounds({_bounds.x, _bounds.y, width, height}); }
    void setSize(const math::Vector<double, 2>& s) { setSize(s.x, s.y); }

    Rectangle<double> windowBounds() const;

    /**
    * Sets bounds as a percent of superview bounds (0-1)
    */
    void setBoundsRelative(double x, double y, double width, double height);

    void setInterceptsInteractions(bool intercepts, bool childrenIntercept);

    ShaderCache* shaderCache();

    void setIsVisible(bool isVisible = true);
    bool isVisible() const { return _isVisible; }

    void show() { setIsVisible(); }
    void hide() { setIsVisible(false); }

    /**
    * Allows scaling of the view by a particular factor.
    */
    void setScale(double scale) { setScale(scale, scale); }
    void setScale(double scaleX, double scaleY);

    /**
    * Sets the view's background color.
    */
    void setBackgroundColor(const Color& color) { _backgroundColor = color; }
    const Color& backgroundColor() const        { return _backgroundColor; }

    /**
    * Sets the view's tint. If this is anything but opaque white, this is a post-rendering effect that
    * clips the view's contents.
    */
    void setTintColor(const Color& color);
    const Color& tintColor() const        { return _tintColor; }

    /**
    * Sets the view's opacity. If this is anything but 1, this is a post-rendering effect that
    * clips the view's contents.
    *
    * This effectively sets the alpha component of the view's tint color.
    */
    void setOpacity(double opacity)  { setTintColor(_tintColor.withAlphaF(opacity)); }
    double opacity() const           { return _tintColor.alphaF(); }

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
    * Override this if the view can become focused. By default, the view can only become focus
    * if a preferred focus is set.
    */
    virtual bool canBecomeFocus() { return _preferredFocus; }

    /**
    * Makes the view the current focus. If the view has a preferred focus, the preferred focus will
    * become the focus instead.
    */
    void focus();

    /**
     * Sets the focus to the next ancestor that can be focused which doesn't result in this view
     * retaining focus (which would otherwise happen if your ancestor's preferred focus was you)
     */
    void focusAncestor();

    /**
    * If the view or any of its children are focused, unfocuses them.
    */
    void unfocus();

    /**
    * If this view would receive focus and has a preferred focus, the preferred focus will become focus instead.
    */
    View* preferredFocus() const       { return _preferredFocus; }
    void setPreferredFocus(View* view) { _preferredFocus = view; }

    View* nextFocus() const     { return _nextFocus; }
    View* previousFocus() const { return _previousFocus; }

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

    /**
    * Returns true if the view or any of its children has focus.
    */
    bool isFocus() const;

    TouchpadFocus& touchpadFocus() { return _touchpadFocus; }

    /**
    * @param rendersToTexture if true, the view is rendered to a texture, making it available via renderTexture
    */
    void setRendersToTexture(bool rendersToTexture = true) { _rendersToTexture = rendersToTexture; }

    /**
    * If the view is set to render to a texture, this can be used to obtain said texture.
    */
    std::shared_ptr<TextureInterface> renderTexture() const;

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
    T* shader(const char* identifier);

    const AffineTransformation& renderTransformation() const { return _renderTransformation; }

    /**
    * Converts a point from local coordinates to superview coordinates.
    */
    Point<double> localToSuperview(double x, double y) const { return localToAncestor(x, y, superview()); }
    Point<double> localToSuperview(const Point<double>& p) const { return localToSuperview(p.x, p.y); }

    /**
    * Converts a point from local coordinates to the coordinates of an ancestor.
    */
    Point<double> localToAncestor(double x, double y, const View* ancestor) const;
    Point<double> localToAncestor(const Point<double>& p, const View* ancestor) const { return localToAncestor(p.x, p.y, ancestor); }

    /**
    * Converts a point from super coordinates to local coordinates.
    */
    Point<double> superviewToLocal(double x, double y) const;
    Point<double> superviewToLocal(const Point<double>& p) const;

    /**
    * Converts a point from local coordinates to window coordinates.
    */
    Point<double> localToWindow(double x, double y) const;
    Point<double> localToWindow(const Point<double>& p) const;

    /**
    * Returns true if this view has the given relation to the given view.
    *
    * For example, hasRelation(kDescendant, superview()) should return true.
    */
    bool hasRelation(Relation relation, const View* view) const;

    /**
    * Returns the first view that both views have in their hierarchy, if any.
    */
    const View* commonView(const View* other) const { return commonNode(other); }

    using TreeNode::isDescendantOf;
    using TreeNode::isAncestorOf;

    /**
    * Posts a message to listeners with the given relation. The view and its potential listeners must have an
    * associated application for the message to be delivered.
    *
    * For example, post(message, Relation::kAncestor) posts the message to listeners that are ancestors of this view.
    */
    template <typename T>
    void post(T& message, Relation relation = Relation::kHierarchy) {
        _post(std::type_index(typeid(typename std::decay_t<T>)), &message, relation);
    }
    template <typename T>
    void post(T&& message, Relation relation = Relation::kHierarchy) {
        T m = std::move(message);
        _post(std::type_index(typeid(typename std::decay_t<T>)), &m, relation);
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
    auto listen(Action&& action, Relation relation = Relation::kHierarchy) -> typename std::enable_if<ListenerAction<Action>::ArgumentCount::value == 1, void>::type {
        using MessageType = typename ListenerAction<Action>::MessageType;
        _listen(std::type_index(typeid(MessageType)), [action = std::forward<Action>(action)](const void* message, View* sender) { action(*reinterpret_cast<const MessageType*>(message)); }, relation);
    }

    template <typename Action>
    auto listen(Action&& action, Relation relation = Relation::kHierarchy) -> typename std::enable_if<ListenerAction<Action>::ArgumentCount::value == 2, void>::type {
        using MessageType = typename ListenerAction<Action>::MessageType;
        _listen(std::type_index(typeid(MessageType)), [action = std::forward<Action>(action)](const void* message, View* sender) { action(*reinterpret_cast<const MessageType*>(message), sender); }, relation);
    }

    /**
    * Makes the given object available via get(). This can be used to store arbitrary state with the view.
    *
    * @param relation the object will only be available to views of the given relation
    */
    template <typename T>
    auto set(T&& object, Relation relation = Relation::kHierarchy) {
        _provisions.emplace_front(std::forward<T>(object), relation);
        return stdts::any_cast<std::decay_t<T>>(&_provisions.front().object);
    }

    /**
    * Finds an object provided via provide. It will return a pointer to the first object it finds of the
    * specified type. If more than one objects of the given type has been provided, it is undefined
    * which one will be returned.
    *
    * @param relation only objects from views of the given relation will be returned
    */
    template <typename T>
    T* find(Relation relation = Relation::kHierarchy);

    template <typename T>
    T* get() { return find<T>(Relation::kSelf); }

    template <typename T>
    T* inherit() { return find<T>(Relation::kAncestor); }

    /**
    * Asynchronously schedules a function to be invoked using the application's task scheduler.
    *
    * If the view is destroyed, the invocation will be canceled.
    */
    template <typename... Args>
    auto async(Args&&... args) -> decltype(std::declval<scraps::AbstractTaskScheduler>().async(std::forward<Args>(args)...)) {
        return _taskScheduler()->async(_taskScope, std::forward<Args>(args)...);
    }

    /**
    * Asynchronously schedules a function to be invoked after a delay using the application's task scheduler.
    *
    * If the view is destroyed, the invocation will be canceled.
    */
    template <typename... Args>
    auto asyncAfter(Args&&... args) -> decltype(std::declval<scraps::AbstractTaskScheduler>().asyncAfter(std::forward<Args>(args)...)) {
        return _taskScheduler()->asyncAfter(_taskScope, std::forward<Args>(args)...);
    }

    /**
    * Asynchronously schedules a function to be invoked at a time using the application's task scheduler.
    *
    * If the view is destroyed, the invocation will be canceled.
    */
    template <typename... Args>
    auto asyncAt(Args&&... args) -> decltype(std::declval<scraps::AbstractTaskScheduler>().asyncAt(std::forward<Args>(args)...)) {
        return _taskScheduler()->asyncAt(_taskScope, std::forward<Args>(args)...);
    }

    /**
    * The hook provided here will be called before rendering each frame.
    */
    void addUpdateHook(const std::string& handle, std::function<void()> hook);

    /**
    * The hook provided here will no longer be called.
    */
    void removeUpdateHook(const std::string& handle);

    /**
    * Override this to do drawing.
    */
    virtual void render() {}

    virtual void render(const RenderTarget* renderTarget, const Rectangle<int>& area) { render(); }

    /**
    * Override this to implement custom post-render effects. For this method to be called, the view must be
    * set to render to texture. The default implementation provides the built-in effects such as tint and
    * opacity. If this is overridden, you will need to provide those effects in your implementation in
    * order to use them.
    *
    * @param texture the view, rendered to a texture with premultiplied alpha
    */
    virtual void postRender(std::shared_ptr<TextureInterface> texture, const AffineTransformation& transformation);

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
    virtual bool hitTest(double x, double y);
    bool hitTest(const Point<double>& p) { return hitTest(p.x, p.y); }

    /**
    * Return the most descendant visible view which intersects with (x, y)
    */
    View* hitTestView(double x, double y);
    View* hitTestView(const Point<double>& p) { return hitTestView(p.x, p.y); }

    /**
    * Override these to handle mouse events. Call the base implementation to pass on the event.
    */
    virtual void mouseDown(MouseButton button, double x, double y);
    virtual void mouseUp(MouseButton button, double startX, double startY, double x, double y);
    virtual void mouseWheel(double xPos, double yPos, int xWheel, int yWheel);
    virtual void mouseDrag(double startX, double startY, double x, double y);
    virtual void mouseMovement(double x, double y);
    virtual void mouseEnter() {}
    virtual void mouseExit() {}

    /**
    * Called whenever the view or one of its subviews gains focus.
    */
    virtual void focusGained() {}

    /**
    * Called whenever the view and its subviews lose focus.
    */
    virtual void focusLost() {}

    /**
    * Called whenever the view or any subview gains or loses focus.
    */
    virtual void focusChanged() {}

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
    void renderAndRenderSubviews(const RenderTarget* target, const Rectangle<int>& area, stdts::optional<Rectangle<int>> clipBounds = stdts::nullopt);

    bool dispatchMouseDown(MouseButton button, double x, double y);
    bool dispatchMouseUp(MouseButton button, double startX, double startY, double x, double y);
    bool dispatchMouseMovement(double x, double y);
    bool dispatchMouseWheel(double xPos, double yPos, int xWheel, int yWheel);
    void dispatchUpdate(std::chrono::high_resolution_clock::duration elapsed);

    // Responder overrides
    virtual Responder* nextResponder() override;
    virtual void keyDown(KeyCode key, KeyModifiers mod, bool repeat) override;
    virtual void touchUp(size_t finger, Point<double> position, double pressure) override;
    virtual void touchDown(size_t finger, Point<double> position, double pressure) override;
    virtual void touchMovement(size_t finger, Point<double> position, Point<double> distance, double pressure) override;

private:
    friend class scraps::TreeNode<View>;
    friend class Window;

    struct Listener {
        Listener(std::type_index index, std::function<void(const void*, View*)> action, Relation relation)
            : index{index}, action{std::move(action)}, relation{relation} {}

        std::type_index index;
        std::function<void(const void*, View*)> action;
        Relation relation;
    };

    struct Provision {
        Provision(stdts::any object, Relation relation)
            : object{std::move(object)}, relation{relation} {}

        stdts::any object;
        Relation relation;
    };

    void _addSubviews() {}
    template <typename... Views>
    void _addSubviews(View* view, Views&&... views);

    void _removeSubviews() {}
    template <typename... Views>
    void _removeSubviews(View* view, Views&&... views);

    void _setBounds(const Rectangle<double>& bounds);

    void _invalidateSuperviewRenderCache();

    void _dispatchFutureVisibilityChange(bool visible);
    void _dispatchVisibilityChange(bool visible);
    void _dispatchWindowChange(Window* window);
    void _mouseExit();

    void _updateFocusableRegions(std::vector<std::tuple<View*, Rectangle<double>>>& regions);

    bool _requiresTextureRendering();
    void _renderAndRenderSubviews(const RenderTarget* target, const Rectangle<int>& area, bool shouldClear = false, stdts::optional<Rectangle<int>> clipBounds = stdts::nullopt);

    void _post(std::type_index index, const void* ptr, Relation relation);
    void _listen(std::type_index index, std::function<void(const void*, View*)> action, Relation relation);

    template <typename F>
    void _traverseRelation(Relation relation, F&& function);

    std::vector<View*> _topViewsForRelation(Relation relation);

    scraps::AbstractTaskScheduler* _taskScheduler() const;
    void _updateTouchFocus(std::chrono::high_resolution_clock::duration elapsed);

    void _checkUpdateSubscription();
    bool _shouldSubscribeToUpdates();

    std::string _name;
    bool _isVisible                     = true;
    bool _rendersToTexture              = false;
    bool _cachesRender                  = false;
    bool _hasCachedRender               = false;
    bool _clipsToBounds                 = true;
    bool _interceptsInteractions        = true;
    bool _childrenInterceptInteractions = true;

    Window* _window                     = nullptr;
    View*   _subviewWithMouse           = nullptr;
    View*   _nextFocus                  = nullptr;
    View*   _previousFocus              = nullptr;
    View*   _preferredFocus             = nullptr;

    Rectangle<double>    _bounds;
    Point<double>        _scale{1.0, 1.0};
    Color                _backgroundColor = Color::kTransparentBlack;
    Color                _tintColor = Color::kWhite;
    AffineTransformation _renderTransformation;

    std::unique_ptr<opengl::Framebuffer> _renderCache;
    opengl::Framebuffer::Attachment*     _renderCacheColorAttachment = nullptr;
    std::shared_ptr<WeakTexture>         _renderCacheTexture = std::make_shared<WeakTexture>();

    std::list<Listener>                               _listeners;
    std::list<Provision>                              _provisions;
    TouchpadFocus                                     _touchpadFocus;
    std::unordered_map<size_t, std::function<void()>> _updateHooks;

    scraps::AbstractTaskScheduler::TaskScope          _taskScope; /* must be the last member */
};

template <typename... Views>
void View::_addSubviews(View* view, Views&&... views) {
    addSubview(view);
    _addSubviews(std::forward<Views>(views)...);
}

template <typename... Views>
void View::_removeSubviews(View* view, Views&&... views) {
    removeSubview(view);
    _removeSubviews(std::forward<Views>(views)...);
}

template <typename T>
T* View::shader(const char* identifier) {
    auto s = shaderCache()->get(std::string(identifier));
    if (!s) {
        s = shaderCache()->add(std::make_unique<T>(), std::string(identifier), ShaderCache::Policy::kKeepForever);
    }
    auto ret = dynamic_cast<T*>(s->get());
    ret->setTransformation(renderTransformation());
    return ret;
}

template <typename T>
T* View::find(Relation relation) {
    T* ret = nullptr;
    _traverseRelation(relation, [&](View* view, bool* shouldContinue) {
        for (auto& provision : view->_provisions) {
            if (!hasRelation(provision.relation, view)) { continue; }
            if (auto object = stdts::any_cast<T>(&provision.object)) {
                ret = object;
                *shouldContinue = false;
                return 1;
            }
            if (auto pointer = stdts::any_cast<T*>(&provision.object)) {
                ret = *pointer;
                *shouldContinue = false;
                return 1;
            }
        }
        return 0;
    });
    if (!ret && application() && (relation == Relation::kHierarchy || relation == Relation::kAny || relation == Relation::kAncestor)) {
        return application()->get<T>();
    }
    return ret;
}

template <typename F>
void View::_traverseRelation(Relation relation, F&& function) {
    switch (relation) {
        case Relation::kAny: {
            bool shouldContinue = true;
            for (auto& v : _topViewsForRelation(relation)) {
                v->TreeNode::traverseRelation(TreeNode::Relation::kCommonRoot, function, 0, &shouldContinue);
            }
            break;
        }
        case Relation::kHierarchy:  TreeNode::traverseRelation(TreeNode::Relation::kCommonRoot, function, 0); break;
        case Relation::kDescendant: TreeNode::traverseRelation(TreeNode::Relation::kDescendant, function, 0); break;
        case Relation::kAncestor:   TreeNode::traverseRelation(TreeNode::Relation::kAncestor,   function, 0); break;
        case Relation::kSibling:    TreeNode::traverseRelation(TreeNode::Relation::kSibling,    function, 0); break;
        case Relation::kSelf:       TreeNode::traverseRelation(TreeNode::Relation::kSelf,       function, 0); break;
    }
}

} // namespace okui
