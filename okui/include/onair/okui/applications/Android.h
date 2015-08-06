#pragma once

#include "onair/okui/config.h"

#if ONAIR_ANDROID

#include "onair/okui/Application.h"

namespace onair {
namespace okui {
namespace applications {

/**
* Provides some native Platform overrides for Android.
*
* This isn't intended to be a full implementation, but can be used as a base for
* other implementations.
*/
class Android : public Application {
public:
    using Application::Application;

    virtual void openDialog(Window* window,
                            const char* title,
                            const char* message,
                            const std::vector<std::string>& buttons,
                            std::function<void(int)> action = std::function<void(int)>()) override;
};

} // namespace applications
} // namespace okui
} // namespace onair

#endif
