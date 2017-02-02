/**
* Copyright 2017 BitTorrent Inc.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*    http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/
#pragma once

#include <okui/config.h>

#include <okui/FileResourceManager.h>
#include <okui/applications/Native.h>

#if ONAIR_OKUI_HAS_NATIVE_APPLICATION

#include <okui/applications/SDL.h>

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
