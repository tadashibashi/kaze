#include "http.h"
#include <kaze/core/platform/native/android/AndroidNative.h>

KAZE_NS_BEGIN

auto http::sendHttpRequestSync(const HttpRequest &req) -> HttpResponse
{
    return android::sendHttpRequestSync(req);
}

auto http::sendHttpRequest(
        const HttpRequest &req,
        funcptr_t<void(const HttpResponse &res, void *userdata)> callback,
        void *userdata
) -> Bool
{
    return android::sendHttpRequest(req, callback, userdata);
}

auto http::getLocalHost() -> Cstring
{
    return android::isEmulator() ? "10.0.2.2" : "127.0.0.1";
}

KAZE_NS_END
