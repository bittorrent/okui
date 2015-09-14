#pragma once

#include "onair/okui/config.h"

#if ONAIR_IOS || ONAIR_TVOS

#include "onair/okui/applications/Apple.h"

namespace onair {
namespace okui {
namespace applications {

/**
* Provides some native Platform overrides for iOS.
*
* This isn't intended to be a full implementation, but can be used as a base for other implementations.
*/
class IOS : public Apple {
public:
    using Apple::Apple;

    virtual bool canOpenURL(const char* url) const override;
    virtual bool openURL(const char* url) override;

    virtual void openDialog(Window* window, const char* title, const char* message, const std::vector<std::string>& buttons, std::function<void(int)> action = std::function<void(int)>()) override;

    virtual void showStatusBar() override;
    virtual void hideStatusBar() override;

    virtual std::string distinctId() const override;
};

} // namespace applications
} // namespace okui
} // namespace onair

#endif
