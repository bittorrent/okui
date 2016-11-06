#pragma once

#include <okui/config.h>

#include <okui/TextureInterface.h>

#include <vector>

namespace okui {

class TextureHandle {
public:
    TextureHandle() {}
    TextureHandle(std::nullptr_t) {}
    explicit TextureHandle(std::shared_ptr<TextureInterface> texture) : _texture(texture) {}
    TextureHandle(const TextureHandle&) = delete;
    TextureHandle(TextureHandle&&) = default;
    TextureHandle& operator=(const TextureHandle&) = delete;
    TextureHandle& operator=(TextureHandle&&) = default;

    explicit operator bool() const                             { return _texture.get(); }
    operator std::shared_ptr<TextureInterface>()               { return _texture; }
    std::shared_ptr<TextureInterface> operator->()             { return _texture; }
    std::shared_ptr<const TextureInterface> operator->() const { return _texture; }
    TextureInterface& operator*()                              { return *_texture; }
    const TextureInterface& operator*() const                  { return *_texture; }

    std::shared_ptr<TextureInterface> texture() { return _texture; }
    bool isLoaded() const                       { return _texture && _texture->isLoaded(); }

    /**
    * Used to create a new handle to the same texture, but with no registered callbacks.
    */
    TextureHandle newHandle();

    /**
    * Use this to register a callback that should be invoked once loaded.
    */
    void onLoad(std::function<void()> callback);

    /**
    * Use this to invoke registered callbacks on all handles tied to this one.
    */
    void invokeLoadCallbacks();

    bool unique() const { return _texture.unique(); }

private:
    using OwnedCallbacks  = std::vector<std::shared_ptr<std::function<void()>>>;
    using WeakCallbacks   = std::vector<std::weak_ptr<std::function<void()>>>;

    std::shared_ptr<TextureInterface> _texture;
    OwnedCallbacks                    _callbacks;
    std::shared_ptr<WeakCallbacks>    _sharedCallbacks = std::make_shared<WeakCallbacks>();
};

} // namespace okui
