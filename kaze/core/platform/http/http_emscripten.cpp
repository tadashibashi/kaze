#include "http.h"
#include <kaze/core/memory.h>
#include <kaze/core/str.h>

#include <emscripten/fetch.h>

KAZE_NS_BEGIN

static auto emplaceHeaders(HttpResponse *res, emscripten_fetch_t *fetch) -> void
{
    auto length = emscripten_fetch_get_response_headers_length(fetch);
    if (length == 0)
        return;
    auto headersBuffer = static_cast<char *>(memory::alloc(length + 1));

    emscripten_fetch_get_response_headers(fetch, headersBuffer, length + 1);
    headersBuffer[length] = '\0';

    const auto headers = emscripten_fetch_unpack_response_headers(headersBuffer);

    for (Int i = 0; headers[i] != Null; i += 2)
    {
        const String header = str::toLower(headers[i]);
        auto value = headers[i + 1];
        if (!value) break; // in case of odd number for kvp

        auto trimmedView = str::trim(value, true, true);
        auto trimmedValue = String(trimmedView.data(), trimmedView.length());

        if (header == "set-cookie")
        {
            res->cookies.emplace_back(std::move(trimmedValue));
        }
        else
        {
            res->headers[header] = std::move(trimmedValue);
        }
    }

    emscripten_fetch_free_unpacked_response_headers(headers);
    memory::free(headersBuffer);
}

auto http::sendHttpRequestSync(const HttpRequest &req) -> HttpResponse
{
    emscripten_fetch_attr_t attr;
    emscripten_fetch_attr_init(&attr);

    attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY | EMSCRIPTEN_FETCH_SYNCHRONOUS | EMSCRIPTEN_FETCH_STREAM_DATA;

    auto method = HttpRequest::getMethodString(req.method());
    if (method.empty())
        method = "GET";
    memory::copy(attr.requestMethod, method.data(), method.length());

    List<const char *> headers;
    if (req.headers().size() > 0)
    {
        headers.reserve(req.headers().size() * 2 + 1);
        for (const auto &[header, value] : req.headers())
        {
            headers.emplace_back(header.data());
            headers.emplace_back(value.data());
        }
        headers.emplace_back(Null);

        attr.requestHeaders = headers.data();
    }

    if ( !req.mimeType().empty() )
    {
        attr.overriddenMimeType = req.mimeType().data();
    }

    if ( !req.body().empty() )
    {
        attr.requestData = req.body().data();
        attr.requestDataSize = req.body().size();
    }

    HttpResponse res{};
    attr.userData = &res;
    attr.onerror = [](emscripten_fetch_t *fetch) {
        auto res = static_cast<HttpResponse *>(fetch->userData);
        res->status = static_cast<Int>(fetch->status);
        res->error = fetch->statusText;
        emplaceHeaders(res, fetch);
        emscripten_fetch_close(fetch);
    };

    attr.onsuccess = [](emscripten_fetch_t *fetch) {
        auto res = static_cast<HttpResponse *>(fetch->userData);
        res->status = static_cast<Int>(fetch->status);

        emplaceHeaders(res, fetch);

        if (fetch->data)
            res->body = String(fetch->data, fetch->numBytes);

        emscripten_fetch_close(fetch);
    };

    emscripten_fetch(&attr, req.url().data());
    return res;
}

struct EmURLContext {
    funcptr_t<void (const HttpResponse &, void *)> callback;
    void *userdata;
    String body;
};

static auto asyncErrorCallback(emscripten_fetch_t *fetch) -> void
{
    auto context = static_cast<EmURLContext *>(fetch->userData);
    HttpResponse res {
        .status = static_cast<Int>(fetch->status),
        .error = fetch->statusText,
    };

    emplaceHeaders(&res, fetch);

    if (context)
    {
        if (context->callback)
            context->callback(res, context->userdata);
    }

    delete context;
    emscripten_fetch_close(fetch);
}

static auto asyncSuccessCallback(emscripten_fetch_t *fetch) -> void
{
    auto context = static_cast<EmURLContext *>(fetch->userData);
    HttpResponse res {
        .status = static_cast<Int>(fetch->status),
        .body = fetch->data ? String(fetch->data, fetch->numBytes) : ""
    };

    emplaceHeaders(&res, fetch);

    if (context)
    {
        if (context->callback)
            context->callback(res, context->userdata);
    }

    delete context;
    emscripten_fetch_close(fetch);
}

auto http::sendHttpRequest(
    const HttpRequest &req,
    funcptr_t<void(const HttpResponse &, void *)> callback,
    void *userptr) -> Bool
{
    emscripten_fetch_attr_t attr;
    emscripten_fetch_attr_init(&attr);

    attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY | EMSCRIPTEN_FETCH_STREAM_DATA;

    auto method = HttpRequest::getMethodString(req.method());
    if (method.empty())
        method = "GET";
    memory::copy(attr.requestMethod, method.data(), method.length());

    List<const char *> headers;
    if (req.headers().size() > 0)
    {
        headers.reserve(req.headers().size() * 2 + 1);
        for (const auto &[header, value] : req.headers())
        {
            headers.emplace_back(header.data());
            headers.emplace_back(value.data());
        }
        headers.emplace_back(Null);

        attr.requestHeaders = headers.data();
    }

    if ( !req.mimeType().empty() )
    {
        attr.overriddenMimeType = req.mimeType().data();
    }

    auto userdata = new EmURLContext {
        .callback = callback,
        .userdata = userptr,
        .body = req.body(),
    };

    attr.userData = userdata;

    if ( !userdata->body.empty() )
    {
        attr.requestData = userdata->body.c_str();
        attr.requestDataSize = userdata->body.size();
    }

    attr.onerror = asyncErrorCallback;
    attr.onsuccess = asyncSuccessCallback;

    emscripten_fetch(&attr, req.url().data() ? req.url().data() : "");

    return True;
}

KAZE_NS_END
