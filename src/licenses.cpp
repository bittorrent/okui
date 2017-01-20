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
#include <okui/licenses.h>

#include <scraps/licenses.h>

namespace okui {

namespace {
    namespace jpegturbo {
        #include <tjlicense.c>
    }
    namespace png {
        #include <pnglicense.c>
    }
    namespace sdl2 {
        #include <SDL_license.c>
    }
    namespace utf8 {
        #include <utf8/license.c>
    }
}

std::unordered_map<std::string, std::string> ThirdPartyLicenses() {
    static std::unordered_map<std::string, std::string> ret{{
        {"jpegturbo", {reinterpret_cast<char*>(jpegturbo::license), jpegturbo::license_len}},
        {"png", {reinterpret_cast<char*>(png::LICENSE), png::LICENSE_len}},
        {"sdl2", {reinterpret_cast<char*>(sdl2::COPYING_txt), sdl2::COPYING_txt_len}},
        {"utf8", {reinterpret_cast<char*>(utf8::LICENSE), utf8::LICENSE_len}},
    }};
    static std::once_flag flag;
    std::call_once(flag, [&] {
        auto scraps = scraps::ThirdPartyLicenses();
        ret.insert(scraps.begin(), scraps.end());
    });
    return ret;
}

} // namespace okui
