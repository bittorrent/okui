#pragma once

#include "onair/okui/config.h"

#include "onair/okui/TextureHandle.h"

#include "scraps/Cache.h"

namespace scraps {

/**
* Thread-safe.
*/
template <>
class Cache<onair::okui::TextureHandle> {
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
    onair::okui::TextureHandle get(T&& hashable) {
        auto hash = std::hash<typename std::remove_reference<T>::type>()(std::forward<T>(hashable));
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
    onair::okui::TextureHandle add(onair::okui::TextureHandle& entry, T&& hashable, Policy policy = kRemoveUnreferenced) {
        return add(entry.newHandle(), std::forward<T>(hashable), policy);
    }

    /**
    * Adds the given entry to the cache and returns it.
    *
    * Ownership of entry is relinquished.
    */
    template <typename T>
    onair::okui::TextureHandle add(onair::okui::TextureHandle&& entry, T&& hashable, Policy policy = kRemoveUnreferenced) {
        auto hash = std::hash<typename std::remove_reference<T>::type>()(std::forward<T>(hashable));
        std::lock_guard<std::mutex> l(_mutex);

        auto it = _entries.find(hash);
        if (it != _entries.end()) {
            if (policy > it->second.policy) {
                it->second.policy = policy;
            }
            return it->second.entry.newHandle();
        }

        _removeExpired();

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
        auto hash = std::hash<typename std::remove_reference<T>::type>()(std::forward<T>(hashable));
        std::lock_guard<std::mutex> l(_mutex);
        _entries.erase(hash);
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
        onair::okui::TextureHandle entry;
        Policy policy;
    };

    void _removeExpired() {
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
