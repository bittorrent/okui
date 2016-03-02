#pragma once

#include "onair/okui/config.h"
#include "onair/okui/Texture.h"

#include <vector>

namespace onair {
namespace okui {

class TextureHandle {
public:
    TextureHandle() {}
    TextureHandle(std::nullptr_t null) {}
    explicit TextureHandle(std::shared_ptr<Texture> texture) : _texture(texture) {}

    /**
    * Creates a handle for the given texture and ties it to another handle.
    */
    TextureHandle(std::shared_ptr<Texture> texture, TextureHandle& other);

    TextureHandle(TextureHandle&& other) = default;
    TextureHandle& operator=(TextureHandle&& other) = default;

    TextureHandle(const TextureHandle& other) = delete;
    TextureHandle& operator=(const TextureHandle& other) = delete;

    explicit operator bool() const { return _texture.get(); }
    operator std::shared_ptr<Texture>() { return _texture; }
    std::shared_ptr<Texture> operator->() { return _texture; }
    std::shared_ptr<const Texture> operator->() const { return _texture; }
    Texture& operator*() { return *_texture; }
    const Texture& operator*() const { return *_texture; }

    std::shared_ptr<Texture> texture() { return _texture; }

    bool isLoaded() const { return _texture && _texture->isLoaded(); }

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

private:
    std::shared_ptr<Texture> _texture;

    std::vector<std::shared_ptr<std::function<void()>>> _callbacks;

    using WeakCallbacks = std::vector<std::weak_ptr<std::function<void()>>>;

    std::shared_ptr<WeakCallbacks> _sharedCallbacks = std::make_shared<WeakCallbacks>();
};

}}
