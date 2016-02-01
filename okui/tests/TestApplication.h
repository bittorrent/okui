#pragma once

#include "onair/okui/FileResourceManager.h"
#include "onair/okui/applications/Native.h"

#if ONAIR_OKUI_HAS_NATIVE_APPLICATION

#include "onair/okui/applications/SDL.h"

class TestApplication : public onair::okui::applications::Native<onair::okui::applications::SDL> {
public:
    TestApplication() {
        setResourceManager(&_resourceManager);
    }

    virtual std::string name() const override { return "Test Application"; }
    virtual std::string organization() const override { return "BitTorrent Inc."; }

private:
    static std::string ResourceManagerPath();

    onair::okui::FileResourceManager _resourceManager{ResourceManagerPath().c_str()};
};

#endif // ONAIR_OKUI_HAS_NATIVE_APPLICATION
