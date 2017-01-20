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

#include <string>
#include <cstdint>

namespace okui {

class UserPreferencesInterface {
public:
    virtual ~UserPreferencesInterface() {}

    virtual bool has(const std::string& key) const = 0;
    virtual void unset(const std::string& key) = 0;

    virtual bool getBool(const std::string& key) const = 0;
    virtual int32_t getInt32(const std::string& key) const = 0;
    virtual int64_t getInt64(const std::string& key) const = 0;
    virtual float getFloat(const std::string& key) const = 0;
    virtual std::string getString(const std::string& key) const = 0;

    virtual void setBool(const std::string& key, bool value) = 0;
    virtual void setInt32(const std::string& key, int32_t value) = 0;
    virtual void setInt64(const std::string& key, int64_t value) = 0;
    virtual void setFloat(const std::string& key, float) = 0;
    virtual void setString(const std::string& key, const std::string& value) = 0;
};

} // namespace okui
