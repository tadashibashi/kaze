#include "HttpRequest.h"
#include <kaze/core/platform/http/http.h>
#include <kaze/core/str.h>
KAZE_NS_BEGIN

auto HttpRequest::create(const HttpRequestCreate &config) -> HttpRequest
{
    HttpRequest req;
    req.m_body = str::toString(config.body);
    for (auto &[header, value] : config.headers)
        req.m_headers.emplace_back(std::make_pair(str::toString(header), str::toString(value)));
    req.m_method = config.method;
    req.m_mimeType = str::toString(config.mimeType);
    req.m_url = str::toString(config.url);

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
