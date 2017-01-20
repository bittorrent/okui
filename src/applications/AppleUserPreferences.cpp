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
#include <okui/applications/AppleUserPreferences.h>

#if SCRAPS_APPLE

namespace okui::applications {

AppleUserPreferences::AppleUserPreferences() : _userDefaults{[NSUserDefaults standardUserDefaults]} {}

AppleUserPreferences::AppleUserPreferences(const std::string& suiteName) {
    if (suiteName.empty()) {
        _userDefaults = [NSUserDefaults standardUserDefaults];
    } else {
        NSString* nsSuiteName = [NSString stringWithUTF8String:suiteName.c_str()];
        _userDefaults = [[NSUserDefaults alloc] initWithSuiteName:nsSuiteName];
    }
}

bool AppleUserPreferences::has(const std::string& key) const {
    NSString* nsKey = [NSString stringWithUTF8String:key.c_str()];
    id nsValue = [_userDefaults objectForKey:nsKey];
    return nsValue != nil;
}

void AppleUserPreferences::unset(const std::string& key) {
    NSString* nsKey = [NSString stringWithUTF8String:key.c_str()];
    [_userDefaults removeObjectForKey:nsKey];
}

bool AppleUserPreferences::getBool(const std::string& key) const {
    NSString* nsKey = [NSString stringWithUTF8String:key.c_str()];
    return [_userDefaults boolForKey:nsKey];
}

int32_t AppleUserPreferences::getInt32(const std::string& key) const {
    NSString* nsKey = [NSString stringWithUTF8String:key.c_str()];
    return [_userDefaults integerForKey:nsKey];
}

int64_t AppleUserPreferences::getInt64(const std::string& key) const {
    NSString* nsKey = [NSString stringWithUTF8String:key.c_str()];
    return [_userDefaults integerForKey:nsKey];
}

float AppleUserPreferences::getFloat(const std::string& key) const {
    NSString* nsKey = [NSString stringWithUTF8String:key.c_str()];
    return static_cast<float>([_userDefaults doubleForKey:nsKey]);
}

std::string AppleUserPreferences::getString(const std::string& key) const {
    NSString* nsKey = [NSString stringWithUTF8String:key.c_str()];
    NSString* nsValue = [_userDefaults objectForKey:nsKey];
    return nsValue ? [nsValue UTF8String] : "";
}

void AppleUserPreferences::setBool(const std::string& key, bool value) {
    NSString* nsKey = [NSString stringWithUTF8String:key.c_str()];
    [_userDefaults setBool:value forKey:nsKey];
}

void AppleUserPreferences::setInt32(const std::string& key, int32_t value) {
    NSString* nsKey = [NSString stringWithUTF8String:key.c_str()];
    [_userDefaults setInteger:value forKey:nsKey];
}

void AppleUserPreferences::setInt64(const std::string& key, int64_t value) {
    NSString* nsKey = [NSString stringWithUTF8String:key.c_str()];
    [_userDefaults setInteger:value forKey:nsKey];
}

void AppleUserPreferences::setFloat(const std::string& key, float value) {
    NSString* nsKey = [NSString stringWithUTF8String:key.c_str()];
    [_userDefaults setDouble:value forKey:nsKey];
}

void AppleUserPreferences::setString(const std::string& key, const std::string& value) {
    NSString* nsKey = [NSString stringWithUTF8String:key.c_str()];
    NSString* nsValue = [NSString stringWithUTF8String:value.c_str()];
    [_userDefaults setObject:nsValue forKey:nsKey];
}

} // namespace okui::applications

#endif
