#include "http.h"
#include <kaze/core/debug.h>
#include <kaze/core/str.h>

#include <kaze/core/platform/defines.h>

#include <windows.h>
#include <winhttp.h>

#include <regex>
#include <string>
#include <thread>

#include <kaze/core/math.hpp>

#if KAZE_ARCH_ARM
#   if KAZE_ARCH_64BIT
#       define KAZE_USER_AGENT L"KazeEngine/1.0 (Windows arm64)"
#   else
#       define KAZE_USER_AGENT L"KazeEngine/1.0 (Windows arm32)"
#   endif
#elif KAZE_ARCH_X86_64
#   define KAZE_USER_AGENT L"KazeEngine/1.0 (Windows x64)"
#elif KAZE_ARCH_X86_32
#   define KAZE_USER_AGENT L"KazeEngine/1.0 (Windows x86)"
#else
#   define KAZE_USER_AGENT L"KazeEngine/1.0 (Windows)"
#endif

KAZE_NS_BEGIN

static auto getComponentsFromURL(const Wstring &url, Wstring *hostname, Wstring *pathAndQuery) -> Bool
{
    static std::wregex regex(LR"((?:https?://)?([^/:]+)(/[^?]*)?(\?.*)?)");
    std::wsmatch match;

    if (std::regex_search(url, match, regex))
    {
        if (hostname)
            *hostname = match[1].str();

        if (pathAndQuery)
            *pathAndQuery = match[2].str();

        return True;
    }

    return False;
}

static auto getResponseBody(HINTERNET hRequest) -> String
{
    String body;
    DWORD dwSize = 0;

    // Query initial data size, if avaialble
    if (WinHttpQueryDataAvailable(hRequest, &dwSize) && dwSize > 0)
    {
        body.reserve(dwSize);
    }

    char buffer[4096];
    do {
        DWORD bytesRead = 0;

        // Get size of available data
        if (!WinHttpQueryDataAvailable(hRequest, &dwSize))
        {
            KAZE_PUSH_ERR(Error::RuntimeErr, "Failed to query http data availability");
            return {};
        }

        if (!WinHttpReadData(hRequest, buffer, mathf::min<DWORD>(dwSize, sizeof(buffer)), &bytesRead))
        {
            KAZE_PUSH_ERR(Error::RuntimeErr, "Failed to read http data");
            return {};
        }

        body += StringView(buffer, bytesRead);
    } while (dwSize > 0);

    return body;
}

static auto parseResponseHeaders(HINTERNET hRequest, Dictionary<String, String> *outHeaders, List<String> *outCookies) -> Bool
{
    // Get the header buffer size
    DWORD dwSize = 0;
    WinHttpQueryHeaders(
        hRequest,
        WINHTTP_QUERY_RAW_HEADERS_CRLF,
        WINHTTP_HEADER_NAME_BY_INDEX,
        Null,
        &dwSize,
        WINHTTP_NO_HEADER_INDEX);

    // We want this error, since we passed a null buffer just to query the buffer size
    if (GetLastError() != ERROR_INSUFFICIENT_BUFFER)
    {
        KAZE_PUSH_ERR(Error::RuntimeErr, "WinHttpQueryHeaders failed to determine headers size");
        return False;
    }
    
    if (dwSize == 0) return True; // Edge case?

    // Allocate and fill buffer for headers
    Wstring headers(dwSize / sizeof(wchar_t), L'\0');
    if (!WinHttpQueryHeaders(
        hRequest,
        WINHTTP_QUERY_RAW_HEADERS_CRLF,
        WINHTTP_HEADER_NAME_BY_INDEX,
        headers.data(),
        &dwSize,
        WINHTTP_NO_HEADER_INDEX))
    {
        KAZE_PUSH_ERR(Error::RuntimeErr, "WinHttpQueryHeaders failed to get headers, "
            "with Windows error code: {}",
            GetLastError());
        return False;
    }

    Dictionary<String, String> tempHeaders;
    List<String> tempCookies;

    WstringView headerView = headers;

    // Parse headers by line
    Size startLine = 0, endLine = 0;
    while ((endLine = headerView.find(L"\r\n", startLine)) != WstringView::npos)
    {
        WstringView line(headerView.data() + startLine, endLine - startLine);

        auto colonIdx = line.find_first_of(L':');
        if (colonIdx != WstringView::npos)
        {
            const auto header = str::trim(line.substr(0, colonIdx), True, True);
            const auto value = str::trim(line.substr(colonIdx + 1), True, True);

            if (str::compareCaseless(header, L"set-cookie") == 0)
            {
                // cookie
                tempCookies.emplace_back(str::toString(value));
            }
            else
            {
                // other header type
                auto tempHeader = str::toString(header);
                tempHeaders[str::makeLower(&tempHeader)] = str::toString(value);
            }
        }

        startLine = endLine + 2; // "\r\n" length
    }

    if (outHeaders)
        tempHeaders.swap(*outHeaders);
    if (outCookies)
        tempCookies.swap(*outCookies);
    return True;
}

