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

#if SCRAPS_APPLE

#include <okui/UserPreferencesInterface.h>

#import <Foundation/Foundation.h>

namespace okui::applications {

class AppleUserPreferences : public UserPreferencesInterface {
public:
    AppleUserPreferences();
    explicit AppleUserPreferences(const std::string& suiteName);

    AppleUserPreferences(const AppleUserPreferences&) = delete;
    AppleUserPreferences& operator=(const AppleUserPreferences&) = delete;

    virtual bool has(const std::string& key) const override;
    virtual void unset(const std::string& key) override;

    virtual bool getBool(const std::string& key) const override;
    virtual int32_t getInt32(const std::string& key) const override;
    virtual int64_t getInt64(const std::string& key) const override;
    virtual float getFloat(const std::string& key) const override;
    virtual std::string getString(const std::string& key) const override;

    virtual void setBool(const std::string& key, bool value) override;
    virtual void setInt32(const std::string& key, int32_t value) override;
    virtual void setInt64(const std::string& key, int64_t value) override;
    virtual void setFloat(const std::string& key, float value) override;
    virtual void setString(const std::string& key, const std::string& value) override;

private:
    NSUserDefaults* _userDefaults = nil;
};

} // namespace okui::applications

#endif
