#pragma once
#include <kaze/core/lib.h>
#include <kaze/core/HttpRequest.h>
#include <kaze/core/HttpResponse.h>

KAZE_NS_BEGIN

namespace http {
    /// Send a synchronous (blocking) http request
    /// \param[in] req   request to send
    /// \returns response object, if status is < 0, a system exception likely occurred
    auto sendHttpRequestSync(
        const HttpRequest &req
    ) -> HttpResponse;

    /// Send an asynchronous http request
    /// \param[in]  req       request to send
    /// \param[in]  callback  callback on success or error, check status or `HttpRequest::ok`
    /// \param[in]  userdata  user context pointer that will be passed to the `callback`
    /// \returns whether the request was sent successfully without errors
    auto sendHttpRequest(
        const HttpRequest &req,
        funcptr_t<void(const HttpResponse &res, void *userdata)> callback,
        void *userdata
    ) -> Bool;
}

KAZE_NS_END
