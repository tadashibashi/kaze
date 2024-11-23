#include "HttpRequest.h"
#include <kaze/core/platform/http/http.h>

KAZE_NS_BEGIN

auto HttpRequest::create(const HttpRequestCreate &config) -> HttpRequest
{
    HttpRequest req;
    req.m_body = config.body;
    req.m_headers = config.headers;
    req.m_method = config.method;
    req.m_mimeType = config.mimeType;
    req.m_url = config.url;

    return req;
}

auto HttpRequest::sendSync() -> HttpResponse
{
    return http::sendHttpRequestSync(*this);
}

auto HttpRequest::send(funcptr_t<void (const HttpResponse &res, void *userptr)> callback, void *userptr) -> Bool
{
    return http::sendHttpRequest(*this, callback, userptr);
}

KAZE_NS_END
