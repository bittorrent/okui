#pragma once

#include "onair/okui/config.h"

#if ONAIR_ANDROID

#include "onair/okui/Application.h"

#include <android/asset_manager_jni.h>

namespace onair {
namespace okui {
namespace applications {

/**
* Provides some native Platform overrides for Android.
*
* This isn't intended to be a full implementation, but can be used as a base for
* other implementations.
*/
class Android : public Application {
public:
    using Application::Application;
    virtual ~Android();
    
    void setActivity(JNIEnv* env, jobject activity);

    virtual void initialize() override;

    virtual void openDialog(Window* window,
                            const char* title,
                            const char* message,
                            const std::vector<std::string>& buttons,
                            std::function<void(int)> action = std::function<void(int)>()) override;

    class AssetResourceManager : public ResourceManager {
    public:
        AssetResourceManager(JNIEnv* env, jobject assetManager)
            : _env{env}
            , _assetManagerReference{env->NewGlobalRef(assetManager)}
            , _assetManager{AAssetManager_fromJava(env, assetManager)}
        {}

        ~AssetResourceManager() {
            _env->DeleteGlobalRef(_assetManagerReference);
        }
        
        virtual std::shared_ptr<std::string> load(const char* name) override;

    private:
        JNIEnv* _env;
        jobject _assetManagerReference;
        AAssetManager* _assetManager;
    };

private:
    JNIEnv* _jniEnv = nullptr;
    jobject _activity = nullptr;
    
    std::unique_ptr<ResourceManager> _resourceManager;
};

} // namespace applications
} // namespace okui
} // namespace onair

#endif
