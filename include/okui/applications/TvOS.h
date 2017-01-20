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

#if SCRAPS_TVOS

#include <okui/applications/IOS.h>

namespace okui::applications {

/**
* Provides some native Platform overrides for TvOS.
*
* This isn't intended to be a full implementation, but can be used as a base for other implementations.
*/
template <typename Base>
class TvOS : public IOS<Base> {
public:
    virtual std::string userStoragePath() const override {
        NSArray* paths = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES);
        return std::string{[[paths firstObject] UTF8String]};
    }

    virtual void keyUp(KeyCode key, KeyModifiers mod, bool repeat) override {
        if (key == KeyCode::kMenu) {
            return this->command(kCommandBack);
        }
        IOS<Base>::keyUp(key, mod, repeat);
    }

    virtual bool isTenFootUI() const override {
        return true;
    }
};


} // namespace okui::applications

#endif
