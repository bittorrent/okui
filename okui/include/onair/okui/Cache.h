#pragma once

#include "onair/okui/config.h"

#include <mutex>

#include <unordered_map>

namespace onair {
namespace okui {

/**
* Thread-safe.
*/
template <typename Entry>
class Cache {
public:
    using EntryReference = std::shared_ptr<Entry>;

    // order matters. policy can be upgraded, but not downgraded via add
    enum Policy {
        kRemoveUnreferenced,
        kKeepForever,
    };

    /**
    * Gets the given entry from the cache if it exists.
    */
    template <typename T>
    EntryReference get(T&& hashable) const {
        std::lock_guard<std::mutex> l(_mutex);
        auto hash = std::hash<typename std::remove_reference<T>::type>()(std::forward<T>(hashable));

        auto it = _entries.find(hash);
        return it == _entries.end() ? nullptr : it->second.entry;
    }

    /**
    * Adds the given entry to the cache and returns it.
    *
    * The returned entry may be different than the one given if it already
    * exists in the cache, so always use the returned entry.
    */
    template <typename T>
    EntryReference add(std::shared_ptr<Entry> entry, T&& hashable, Policy policy = kRemoveUnreferenced) {
        std::lock_guard<std::mutex> l(_mutex);
        auto hash = std::hash<typename std::remove_reference<T>::type>()(std::forward<T>(hashable));

        auto it = _entries.find(hash);
        if (it != _entries.end()) {
            if (policy > it->second.policy) {
                it->second.policy = policy;
            }
            return it->second.entry;
        }

        _removeExpired();

        EntryInfo info;
        info.entry = entry;
        info.policy = policy;
        _entries[hash] = info;
        return info.entry;
    }

    /**
    * Adds the given entry to the cache and returns it.
    *
    * Ownership of entry is relinquished.
    */
    template <typename T>
    EntryReference add(Entry&& entry, T&& hashable, Policy policy = kRemoveUnreferenced) {
        return add(std::make_shared<Entry>(std::move(entry)), std::forward<T>(hashable), policy);
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
        std::lock_guard<std::mutex> l(_mutex);
        auto hash = std::hash<typename std::remove_reference<T>::type>()(std::forward<T>(hashable));
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
    mutable std::mutex _mutex;

    struct EntryInfo {
        std::shared_ptr<Entry> entry;
        Policy policy;
    };

    std::unordered_map<size_t, EntryInfo> _entries;

    void _removeExpired() {
        for (auto it = _entries.begin(); it != _entries.end();) {
            if (it->second.entry.use_count() == 1 && it->second.policy == kRemoveUnreferenced) {
                it = _entries.erase(it);
            } else {
                ++it;
            }
        }
    }
};

}}
