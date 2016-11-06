#include <okui/TextureHandle.h>

namespace okui {

TextureHandle TextureHandle::newHandle() {
    TextureHandle handle{_texture};
    handle._sharedCallbacks = _sharedCallbacks;
    return handle;
}

void TextureHandle::onLoad(std::function<void()> callback) {
    auto ptr = std::make_shared<std::function<void()>>(std::move(callback));
    _callbacks.emplace_back(ptr);
    _sharedCallbacks->emplace_back(ptr);
}

void TextureHandle::invokeLoadCallbacks() {
    WeakCallbacks callbacks;
    callbacks.swap(*_sharedCallbacks);
    for (auto& callback : callbacks) {
        if (auto ptr = callback.lock()) {
            (*ptr)();
        }
    }
}

} // namespace okui
