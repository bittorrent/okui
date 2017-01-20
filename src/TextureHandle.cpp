/**
* Copyright 2017 BitTorrent Inc.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*    http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/
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
