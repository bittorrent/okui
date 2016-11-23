#pragma once

#include <okui/config.h>

#if SCRAPS_ANDROID

#include <okui/applications/AndroidUserPreferences.h>

#include <jshackle/JavaClass.h>
#include <jshackle/NativeClass.h>
#include <jshackle/JNIContext.h>
#include <jshackle/android/app/Activity.h>

#include <stdts/string_view.h>

#include <android/asset_manager_jni.h>

namespace okui::applications {

JSHACKLE_JAVA_CLASS_BEGIN(AndroidJavaHelper);
    JSHACKLE_JAVA_CLASS_CONSTRUCTOR(AndroidJavaHelper, android::app::Activity);

    struct OpenDialogCallback {
        OpenDialogCallback(std::function<void(int)> f) : f{std::move(f)} {}
        void invoke(int button) { f(button); delete this; }
        std::function<void(int)> f;
    };

    JSHACKLE_NATIVE_CLASS_BEGIN(JavaOpenDialogCallback, OpenDialogCallback);
        JSHACKLE_NATIVE_CLASS_METHOD(void, invoke, int);
    JSHACKLE_NATIVE_CLASS_END();

    JSHACKLE_JAVA_CLASS_METHOD(bool, openURL, const char*);
    JSHACKLE_JAVA_CLASS_METHOD(void, openDialog, const char*, const char*, const std::vector<std::string>&, OpenDialogCallback*);
    JSHACKLE_JAVA_CLASS_METHOD(std::string, distinctId);
    JSHACKLE_JAVA_CLASS_METHOD(std::string, operatingSystem);
    JSHACKLE_JAVA_CLASS_METHOD(std::string, deviceModel);
    JSHACKLE_JAVA_CLASS_METHOD(float, renderScale);
    JSHACKLE_JAVA_CLASS_METHOD(bool, hasNetworkConnection);
    JSHACKLE_JAVA_CLASS_METHOD(bool, isMobileConnection);
JSHACKLE_JAVA_CLASS_END();

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

    android::app::Activity* activity() { return _javaActivity.get(); }

    virtual bool openURL(const char* url) override;

    virtual void openDialog(Window* window,
                            const char* title,
                            const char* message,
                            const std::vector<DialogButton>& buttons,
                            std::function<void(int)> action = std::function<void(int)>()) override;

    virtual double renderScale() const override;

    virtual std::string distinctId() const override;

    virtual std::string operatingSystem() const override;

    virtual std::string deviceModel() const override;

    virtual UserPreferencesInterface* getUserPreferences() override { return _userPreferences.get(); }

    virtual bool hasNetworkConnection() const override;

    virtual bool isMobileConnection() const override;

    virtual bool canHandleCommand(Command command) override {
        return command == kCommandBack || Base::canHandleCommand(command);
    }

    virtual void handleCommand(Command command, CommandContext context) override {
        if (command == kCommandBack) {
            return this->quit();
        }
        Base::handleCommand(command, context);
    }

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

        virtual std::shared_ptr<std::string> load(stdts::string_view name) override;

    private:
        JNIEnv* _env;
        jobject _assetManagerReference;
        AAssetManager* _assetManager;
    };

private:
    JNIEnv*                                    _jniEnv = nullptr;
    jobject                                    _activity = nullptr;
    std::unique_ptr<android::app::Activity>    _javaActivity;
    std::shared_ptr<jshackle::JNIContext>      _jniContext;
    std::unique_ptr<AndroidUserPreferences>    _userPreferences;
    std::unique_ptr<AndroidJavaHelper>         _javaHelper;
    std::unique_ptr<ResourceManager>           _resourceManager;

    static std::weak_ptr<jshackle::JNIContext> _sJNIContext;
};

template <typename Base>
std::weak_ptr<jshackle::JNIContext> Android<Base>::_sJNIContext;

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
        _jniContext = std::make_shared<jshackle::JNIContext>(jvm, _jniEnv->GetVersion());
        AndroidJavaHelper::Traits::Register(_jniContext.get(), "tv/watchonair/okui/Helper");
        AndroidJavaHelper::JavaOpenDialogCallback::Traits::Register(_jniContext.get(), "tv/watchonair/okui/Helper$OpenDialogCallback");
        _sJNIContext = _jniContext;
    }

    _javaActivity = std::make_unique<android::app::Activity>(_jniEnv, _activity);
    _javaHelper = std::make_unique<AndroidJavaHelper>(*_javaActivity);
    _userPreferences = std::make_unique<AndroidUserPreferences>(_javaActivity.get());

    _jniEnv->PushLocalFrame(10);

    auto c = _jniEnv->GetObjectClass(_activity);
    auto m = _jniEnv->GetMethodID(c, "getAssets", "()Landroid/content/res/AssetManager;");
    _resourceManager = std::make_unique<AssetResourceManager>(_jniEnv, _jniEnv->CallObjectMethod(_activity, m));
    Base::setResourceManager(_resourceManager.get());

    _jniEnv->PopLocalFrame(nullptr);
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
inline bool Android<Base>::openURL(const char* url) {
    return _javaHelper->openURL(url);
}

template <typename Base>
inline void Android<Base>::openDialog(Window* window,
                         const char* title,
                         const char* message,
                         const std::vector<DialogButton>& buttons,
                         std::function<void(int)> action)
{
    // TODO: support button styles?
    std::vector<std::string> buttonLabels;
    for (auto& button : buttons) {
        buttonLabels.emplace_back(button.label);
    }

    _javaHelper->openDialog(title, message, buttonLabels, new AndroidJavaHelper::OpenDialogCallback([=] (int button) {
        this->taskScheduler()->async([=] {
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
inline bool Android<Base>::hasNetworkConnection() const {
    return _javaHelper->hasNetworkConnection();
}

template <typename Base>
inline bool Android<Base>::isMobileConnection() const {
    return _javaHelper->isMobileConnection();
}

template <typename Base>
inline std::shared_ptr<std::string> Android<Base>::AssetResourceManager::load(stdts::string_view name) {
    auto ret = std::make_shared<std::string>();
    auto a = AAssetManager_open(_assetManager, std::string(name).c_str(), AASSET_MODE_BUFFER);
    if (!a) {
        return ret;
    }
    auto size = AAsset_getLength64(a);
    ret->resize(size);
    AAsset_read(a, &ret->front(), size);
    AAsset_close(a);
    return ret;
}

} // namespace okui::applications

#endif
