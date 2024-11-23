#include "AndroidNative.h"

#if KAZE_PLATFORM_ANDROID
#include <kaze/core/debug.h>
#include <kaze/core/HttpRequest.h>

#include <jni.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <SDL3/SDL_platform.h>

static jobject g_activity;
static AAssetManager *g_assets;
static int defaultSampleRate = 48000;
static int defaultFramesPerBuffer = 512;

static KAZE_NS::String dataDirectory;


// Global variable to store JavaVM pointer
static JavaVM *g_jvm = NULL;

extern "C" JNIEXPORT jint JNICALL
JNI_OnLoad(JavaVM* vm, void* reserved)
{
    // Save the JavaVM pointer
    g_jvm = vm;

    // Optionally perform initialization tasks
    JNIEnv* env;
    if (vm->GetEnv((void**)&env, JNI_VERSION_1_6) != JNI_OK) {
        return JNI_ERR; // JNI version not supported
    }

    // Return the supported JNI version
    return JNI_VERSION_1_6;
}

static auto getEnv()  -> JNIEnv *
{
    if ( !g_jvm )
    {
        KAZE_PUSH_ERR(KAZE_NS::Error::RuntimeErr, "Failed to get JavaVM, native interface was not initialized");
        return nullptr;
    }

    JNIEnv* env;
    if (g_jvm->GetEnv((void**)&env, JNI_VERSION_1_6) != JNI_OK) {
        if (g_jvm->AttachCurrentThread(&env, nullptr) != JNI_OK)
        {
            KAZE_PUSH_ERR(KAZE_NS::Error::RuntimeErr, "Failed to attach thread from JVM");
            return nullptr;
        }
    }

    return env;
}

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
    const char *cppPath =  env->GetStringUTFChars(path, nullptr);

    dataDirectory = cppPath;

    env->ReleaseStringUTFChars(path, cppPath);
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

    static auto toJavaString(JNIEnv *env, CStringView str) -> jstring
    {
        return env->NewStringUTF(str.data() ? str.data() : "");
    }

    static auto headersToArrayList(
        JNIEnv *env,
        const std::initializer_list< std::pair<CStringView, CStringView> > &headers
        ) -> jobject
    {
        jclass arrayListClass = env->FindClass("java/util/ArrayList");
        if ( !arrayListClass )
        {
            KAZE_PUSH_ERR(Error::RuntimeErr, "Failed to get Java ArrayList class");
            return Null;
        }

        jmethodID arrayListCtor = env->GetMethodID(arrayListClass, "<init>", "()V");
        if ( !arrayListCtor )
        {
            KAZE_PUSH_ERR(Error::RuntimeErr, "Failed to get Java ArrayList constructor");
            return Null;
        }

        jmethodID addMethod = env->GetMethodID(arrayListClass, "add", "(Ljava/lang/Object;)Z");
        if ( !addMethod )
        {
            KAZE_PUSH_ERR(Error::RuntimeErr, "Failed to get Java ArrayList `add` method");
            return Null;
        }

        jobject arrayList = env->NewObject(arrayListClass, arrayListCtor);
        if ( !arrayList )
        {
            KAZE_PUSH_ERR(Error::RuntimeErr, "Failed to instantiate a Java ArrayList jobject");
            return Null;
        }

        for (const auto &[header, value] : headers)
        {
            const auto jheader = toJavaString(env, header.data());
            const auto jvalue  = toJavaString(env, value.data());

            env->CallBooleanMethod(arrayList, addMethod, jheader);
            env->CallBooleanMethod(arrayList, addMethod, jvalue);

            env->DeleteLocalRef(jheader);
            env->DeleteLocalRef(jvalue);
        }

        return arrayList;
    }

    static auto arrayListToHeaders(JNIEnv *env, jobject jheaders) -> List<String>
    {
        jclass JArrayList = env->FindClass("java/util/ArrayList");
        if ( !JArrayList )
        {
            KAZE_PUSH_ERR(Error::RuntimeErr, "Failed to get Java ArrayList class");
            return {};
        }

        jmethodID getMethod = env->GetMethodID(JArrayList, "get", "(I)Ljava/lang/Object;");
        if ( !getMethod )
        {
            KAZE_PUSH_ERR(Error::RuntimeErr, "Failed to get Java ArrayList.get method id");
            return {};
        }

       jmethodID sizeMethod = env->GetMethodID(JArrayList, "size", "()I");
        if ( !getMethod )
        {
            KAZE_PUSH_ERR(Error::RuntimeErr, "Failed to get Java ArrayList.get method id");
            return {};
        }

        auto size = static_cast<Int>(env->CallIntMethod(jheaders, sizeMethod));

        List<String> res;
        res.reserve(size);

        for (Int i = 0; i < size; ++i)
        {
            auto str = static_cast<jstring>(env->CallObjectMethod(jheaders, getMethod, static_cast<jint>(i)));
            if (str)
            {
                res.emplace_back(env->GetStringUTFChars(str, Null));
                env->DeleteLocalRef(str);
            }
        }

        env->DeleteLocalRef(JArrayList);
        return res;
    }

    static auto toHttpResponse(JNIEnv *env, jobject jres) -> HttpResponse
    {
        if ( !jres ) return {};

        jclass JHttpRequest = env->FindClass("com/kaze/app/HttpResponse");
        if ( !JHttpRequest )
        {
            KAZE_PUSH_ERR(Error::RuntimeErr, "Failed to find Java class com/kaze/app/HttpResponse:");
            env->ExceptionDescribe();
            return {};
        }


        jfieldID statusField = env->GetFieldID(JHttpRequest, "status", "I");
        if ( !statusField )
        {
            KAZE_PUSH_ERR(Error::RuntimeErr, "Failed to find field `com/kaze/app/HttpResponse.status`");
            env->ExceptionDescribe();
            return {};
        }

        jfieldID headersField = env->GetFieldID(JHttpRequest, "headers", "java/util/ArrayList");
        if ( !headersField )
        {
            KAZE_PUSH_ERR(Error::RuntimeErr, "Failed to find field `com/kaze/app/HttpResponse.headers`");
            env->ExceptionDescribe();
            return {};
        }

        jfieldID cookiesField = env->GetFieldID(JHttpRequest, "cookies", "java/util/ArrayList");
        if ( !cookiesField )
        {
            KAZE_PUSH_ERR(Error::RuntimeErr, "Failed to find field `com/kaze/app/HttpResponse.cookies`");
            env->ExceptionDescribe();
            return {};
        }

        jint status = env->GetIntField(jres, statusField);

        HttpResponse res{};
        res.status = static_cast<Int>(status);
        if (res.ok())
        {
            // success - get the body
            jfieldID bodyField = env->GetFieldID(JHttpRequest, "body", "Ljava/lang/String;");
            if ( !bodyField )
            {
                KAZE_PUSH_ERR(Error::RuntimeErr, "Failed to find field `com/kaze/app/HttpResponse.body`");
                return {};
            }

            auto body = static_cast<jstring>(env->GetObjectField(jres, bodyField));
            if (body)
                res.body = env->GetStringUTFChars(body, nullptr);
        }
        else
        {
            // error - get the error
            jfieldID errorField = env->GetFieldID(JHttpRequest, "error", "Ljava/lang/String;");
            if ( !errorField )
            {
                KAZE_PUSH_ERR(Error::RuntimeErr, "Failed to find field `com/kaze/app/HttpResponse.error`");
                return {};
            }

            auto error = static_cast<jstring>(env->GetObjectField(jres, errorField));
            if (error)
                res.error = env->GetStringUTFChars(error, nullptr);
        }

        auto jheaders = env->GetObjectField(jres, headersField);
        auto headers = arrayListToHeaders(env, jheaders);
        env->DeleteLocalRef(jheaders);

        auto jcookies = env->GetObjectField(jres, cookiesField);
        auto cookies = arrayListToHeaders(env, jcookies);
        env->DeleteLocalRef(jcookies);

        for (Int i = 0, size = static_cast<Int>(headers.size());
            i < size - 1;
            i += 2)
        {
            res.headers[headers[i]] = headers[i + 1];
        }

        res.cookies = std::move(cookies);
        return res;
    }

    auto sendHttpRequestSync(
        const HttpRequest &req
    ) -> HttpResponse
    {
        auto env = getEnv();
        if ( !env )
        {
            return {};
        }

        try {
            auto url = toJavaString(env, req.url());
            auto method = toJavaString(env, req.methodString());
            auto mimeType = toJavaString(env, req.mimeType());
            auto headers = headersToArrayList(env, req.headers());

            if ( !url || !method || !mimeType || !headers )
            {
                KAZE_PUSH_ERR(Error::RuntimeErr, "Failed to convert/transfer params to Java JNI types:");
                env->ExceptionDescribe();
                return {};
            }

            jclass kazeClass = env->FindClass("com/kaze/app/Kaze");
            if (kazeClass == Null) {
                KAZE_PUSH_ERR(Error::RuntimeErr, "Failed to find Java class:");
                env->ExceptionDescribe();
                return {}; // Class not found
            }

            jmethodID sendHTTPRequestMethod = env->GetStaticMethodID(
                kazeClass,
                "sendHTTPRequestSync",
                "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/util/ArrayList;)Lcom/kaze/app/HttpResponse;"
            );

            if (sendHTTPRequestMethod == Null)
            {
                KAZE_PUSH_ERR(Error::RuntimeErr, "Failed to get static Java method:");
                env->ExceptionDescribe();
                return {};
            }

            const auto jres = env->CallStaticObjectMethod(
                kazeClass,
                sendHTTPRequestMethod,
                url,
                method,
                mimeType,
                headers);

            return toHttpResponse(env, jres);
        }
        catch(const std::exception &e)
        {
            KAZE_PUSH_ERR(Error::StdExcept,
                "Exception occurred during sendHttpRequestSync: {}",
                e.what());
            return {
                .status = -1,
                .error = e.what()
            };
        }
        catch(...)
        {
            KAZE_PUSH_ERR(Error::Unknown,
                "Unknown exception occurred during sendHttpRequestSync");
            return {};
        }
    }

    auto sendHttpRequest(
        const HttpRequest &req,
        funcptr_t<void(const HttpResponse &res, void *userdata)> callback,
        void *userdata
    ) -> Bool
    {
        auto env = getEnv();

        try {
            auto url = toJavaString(env, req.url());
            auto method = toJavaString(env, req.methodString());
            auto mimeType = toJavaString(env, req.mimeType());
            auto headers = headersToArrayList(env, req.headers());

            if ( !url || !method || !mimeType || !headers )
            {
                KAZE_PUSH_ERR(Error::RuntimeErr,
                              "sendHttpRequest failed to convert parameters to Java types:");
                env->ExceptionDescribe();
                return False;
            }

            jclass kazeClass = env->FindClass("com/kaze/app/Kaze");
            if (kazeClass == Null)
            {
                KAZE_PUSH_ERR(Error::RuntimeErr,
                              "sendHttpRequest failed to find Kaze java class:");
                env->ExceptionDescribe();
                return False;
            }

            jmethodID sendHTTPRequestMethod = env->GetStaticMethodID(
                kazeClass,
                "sendHTTPRequest",
                "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/util/ArrayList;JJ)V"
            );

            if (sendHTTPRequestMethod == Null) {
                KAZE_PUSH_ERR(Error::RuntimeErr,
                              "sendHttpRequest failed to get sendHTTPRequest Java method:");
                env->ExceptionDescribe();
                return False;
            }

            env->CallStaticVoidMethod(
                kazeClass,
                sendHTTPRequestMethod,
                url,
                method,
                mimeType,
                headers,
                static_cast<jlong>(reinterpret_cast<uintptr_t>(callback)),
                static_cast<jlong>(reinterpret_cast<uintptr_t>(userdata)));

            if (env->ExceptionOccurred())
            {
                KAZE_PUSH_ERR(Error::RuntimeErr,
                              "sendHttpRequest: an exception occurred when calling "
                              "the underlying Java function:");
                env->ExceptionDescribe();
                return False;
            }

            return True;
        }
        catch(const std::exception &e)
        {
            KAZE_PUSH_ERR(Error::StdExcept, "Exception was thrown during sendHttpRequest: {}",
                          e.what());
            return False;
        }
        catch(...)
        {
            KAZE_PUSH_ERR(Error::Unknown, "Unknown error was thrown during sendHttpRequest");
            return False;
        }
    }
}

