#pragma once

#include <okui/config.h>

#if SCRAPS_ANDROID

#include <okui/UserPreferencesInterface.h>

#include <jshackle/android/app/Activity.h>

namespace okui {

class AndroidUserPreferences : public UserPreferencesInterface {
public:
    explicit AndroidUserPreferences(android::app::Activity* activity)
        : _sharedPrefs{activity->getPreferences(0)}
        , _editor{_sharedPrefs->edit()}
    {}

    AndroidUserPreferences(const AndroidUserPreferences&) = delete;
    AndroidUserPreferences& operator=(const AndroidUserPreferences&) = delete;

    virtual bool has(const std::string& key) const override { return _sharedPrefs->contains(key); }
    virtual void unset(const std::string& key) override { _editor->remove(key); _editor->apply(); }

    virtual std::string getString(const std::string& key) const override { return _sharedPrefs->getString(key, ""); }
    virtual int32_t getInt32(const std::string& key) const override      { return _sharedPrefs->getInt(key, 0); }
    virtual int64_t getInt64(const std::string& key) const override      { return _sharedPrefs->getLong(key, 0); }
    virtual bool getBool(const std::string& key) const override          { return _sharedPrefs->getBoolean(key, false); }
    virtual float getFloat(const std::string& key) const override        { return _sharedPrefs->getFloat(key, 0.0f); }

    virtual void setBool(const std::string& key, bool value) override                 { _editor->putBoolean(key, value); _editor->apply(); }
    virtual void setInt32(const std::string& key, int32_t value) override             { _editor->putInt(key, value); _editor->apply(); }
    virtual void setInt64(const std::string& key, int64_t value) override             { _editor->putLong(key, value); _editor->apply(); }
    virtual void setFloat(const std::string& key, float value) override               { _editor->putFloat(key, value); _editor->apply(); }
    virtual void setString(const std::string& key, const std::string& value) override { _editor->putString(key, value); _editor->apply(); }

private:
    android::content::SharedPreferences* _sharedPrefs;
    android::content::SharedPreferences::Editor* _editor;
};

} // namespace okui

#endif
