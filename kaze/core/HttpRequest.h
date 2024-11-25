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

    /// \returns string form of an `HttpRequest::Method`
    /// \note returns an empty string if not a recognized enum value.
    static auto methodToString(const Method method) -> CStringView;

    /// Send a synchronous (blocking) http request
    /// \returns response object, if status is < 0, a system exception likely occurred
    auto sendSync() -> HttpResponse;

    /// Send an asynchronous (non-blocking) http request
    /// \param[in]  callback   callback on error or success (check the status)
    /// \param[in]  userptr    user data context pointer
    /// \returns whether request was successfully sent.
    auto send(funcptr_t<void (const HttpResponse &res, void *userptr)> callback, void *userptr) -> Bool;

    /// \returns the request URL
    auto url() const noexcept -> const String & { return m_url; }

    /// \returns the request method enum
    auto method() const noexcept { return m_method; }

    /// \returns the reuqest method as a string
    auto methodString() const noexcept { return methodToString(m_method); }

    /// \returns the mime type as a string, may be empty
    auto mimeType() const noexcept -> const String & { return m_mimeType; }

    /// \returns the body
    auto body() const noexcept -> const String & { return m_body; }

    /// Swap out body. Caution: mutates this `HttpRequest` instance, even if const.
    /// This is necessary for very large uploads.
    /// \param[in]  other  string to swap with the internal one
    auto swapBody(String &other) const noexcept -> void { m_body.swap(other); }

    auto headers() const noexcept -> const List<std::pair<String, String>> &
    {
        return m_headers;
    }
private:
    String m_url;
    Method m_method = Get;
    String m_mimeType = {};
    mutable String m_body = {}; // made mutable to swap out, even if const
    List<std::pair<String, String>> m_headers = {};
};

struct HttpRequestCreate {
    CStringView         url;
    HttpRequest::Method method = HttpRequest::Get;
    CStringView         mimeType = {};
    CStringView         body = {};
    List<std::pair<CStringView, CStringView>> headers = { };
};

KAZE_NS_END
