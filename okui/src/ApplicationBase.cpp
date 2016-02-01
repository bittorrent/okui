#include "onair/okui/ApplicationBase.h"

namespace onair {
namespace okui {

void ApplicationBase::post(View* sender, std::type_index index, const void* message, View::Relation relation) {
    auto range = _listeners.equal_range(index);
    for (auto it = range.first; it != range.second; ++it) {
        if (it->second.view->hasRelation(relation, sender) && sender->hasRelation(it->second.relation, it->second.view)) {
            (*it->second.action)(message, sender);
        }
    }
}

void ApplicationBase::addListener(View* view, std::type_index index, std::function<void(const void*, View*)>* action, View::Relation relation) {
    _listeners.emplace(index, Listener(view, action, relation));
}

void ApplicationBase::removeListeners(View* view) {
    for (auto it = _listeners.begin(); it != _listeners.end();) {
        if (it->second.view == view) {
            it = _listeners.erase(it);
        } else {
            ++it;
        }
    }
}

} // namespace okui
} // namespace onair
