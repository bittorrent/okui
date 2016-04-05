#include "onair/okui/config.h"

#if ONAIR_ANDROID

#include <jshackle/BuiltIns.h>
#include <jshackle/JNIContext.h>

#include <gtest/gtest.h>
#include <android/log.h>

#include <thread>

namespace {
    void logFD(FILE* f, int fd, int level) {
        int pair[2];
        setvbuf(f, 0, _IOLBF, 0);
        pipe(pair);
        dup2(pair[1], fd);
        return std::thread([=, fd = pair[0]] {
            ssize_t bytes = 0;
            char buf[512];
            while ((bytes = read(fd, buf, sizeof(buf) - 1)) > 0) {
                while (bytes > 0 && buf[bytes - 1] == '\n') {
                    --bytes;
                }
                buf[bytes] = '\0';
                __android_log_write(level, "okui-tests", buf);
            }
        }).detach();
    }

    JavaVM* g_jvm = nullptr;
    const jint kJniVersion = JNI_VERSION_1_4;
}

extern "C" {

JNIEXPORT void JNICALL Java_tv_watchonair_okui_tests_MainActivity_nativeInit(JNIEnv* env, jobject obj) {
    env->GetJavaVM(&g_jvm);
}

} // extern "C"

// Called by SDLActivity.java
extern "C" int SDL_main(int argc, char* argv[]) {
    __android_log_print(ANDROID_LOG_INFO, "okui-tests", "initializing");

    logFD(stdout, 1, ANDROID_LOG_INFO);
    logFD(stderr, 2, ANDROID_LOG_ERROR);

    ONAIR_ASSERT(g_jvm);
    JavaVM* jvm = g_jvm;

    JNIEnv* env = nullptr;
    if (jvm->GetEnv((void**)&env, kJniVersion) != JNI_OK) {
        ONAIR_LOGF_ERROR("failed to get the environment using GetEnv()");
        return 1;
    }

    auto context = std::make_unique<jshackle::JNIContext>(jvm, kJniVersion);
    jshackle::RegisterBuiltIns(context.get());

    int initArgc = 1;
    const char* initArgv[] = {"okui-tests"};
    testing::InitGoogleTest(&initArgc, const_cast<char**>(initArgv));
    int result = RUN_ALL_TESTS();
    __android_log_print(ANDROID_LOG_INFO, "okui-tests", "test result = %d", result);
    return result;
}


#endif
