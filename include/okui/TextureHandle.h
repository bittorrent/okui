#pragma once

#include <okui/config.h>

#include <okui/TextureInterface.h>

#include <scraps/Cache.h>

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

namespace scraps {

/**
* Thread-safe.
*/
template <>
class Cache<okui::TextureHandle> {
public:
    // order matters. policy can be upgraded, but not downgraded via add
    enum Policy {
        kRemoveUnreferenced,
        kKeepForever,
    };

    /**
    * Gets the given entry from the cache if it exists.
    */
    template <typename T>
    okui::TextureHandle get(T&& hashable) {
        auto hash = std::hash<std::remove_cv_t<std::remove_reference_t<T>>>()(std::forward<T>(hashable));
        std::lock_guard<std::mutex> l(_mutex);
        auto it = _entries.find(hash);
        return it == _entries.end() ? nullptr : it->second.entry.newHandle();
    }

    /**
    * Adds the given entry to the cache and returns it.
    *
    * The returned entry may be different than the one given if it already
    * exists in the cache, so always use the returned entry.
    */
    template <typename T>
    okui::TextureHandle add(okui::TextureHandle& entry, T&& hashable, Policy policy = kRemoveUnreferenced) {
        return add(entry.newHandle(), std::forward<T>(hashable), policy);
    }

    /**
    * Adds the given entry to the cache and returns it.
    *
    * Ownership of entry is relinquished.
    */
    template <typename T>
    okui::TextureHandle add(okui::TextureHandle&& entry, T&& hashable, Policy policy = kRemoveUnreferenced) {
        auto hash = std::hash<std::remove_cv_t<std::remove_reference_t<T>>>()(std::forward<T>(hashable));
        std::lock_guard<std::mutex> l(_mutex);

        auto it = _entries.find(hash);
        if (it != _entries.end()) {
            if (policy > it->second.policy) {
                it->second.policy = policy;
            }
            return it->second.entry.newHandle();
        }

        _removeUnreferenced();

        _entries[hash] = {entry.newHandle(), policy};
        return std::move(entry);
    }

    /**
    * Removes all entries from the cache.
    */
    void clear() {
        std::lock_guard<std::mutex> l(_mutex);
        _entries.clear();
    }

    /**
    * Removes an entry from the cache.
    */
    template <typename T>
    void remove(T&& hashable) {
        auto hash = std::hash<std::remove_cv_t<std::remove_reference_t<T>>>()(std::forward<T>(hashable));
        std::lock_guard<std::mutex> l(_mutex);
        _entries.erase(hash);
    }

    /**
    * Removed unreferenced cache entires with the kRemoveUnreferenced policy
    */
    void removeUnreferenced() {
        std::lock_guard<std::mutex> l(_mutex);
        _removeUnreferenced();
    }

    /**
    * Returns the number of entries currently in the cache.
    */
    size_t size() {
        std::lock_guard<std::mutex> l(_mutex);
        return _entries.size();
    }

private:
    struct EntryInfo {
        okui::TextureHandle entry;
        Policy policy;
    };

    void _removeUnreferenced() {
        for (auto it = _entries.begin(); it != _entries.end();) {
            if (it->second.entry.unique() && it->second.policy == kRemoveUnreferenced) {
                it = _entries.erase(it);
            } else {
                ++it;
            }
        }
    }

    mutable std::mutex _mutex;
    std::unordered_map<size_t, EntryInfo> _entries;
};

} // namespace scraps
