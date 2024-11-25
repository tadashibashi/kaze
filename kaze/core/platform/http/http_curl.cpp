#include "http.h"
#include <kaze/core/debug.h>
#include <kaze/core/memory.h>
#include <kaze/core/io/stream/Rstream.h>
#include <kaze/core/str.h>

#include <curl/curl.h>

#include <thread>

KAZE_NS_BEGIN

class CurlGuard
{
public:
    CurlGuard() : m_didInit()
    {
        if ( s_initCount == 0 )
        {
            auto result = curl_global_init(CURL_GLOBAL_DEFAULT);
            if (result == CURLE_OK)
            {
                s_initCount = 1;
                m_didInit = True;
            }
        }
        else
        {
            ++s_initCount;
            m_didInit = True;
        }
    }

    ~CurlGuard()
    {
        if (m_didInit)
        {
            --s_initCount;
            if (s_initCount <= 0)
            {
                curl_global_cleanup();
                s_initCount = 0;
            }
        }
    }

private:
    static Int s_initCount;
    Bool m_didInit;
};

Int CurlGuard::s_initCount;

static CurlGuard curlGuard{};

#define CURL_CHECK(statement) do { if (const auto result = (statement); result != CURLE_OK) { \
        KAZE_PUSH_ERR(Error::RuntimeErr, "Failed curl operation: {}, line: {}", curl_easy_strerror(result), __LINE__); \
        return HttpResponse{};\
    } } while (0)

static auto curlWriteCallback(char *data, size_t size, size_t numItems, void *clientp) -> size_t
{
    if ( !data || !clientp )
        return 0;
    auto &res = *static_cast<HttpResponse *>(clientp);

    const size_t fullSize = size * numItems;

    const auto oldSize = res.body.size();
    res.body.resize(oldSize + fullSize);

    memory::copy(res.body.data() + oldSize, data, fullSize);
    return fullSize;
}

static auto curlHeaderCallback(const char *buffer, const size_t size, const size_t nitems, void *userdata) -> size_t
{
    if ( !buffer || !userdata )
        return 0;

    auto &res = *static_cast<HttpResponse *>(userdata);

    StringView view(buffer, size * nitems);
    view = str::trim(view, true, true);
    if (view.empty() || view.starts_with("HTTP/"))
    {
        return size *nitems;
    }

    const auto colonPos = view.find_first_of(':');
    if (colonPos == StringView::npos)
        return 0; // faulty header

    const auto header = str::toLower(str::trim(view.substr(0, colonPos), True, False));
    const auto value = str::trim(view.substr(colonPos + 1), True, True);

    if (header == "set-cookie")
    {
        res.cookies.emplace_back(value);
    }
    else
    {
        res.headers[header] = value;
    }

    return size * nitems;
}

static auto curlReadCallback(char *buffer, size_t size, size_t nitems, void *userdata) -> size_t
{
    // Read data from the stream to the out buffer
    auto &stream = *static_cast<Rstream *>(userdata);
    const auto bytesRead = stream.read(buffer, static_cast<Int64>(size * nitems));

    // Report number of bytes read, 0 will signal end of file
    return static_cast<size_t>(bytesRead);
}

auto http::sendHttpRequestSync(
    const HttpRequest &req
) -> HttpResponse
{
    HttpResponse res{};
    auto curl = curl_easy_init();
    if ( !curl )
    {
        KAZE_PUSH_ERR(Error::RuntimeErr, "Failed to init curl");
        return {};
    }

    // Set url
    CURL_CHECK(curl_easy_setopt(curl, CURLOPT_URL, req.url().data() ? req.url().data() : ""));

    // Set method
    auto method = req.methodString();
    if (method.data())
    {
        CURL_CHECK(curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, method.data()));
    }

    // Create headers
    curl_slist *headers = Null;
    for (const auto &[header, value] : req.headers())
    {
        if (!header.data() || !value.data())
            continue;
        headers = curl_slist_append(headers,
            fmt_lib::format("{}: {}", header, value).data());
    }

    // Set body stream
    String bodyStr;
    req.swapBody(bodyStr);

    Rstream bodyStream;
    bodyStream.openConstMem({bodyStr.data(), bodyStr.size()});

    if (!bodyStr.empty())
    {
        CURL_CHECK(curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L));
        CURL_CHECK(curl_easy_setopt(curl, CURLOPT_READFUNCTION, curlReadCallback));
        CURL_CHECK(curl_easy_setopt(curl, CURLOPT_READDATA, &bodyStream));
        CURL_CHECK(curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE,
            static_cast<curl_off_t>(bodyStr.length())));

        // Mime type
        if (req.mimeType().data())
        {
            headers = curl_slist_append(headers,
                fmt_lib::format("Content-Type: {}", req.mimeType().data()).data());
        }
    }

    // Set headers
    if (headers)
    {
        CURL_CHECK(curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers));
    }

    // Set write callback
    CURL_CHECK(curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curlWriteCallback));
    CURL_CHECK(curl_easy_setopt(curl, CURLOPT_WRITEDATA, &res));

    // Set header callback
    CURL_CHECK(curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, curlHeaderCallback));
    CURL_CHECK(curl_easy_setopt(curl, CURLOPT_HEADERDATA, &res));

    CURL_CHECK(curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L));

    // Perform request
    const auto result = curl_easy_perform(curl);
    if (headers)
        curl_slist_free_all(headers);
    if (result != CURLE_OK)
    {
        KAZE_PUSH_ERR(Error::RuntimeErr,
            "CURL request failed: {}", curl_easy_strerror(result));
        return {};
    }

    long responseCode;
    CURL_CHECK(curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &responseCode));

    res.status = static_cast<Int>(responseCode);

    curl_easy_cleanup(curl);
    return res;
}

auto http::sendHttpRequest(
    const HttpRequest &req,
    funcptr_t<void(const HttpResponse &res, void *userdata)> callback,
    void *userdata
) -> Bool
{
    if ( !callback )
    {
        KAZE_PUSH_ERR(Error::NullArgErr, "sendHttpRequest requires param `callback` to be non-null");
        return False;
    }

    // Fire synchronous request in a thread
    auto thd = std::thread([](
        const HttpRequest &req,
        funcptr_t<void(const HttpResponse &res, void *userdata)> callback,
        void *userdata)
    {
        const auto res = sendHttpRequestSync(req);
        if (callback)
        {
            callback(res, userdata);
        }
    }, req, callback, userdata);

    thd.detach();
    return True;
}

auto http::getLocalHost() -> Cstring
{
    return "localhost"; // This may need to change depending if we use curl on other platforms besides linux
}

KAZE_NS_END
