#pragma once

#include "okui/FileResourceManager.h"
#include "okui/applications/Native.h"

#if ONAIR_OKUI_HAS_NATIVE_APPLICATION

#include "okui/applications/SDL.h"

class TestApplicationBase : public okui::applications::Native<okui::applications::SDL> {
public:
    virtual std::string name() const override { return "Test Application"; }
    virtual std::string organization() const override { return "BitTorrent Inc."; }
};


#ifdef SCRAPS_ANDROID

// On Android, the native application provides a resource manager.
using TestApplication = TestApplicationBase;

#else

class TestApplication : public TestApplicationBase {
public:
    TestApplication() {
        setResourceManager(&_resourceManager);
    }

private:
    static std::string ResourceManagerPath();

    okui::FileResourceManager _resourceManager{ResourceManagerPath().c_str()};
};

#endif // SCRAPS_ANDROID


#endif // ONAIR_OKUI_HAS_NATIVE_APPLICATION