KAZE_NS_END

extern "C" JNIEXPORT void JNICALL
Java_com_kaze_app_Kaze_doHttpCallback(JNIEnv *env, jclass clazz, jint status, jstring body, jobject jheaders,
    jobject jcookies, jlong callback, jlong userptr)
{
    using namespace KAZE_NS;

    auto cppCallback = reinterpret_cast< funcptr_t<void(const HttpResponse &, void *)> >(callback);
    auto cppUserptr = reinterpret_cast<void *>(userptr);
    auto cppBody = body ? env->GetStringUTFChars(body, nullptr) : "";

    if ( !cppBody )
    {
        KAZE_PUSH_ERR(Error::RuntimeErr, "Java environment failed to parse response body into UTF chars");
        return;
    }

    KAZE_ASSERT(cppCallback != nullptr);

    HttpResponse res;
    res.status = static_cast<Int>(status);
    if (res.ok())
        res.body = cppBody;
    else
        res.error = cppBody;

    auto headers = android::arrayListToHeaders(env, jheaders);
    auto cookies = android::arrayListToHeaders(env, jcookies);

    for (Int i = 0, size = static_cast<Int>(headers.size());
        i < size - 1;
        i += 2)
    {
        res.headers[headers[i]] = headers[i + 1];
    }

    res.cookies = std::move(cookies);

    cppCallback(res, cppUserptr);
}

#endif
