#pragma once

#include "onair/okui/Application.h"
#include "onair/okui/FileResourceManager.h"
#include "onair/okui/applications/Native.h"
#include "onair/okui/applications/SDL.h"

#if ONAIR_OKUI_HAS_NATIVE_APPLICATION

class TestApplication : public onair::okui::Application<onair::okui::applications::Native<onair::okui::applications::SDL>> {
public:
    struct Essentials {
        onair::okui::FileResourceManager resourceManager{ResourceManagerPath().c_str()};

        static std::string ResourceManagerPath();
    };

    TestApplication() : TestApplication(std::make_unique<Essentials>()) {}

private:
    TestApplication(std::unique_ptr<Essentials>&& essentials)
        : Application("Test Application", "BitTorrent Inc.", &essentials->resourceManager)
        , _essentials(std::move(essentials))
    {}

    std::unique_ptr<Essentials> _essentials;
};

#endif // ONAIR_OKUI_HAS_NATIVE_APPLICATION
