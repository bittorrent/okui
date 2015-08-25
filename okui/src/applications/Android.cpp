#include "onair/okui/applications/Android.h"

#if ONAIR_ANDROID

namespace onair {
namespace okui {
namespace applications {

Android::~Android() {
    if (_activity) {
        _jniEnv->DeleteGlobalRef(_activity);
    }
}

void Android::setActivity(JNIEnv* env, jobject activity) {
    _jniEnv = env;
    _activity = env->NewGlobalRef(activity);
}

void Android::initialize() {
    if (!resourceManager() && _activity) {
        ONAIR_LOG_DEBUG("using resources from asset manager");

        _jniEnv->PushLocalFrame(10);

        auto c = _jniEnv->GetObjectClass(_activity);
        auto m = _jniEnv->GetMethodID(c, "getAssets", "()Landroid/content/res/AssetManager;");
        _resourceManager.reset(new AssetResourceManager(_jniEnv, _jniEnv->CallObjectMethod(_activity, m)));
        setResourceManager(_resourceManager.get());

        _jniEnv->PopLocalFrame(nullptr);
    }
    
    Application::initialize();
}

void Android::openDialog(Window* window,
                         const char* title,
                         const char* message,
                         const std::vector<std::string>& buttons,
                         std::function<void(int)> action) {
    // TODO: implement
    ONAIR_ASSERT(false);
}

std::shared_ptr<std::string> Android::AssetResourceManager::load(const char* name) {
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
