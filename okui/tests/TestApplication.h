#pragma once

#include "onair/okui/Application.h"
#include "onair/okui/FileResourceManager.h"
#include "onair/okui/platforms/SDL.h"

class TestApplication : public onair::okui::Application {
public:
    TestApplication() : Application("Test Application", "BitTorrent Inc.", &platform, &resourceManager) {}

    onair::okui::platforms::SDL platform;
    onair::okui::FileResourceManager resourceManager{"./"};
};
