#pragma once

#include "onair/okui/config.h"

#if ONAIR_TVOS

#include "onair/okui/applications/IOS.h"

namespace onair {
namespace okui {
namespace applications {

/**
* Provides some native Platform overrides for iOS.
*
* This isn't intended to be a full implementation, but can be used as a base for other implementations.
*/
class TvOS : public IOS {
public:
    virtual std::string userStoragePath() const override;

    virtual void openDialog(Window* window,
                            const char* title,
                            const char* message,
                            const std::vector<std::string>& buttons,
                            std::function<void(int)> action = std::function<void(int)>()) override {} // TODO: implement
};

} // namespace applications
} // namespace okui
} // namespace onair

#endif
