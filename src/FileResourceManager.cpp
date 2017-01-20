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
#include <okui/FileResourceManager.h>

#include <fstream>

namespace okui {

std::shared_ptr<std::string> FileResourceManager::load(stdts::string_view name) {
    auto hashable = std::string(name);

    if (auto hit = _cache.get(hashable)) {
        return hit;
    }

    auto path = _directory + "/";
    path.append(name.data(), name.size());
    std::ifstream f(path);

    if (!f.is_open()) {
        SCRAPS_LOGF_ERROR("error opening resource: %s", path.c_str());
        return nullptr;
    }

    f.seekg(0, std::ios::end);
    auto size = f.tellg();
    std::string buffer(size, ' ');
    f.seekg(0);
    f.read(&buffer[0], size);

    return _cache.add(std::move(buffer), hashable);
}

} // namespace okui
