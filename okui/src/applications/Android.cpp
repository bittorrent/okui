#include "onair/okui/applications/Android.h"

#if ONAIR_ANDROID

namespace onair {
namespace okui {
namespace applications {

void Android::openDialog(Window* window,
                         const char* title,
                         const char* message,
                         const std::vector<std::string>& buttons,
                         std::function<void(int)> action) {
    // TODO: implement
    ONAIR_ASSERT(false);
}

} // namespace applications
} // namespace okui
} // namespace onair

#endif