static auto getSecureProtocols() -> DWORD
{
    static DWORD protocols;
    static Bool setProtocols;
    if (!setProtocols)
    {
        DWORD size = sizeof(protocols);
        if (WinHttpQueryOption(Null, WINHTTP_OPTION_SECURE_PROTOCOLS, &protocols, &size))
        {
            setProtocols = True;
        }
    }

    return protocols;
}

auto http::sendHttpRequestSync(
    const HttpRequest &req
) -> HttpResponse
{
    HttpResponse res{};

    const auto url = str::toWstring(req.url());
    std::wstring hostname;
    std::wstring pathAndQuery;
    if (!getComponentsFromURL(url, &hostname, &pathAndQuery))
    {
        KAZE_PUSH_ERR(Error::RuntimeErr, "Failed to get components from URL");
        return {};
    }

    HINTERNET hSession = WinHttpOpen(KAZE_USER_AGENT,
        WINHTTP_ACCESS_TYPE_NO_PROXY,
        WINHTTP_NO_PROXY_NAME,
        WINHTTP_NO_PROXY_BYPASS, 0);

    if (hSession)
    {
        auto secureProtocols = getSecureProtocols();
        if (secureProtocols)
            WinHttpSetOption(hSession, WINHTTP_OPTION_SECURE_PROTOCOLS, &secureProtocols, sizeof(secureProtocols));

        HINTERNET hConnect = WinHttpConnect(hSession, hostname.data(), INTERNET_DEFAULT_HTTP_PORT, 0);
        if (hConnect)
        {
            auto method = str::toWstring(req.methodString());
            HINTERNET hRequest = WinHttpOpenRequest(hConnect, method.c_str(), pathAndQuery.c_str(), Null,
                WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, secureProtocols ? WINHTTP_FLAG_SECURE : 0);

            if (hRequest)
            {
                // Add headers
                for (const auto &[header, value] : req.headers())
                {
                    auto headerWstr = str::toWstring(header) + L": " + str::toWstring(value);
                    WinHttpAddRequestHeaders(hRequest, headerWstr.c_str(), headerWstr.length(), WINHTTP_ADDREQ_FLAG_ADD);
                }

                // Add mime-type
                if (!req.mimeType().empty())
                {
                    WinHttpAddRequestHeaders(hRequest, (L"Content-Type: " + str::toWstring(req.mimeType())).c_str(), req.mimeType().size() + 14,
                        WINHTTP_ADDREQ_FLAG_ADD);
                }

                // Send the request with no initial data
                if (WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0,
                    WINHTTP_NO_REQUEST_DATA, 0, 0, 0))
                {
                    // Write body to buffer/stream data
                    if (!req.body().empty())
                    {
                        if (!WinHttpWriteData(hRequest, req.body().data(), req.body().size(), Null))
                        {
                            KAZE_PUSH_ERR(Error::RuntimeErr, "Failed to write body data");
                        }
                    }

                    if (WinHttpReceiveResponse(hRequest, Null))
                    {
                        // Successful request-response
                        res.body = std::move(getResponseBody(hRequest));

                        parseResponseHeaders(hRequest, &res.headers, &res.cookies);
                    }
                    else
                    {
                        KAZE_PUSH_ERR(Error::RuntimeErr, "Failed to receive request: {}", GetLastError());
                    }
                }
                else
                {
                    KAZE_PUSH_ERR(Error::RuntimeErr, "Failed to send request: {}", GetLastError());
                }

                WinHttpCloseHandle(hRequest);
            }

            WinHttpCloseHandle(hConnect);
        }

        WinHttpCloseHandle(hSession);
    }

    return res;
}

/// Send an asynchronous http request
/// \param[in]  req       request to send
/// \param[in]  callback  callback on success or error, check status or `HttpRequest::ok`
/// \param[in]  userdata  user context pointer that will be passed to the `callback`
/// \returns whether the request was sent successfully without errors
auto http::sendHttpRequest(
    const HttpRequest &req,
    funcptr_t<void(const HttpResponse &res, void *userdata)> callback,
    void *userdata
) -> Bool
{
    if (!callback)
    {
        KAZE_PUSH_ERR(Error::NullArgErr,
            "http::sendHttpRequest is missing required `callback` argument");
        return False;
    }

    auto thd = std::thread([](
            HttpRequest req,
            funcptr_t<void(const HttpResponse &res, void *userdata)> callback,
            void *userdata)
        {
            const auto res = sendHttpRequestSync(req);
            callback(res, userdata);
        }, req, callback, userdata);

    thd.detach();
    return True;
}

KAZE_NS_END
