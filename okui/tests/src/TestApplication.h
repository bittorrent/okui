#pragma once

#include "onair/okui/FileResourceManager.h"
#include "onair/okui/applications/Native.h"

#if ONAIR_OKUI_HAS_NATIVE_APPLICATION

#include "onair/okui/applications/SDL.h"

class TestApplicationBase : public onair::okui::applications::Native<onair::okui::applications::SDL> {
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

    onair::okui::FileResourceManager _resourceManager{ResourceManagerPath().c_str()};
};

#endif // SCRAPS_ANDROID


#endif // ONAIR_OKUI_HAS_NATIVE_APPLICATION
