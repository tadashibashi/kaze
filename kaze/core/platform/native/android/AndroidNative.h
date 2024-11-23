#pragma once
#include <kaze/core/lib.h>
#include <kaze/core/platform/defines.h>
#include <kaze/core/HttpRequest.h>
#include <kaze/core/HttpResponse.h>

#if KAZE_PLATFORM_ANDROID
class AAsset;

KAZE_NS_BEGIN
namespace android {
    auto openAsset(const char *filename) -> AAsset *; ///< Intended for single read into a buffer
    auto openAssetStream(const char *filename) -> AAsset *; ///< Intended for streaming multiple reads
    auto closeAsset(AAsset *asset) -> void;
    auto getDefaultSampleRate() -> int;
    auto getDefaultFramesPerBuffer() -> int;
    auto getDataDirectory() -> const String &;

    /// Send a synchronous (blocking) http request
    /// \param[in] req   request to send
    /// \returns response object, if status is < 0, a system exception likely occurred
    auto sendHttpRequestSync(
        const HttpRequest &req
    ) -> HttpResponse;

    /// Send an asynchronous http request
    /// \param[in]  req       request to send
    /// \param[in]  callback  callback on success or error, check status or `HttpRequest::ok`
    /// \param[in]  userdata   user context pointer that will be passed to the `callback`
    /// \returns whether request was sent successfully
    auto sendHttpRequest(
        const HttpRequest &req,
        funcptr_t<void(const HttpResponse &res, void *userptr)> callback,
        void *userdata
    ) -> Bool;
}
KAZE_NS_END

#endif
