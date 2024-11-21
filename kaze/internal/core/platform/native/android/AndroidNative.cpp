#include "AndroidNative.h"

#if KAZE_PLATFORM_ANDROID
#include <kaze/internal/core/debug.h>

#include <jni.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>

static jobject g_activity;
static AAssetManager *g_assets;
static int defaultSampleRate = 48000;
static int defaultFramesPerBuffer = 512;

static KAZE_NS::String dataDirectory;

extern "C" JNIEXPORT void JNICALL
Java_com_kaze_app_Kaze_nativeInit(JNIEnv *env, jclass clazz, jobject activity)
{
    g_activity = activity;
}

extern "C" JNIEXPORT void JNICALL
Java_com_kaze_app_Kaze_nativeClose(JNIEnv *env, jclass clazz)
{
    g_activity = nullptr;
    g_assets = nullptr;
}

extern "C" JNIEXPORT void JNICALL
Java_com_kaze_app_Kaze_provideAssetManager(JNIEnv *env, jclass clazz, jobject assetMgr)
{
    g_assets = AAssetManager_fromJava(env, assetMgr);
}

extern "C" JNIEXPORT void JNICALL
Java_com_kaze_app_Kaze_provideAudioDefaults(JNIEnv *env, jclass clazz, jint sampleRate, jint framesPerBuffer)
{
    defaultSampleRate = static_cast<int>(sampleRate);
    defaultFramesPerBuffer = static_cast<int>(framesPerBuffer);
}

extern "C" JNIEXPORT void JNICALL
Java_com_kaze_app_Kaze_provideDataDirectory(JNIEnv *env, jclass clazz, jstring path)
{
    char dataFilePath[4096];
    std::strncpy(dataFilePath, env->GetStringUTFChars(path, nullptr), 4096);
    dataDirectory = dataFilePath;
}

KAZE_NS_BEGIN

namespace android {
    static auto openAssetImpl(const char *filename, int mode) -> AAsset *
    {
        if (!g_assets)
        {
            KAZE_PUSH_ERR(Error::BE_RuntimeErr, "The Kaze Android Native object was not initialized. "
                                          "Please make sure to call com.kaze.Kaze.init(this) "
                                          "in an Activity.onCreate");
            return nullptr;
        }

        return AAssetManager_open(g_assets, filename, mode);
    }

    auto openAsset(const char *filename) -> AAsset *
    {
        return openAssetImpl(filename, AASSET_MODE_BUFFER);
    }

    auto openAssetStream(const char *filename) -> AAsset *
    {
        return openAssetImpl(filename, AASSET_MODE_STREAMING);
    }

    auto closeAsset(AAsset *asset) -> void
    {
        AAsset_close(asset);
    }

    auto getDefaultSampleRate() -> int
    {
        return defaultSampleRate;
    }

    auto getDefaultFramesPerBuffer() -> int
    {
        return defaultFramesPerBuffer;
    }

    auto getDataDirectory() -> const String &
    {
        return dataDirectory;
    }
}

KAZE_NS_END

#endif
