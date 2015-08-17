#include "onair/okui/TextureHandle.h"

namespace onair {
namespace okui {

TextureHandle::TextureHandle(std::shared_ptr<Texture> texture, TextureHandle& other)
    : _texture{texture}, _sharedCallbacks{other._sharedCallbacks}
{}

TextureHandle TextureHandle::newHandle() {
    return TextureHandle{_texture, *this};
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

}}
