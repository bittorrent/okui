#pragma once

#include "bittorrent/ui/config.h"

#include "bittorrent/mutex.h"

#include <unordered_map>

namespace bittorrent {
namespace ui {

/**
* Thread-safe.
*/
template <typename Entry>
class Cache {
public:
	using EntryReference = shared_ptr<Entry>;

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
		lock_guard<mutex> l(_mutex);
		auto hash = std::hash<typename std::remove_reference<T>::type>()(std::forward<T>(hashable));

		auto it = _entries.find(hash);
		return it == _entries.end() ? nullptr : it->second.entry;
	}

	/**
	* Adds the given entry to the cache and returns it.
	*
	* Ownership of the entry is relinquished to the cache. The returned
	* entry may be different than the one given if it already exists in the
	* cache, so always use the returned entry.
	*/
	template <typename T>
	EntryReference add(Entry&& entry, T&& hashable, Policy policy = kRemoveUnreferenced) {
		lock_guard<mutex> l(_mutex);
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
		info.entry = make_shared<Entry>(std::move(entry));
		info.policy = policy;
		_entries[hash] = info;
		return info.entry;
	}

	/**
	* Removes all entries from the cache.
	*/
	void clear() {
		lock_guard<mutex> l(_mutex);
		_entries.clear();
	}
	
	/**
	* Removes an entry from the cache.
	*/
	template <typename T>
	void remove(T&& hashable) {
		lock_guard<mutex> l(_mutex);
		auto hash = std::hash<typename std::remove_reference<T>::type>()(std::forward<T>(hashable));
		_entries.erase(hash);
	}

	/**
	* Returns the number of entries currently in the cache.
	*/
	size_t size() {
		lock_guard<mutex> l(_mutex);
		return _entries.size();
	}

private:
	mutable mutex _mutex;

	struct EntryInfo {
		shared_ptr<Entry> entry;
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