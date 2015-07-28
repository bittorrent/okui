#pragma once

#include "onair/okui/config.h"

#if __APPLE__

#include "onair/okui/Application.h"

namespace onair {
namespace okui {
namespace applications {

/**
* Provides some native Application overrides for iOS and OS X.
*
* This isn't intended to be a full implementation, but can be used as a base for other implementations.
*/
class Apple : public Application {
public:
	using Application::Application;

    virtual std::string userStoragePath() const override;

    virtual KeyModifier defaultShortcutModifier() const override { return KeyModifier::kSuper; }

    virtual std::string resourcePath() const override;
};

}}}

#endif
