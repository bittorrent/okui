#pragma once

#include "onair/okui/config.h"

#if ONAIR_ANDROID

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
* This isn't intended to be a full implementation, but can be used to add native support to other implementations.
*/
template <typename Base>
class Android : public Base {
public:
    Android();
    virtual ~Android();

    virtual void openDialog(Window* window,
                            const char* title,
                            const char* message,
                            const std::vector<std::string>& buttons,
                            std::function<void(int)> action = std::function<void(int)>()) override;

    virtual double renderScale() const override;

    virtual std::string distinctId() const override;

    virtual std::string operatingSystem() const override;

    virtual std::string deviceModel() const override;

    virtual bool wifiConnection() const override;

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

protected:    
    virtual std::unique_ptr<ResourceManager> defaultResourceManager() const override;

private:
    JNIEnv* _jniEnv = nullptr;
    jobject _activity = nullptr;

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
        ONAIR_JNI_JAVA_CLASS_METHOD(bool, wifiConnection);
    ONAIR_JNI_JAVA_CLASS_END();

    std::shared_ptr<jni::JNIContext> _jniContext;
    std::unique_ptr<JavaHelper> _javaHelper;
    
    static std::weak_ptr<jni::JNIContext> _sJNIContext;
};

template <typename Base>
std::weak_ptr<jni::JNIContext> Android<Base>::_sJNIContext;

template <typename Base>
inline Android<Base>::Android() {
    auto activity = Base::nativeActivity(&_jniEnv);
    _activity = _jniEnv->NewGlobalRef(activity);
    _jniEnv->DeleteLocalRef(activity);

    JavaVM* jvm = nullptr;
    _jniEnv->GetJavaVM(&jvm);

    auto existingContext = _sJNIContext.lock();
    assert(!existingContext || existingContext->jvm == jvm);

    if (existingContext) {
        _jniContext = existingContext;
    } else {
        _jniContext = std::make_shared<jni::JNIContext>(jvm, _jniEnv->GetVersion());
        JavaHelper::Traits::Register(_jniContext.get(), "tv/watchonair/okui/Helper");
        JavaHelper::JavaOpenDialogCallback::Traits::Register(_jniContext.get(), "tv/watchonair/okui/Helper$OpenDialogCallback");
        _sJNIContext = _jniContext;
    }

    _javaHelper = std::make_unique<JavaHelper>(android::app::Activity{_jniEnv, _activity});
}

template <typename Base>
inline Android<Base>::~Android() {
    if (_javaHelper) {
        _javaHelper.reset();
    }
    if (_activity) {
        _jniEnv->DeleteGlobalRef(_activity);
    }
}

template <typename Base>
inline std::unique_ptr<ResourceManager> Android<Base>::defaultResourceManager() const {
    _jniEnv->PushLocalFrame(10);

    auto c = _jniEnv->GetObjectClass(_activity);
    auto m = _jniEnv->GetMethodID(c, "getAssets", "()Landroid/content/res/AssetManager;");
    auto resourceManager = std::make_unique<AssetResourceManager>(_jniEnv, _jniEnv->CallObjectMethod(_activity, m));

    _jniEnv->PopLocalFrame(nullptr);

    return std::move(resourceManager);
}

template <typename Base>
inline void Android<Base>::openDialog(Window* window,
                         const char* title,
                         const char* message,
                         const std::vector<std::string>& buttons,
                         std::function<void(int)> action) {
    _javaHelper->openDialog(title, message, buttons, new JavaHelper::OpenDialogCallback([=] (int button) {
        taskScheduler()->async([=] {
            action(button);
        });
    }));
}

template <typename Base>
inline double Android<Base>::renderScale() const {
    return _javaHelper->renderScale();
}

template <typename Base>
inline std::string Android<Base>::distinctId() const {
    return _javaHelper->distinctId();
}

template <typename Base>
inline std::string Android<Base>::operatingSystem() const {
    return _javaHelper->operatingSystem();
}

template <typename Base>
inline std::string Android<Base>::deviceModel() const {
    return _javaHelper->deviceModel();
}

template <typename Base>
inline bool Android<Base>::wifiConnection() const {
    return _javaHelper->wifiConnection();
}

template <typename Base>
inline std::shared_ptr<std::string> Android<Base>::AssetResourceManager::load(const char* name) {
    auto ret = std::make_shared<std::string>();
    auto a = AAssetManager_open(_assetManager, name, AASSET_MODE_BUFFER);
    if (!a) {
        return ret;
    }
    auto size = AAsset_getLength64(a);
    ret->resize(size);
    AAsset_read(a, &ret->front(), size);
    AAsset_close(a);
    return ret;
}

} // namespace applications
} // namespace okui
} // namespace onair

#endif
