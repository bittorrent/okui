#pragma once

#include "bittorrent/ui/Application.h"
#include "bittorrent/ui/FileResourceManager.h"
#include "bittorrent/ui/platforms/SDL.h"

class TestApplication : public bittorrent::ui::Application {
public:
    TestApplication() : Application("Test Application", "BitTorrent Inc.", &platform, &resourceManager) {}

    bittorrent::ui::platforms::SDL platform;
    bittorrent::ui::FileResourceManager resourceManager{"./"};
};
