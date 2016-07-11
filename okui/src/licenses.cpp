#include "onair/okui/licenses.h"

#include "scraps/licenses.h"

namespace onair {
namespace okui {

namespace {
    namespace jpegturbo {
        #include <tjlicense.c>
    }
    namespace png {
        #include <pnglicense.c>
    }
    namespace sdl2 {
        #include <SDL2/license.c>
    }
}

std::unordered_map<std::string, std::string> ThirdPartyLicenses() {
    static std::unordered_map<std::string, std::string> ret{{
        {"jpegturbo", {reinterpret_cast<char*>(jpegturbo::license), jpegturbo::license_len}},
        {"png", {reinterpret_cast<char*>(png::LICENSE), png::LICENSE_len}},
        {"sdl2", {reinterpret_cast<char*>(sdl2::COPYING_txt), sdl2::COPYING_txt_len}},
    }};
    static std::once_flag flag;
    std::call_once(flag, [&] {
        auto scraps = scraps::ThirdPartyLicenses();
        ret.insert(scraps.begin(), scraps.end());
    });
    return ret;
}

}} // namespace onair::okui
