#pragma once
#include <kaze/core/lib.h>
#include <kaze/core/CStringView.h>

#include "HttpResponse.h"

KAZE_NS_BEGIN

struct HttpRequestCreate;

struct HttpRequest {
    enum Method {
        Get,
        Post,
        Put,
        Delete,
        Patch,
        Head,
        Options,
        Merge,
    };

    static auto create(const HttpRequestCreate &config) -> HttpRequest;

    static inline auto getMethodString(const Method method) -> CStringView
    {
        switch(method)
        {
        case Get: return {"GET", 3};
        case Post: return {"POST", 4};
        case Put: return {"PUT", 3};
        case Delete: return {"DELETE", 6};
        case Patch: return {"PATCH", 5};
        case Head: return {"HEAD", 4};
        case Options: return {"OPTIONS", 7};
        case Merge: return {"MERGE", 5};
        default: return {"", 0};
        }
    }

    /// Send a synchronous (blocking) http request
    /// \returns response object, if status is < 0, a system exception likely occurred
    auto sendSync() -> HttpResponse;

    /// Send an asynchronous (non-blocking) http request
    /// \param[in]  callback   callback on error or success (check the status)
    /// \param[in]  userptr    user data context pointer
    /// \returns whether request was successfully sent.
    auto send(funcptr_t<void (const HttpResponse &res, void *userptr)> callback, void *userptr) -> Bool;

    auto url() const noexcept { return m_url; }
    auto method() const noexcept { return m_method; }
    auto methodString() const noexcept { return getMethodString(m_method); }
    auto mimeType() const noexcept { return m_mimeType; }
    auto body() const noexcept { return m_body; }
    auto headers() const noexcept -> const std::initializer_list<std::pair<CStringView, CStringView>> &
    {
        return m_headers;
    }
private:
    CStringView m_url;
    Method m_method = Get;
    CStringView m_mimeType = {};
    CStringView m_body = {};
    std::initializer_list<std::pair<CStringView, CStringView>> m_headers = {};
};

struct HttpRequestCreate {
    CStringView         url;
    HttpRequest::Method method = HttpRequest::Get;
    CStringView         mimeType = {};
    CStringView         body = {};
    std::initializer_list<std::pair<CStringView, CStringView>> headers = {};
};

KAZE_NS_END
