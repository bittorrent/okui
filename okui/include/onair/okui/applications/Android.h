#pragma once

#include "onair/okui/config.h"

#if ONAIR_ANDROID

#include "onair/okui/Application.h"

#include "onair/jni/JavaClass.h"
#include "onair/jni/NativeClass.h"
#include "onair/jni/JNIContext.h"
#include "onair/jni/android/app/Activity.h"
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

    virtual double renderScale() const override;

    virtual std::string distinctId() const override;

    virtual std::string operatingSystem() const override;
    virtual std::string deviceModel() const override;

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

    ONAIR_JNI_JAVA_CLASS_BEGIN(JavaHelper);
        ONAIR_JNI_JAVA_CLASS_CONSTRUCTOR(JavaHelper, android::app::Activity);

        struct OpenDialogCallback {
            OpenDialogCallback(std::function<void(int)> f) : f{std::move(f)} {}
            void invoke(int button) { f(button); delete this; }
            std::function<void(int)> f;
        };

        ONAIR_JNI_NATIVE_CLASS_BEGIN(JavaOpenDialogCallback, OpenDialogCallback);
            ONAIR_JNI_NATIVE_CLASS_METHOD(void, invoke, int);
        ONAIR_JNI_NATIVE_CLASS_END();

        ONAIR_JNI_JAVA_CLASS_METHOD(void, openDialog, const char*, const char*, const std::vector<std::string>&, OpenDialogCallback*);
        ONAIR_JNI_JAVA_CLASS_METHOD(std::string, distinctId);
        ONAIR_JNI_JAVA_CLASS_METHOD(std::string, operatingSystem);
        ONAIR_JNI_JAVA_CLASS_METHOD(std::string, deviceModel);
        ONAIR_JNI_JAVA_CLASS_METHOD(float, renderScale);
    ONAIR_JNI_JAVA_CLASS_END();

    std::shared_ptr<jni::JNIContext> _jniContext;
    std::unique_ptr<JavaHelper> _javaHelper;
};

} // namespace applications
} // namespace okui
} // namespace onair

#endif
