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
