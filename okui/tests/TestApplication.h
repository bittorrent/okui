#pragma once

#include "onair/okui/Application.h"
#include "onair/okui/FileResourceManager.h"
#include "onair/okui/platforms/SDL.h"

class TestApplication : public onair::okui::Application {
public:
    struct Essentials {
        onair::okui::platforms::SDL platform;
        onair::okui::FileResourceManager resourceManager{"./resources/"};
    };

    TestApplication() : TestApplication(std::unique_ptr<Essentials>(new Essentials())){}

private:
    TestApplication(std::unique_ptr<Essentials>&& essentials)
        : Application("Test Application", "BitTorrent Inc.", &essentials->platform, &essentials->resourceManager)
        , _essentials(std::move(essentials))
    {}

    std::unique_ptr<Essentials> _essentials;
};
