#include "str.h"
#include <kaze/core/platform/defines.h>
#include <cstdlib>
#include <codecvt>
#include <cwctype>

#if KAZE_PLATFORM_WINDOWS
#include <windows.h>
#endif

KAZE_NS_BEGIN

auto str::trim(StringView str, Bool trimStart, Bool trimEnd) -> StringView
{
    if (str.empty()) return "";

    Size start = 0;
    if (trimStart)
        while (std::isspace(str[start])) ++start;

    Size end = str.size() - 1;
    if (trimEnd)
        while (std::isspace(str[end])) --end;

    if (start >= end) return "";
    return str.substr(start, end - start + 1);
}

auto str::trim(WstringView str, Bool trimStart, Bool trimEnd) -> WstringView
{
    if (str.empty()) return L"";

    Size start = 0;
    if (trimStart)
        while (std::iswspace(str[start])) ++start;

    Size end = str.size() - 1;
    if (trimEnd)
        while (std::iswspace(str[end])) --end;

    if (start >= end) return L"";
    return str.substr(start, end - start + 1);
}

auto str::compareCaseless(StringView a, StringView b) -> Int
{
    const auto sizeA = a.length();
    const auto sizeB = b.length();

    if (sizeA < sizeB)
        return -1;
    else if (sizeA > sizeB)
        return 1;

    for (Size i = 0; i < sizeA; ++i)
    {
        const auto charA = std::tolower(a[i]);
        const auto charB = std::tolower(b[i]);
        if (charA < charB)
            return -1;
        else if (charA > charB)
            return 1;
    }

    return 0;
}

auto str::compareCaseless(WstringView a, WstringView b) -> Int
{
    const auto sizeA = a.length();
    const auto sizeB = b.length();

    if (sizeA < sizeB)
        return -1;
    else if (sizeA > sizeB)
        return 1;

    for (Size i = 0; i < sizeA; ++i)
    {
        const auto charA = std::towlower(a[i]);
        const auto charB = std::towlower(b[i]);
        if (charA < charB)
            return -1;
        else if (charA > charB)
            return 1;
    }

    return 0;
}

auto str::toLower(StringView s) -> String
{
    if ( !s.data() ) return "";

    String res;
    res.reserve(s.length());

    for (auto c : s)
    {
        res += std::tolower(c);
    }

    return res;
}

auto str::makeLower(String *s) -> String &
{
    if (!s) throw std::logic_error("Passed a null string to str::makeLower");

    for (auto &c : *s)
    {
        c = std::tolower(c);
    }

    return *s;
}

auto str::toLower(WstringView s) -> Wstring
{
    if (!s.data()) return L"";

    Wstring res;
    res.reserve(s.length());

    for (auto c : s)
    {
        res += std::towlower(c);
    }

    return res;
}

auto str::makeLower(Wstring *s) -> Wstring &
{
    if (!s) throw std::logic_error("Passed a null string to str::makeLower");

    for (auto &c : *s)
    {
        c = std::towlower(c);
    }

    return *s;
}

auto str::toUpper(StringView s) -> String
{
    if ( !s.data() ) return "";

    String res(s.data(), s.length());

    for (auto &c : res)
    {
        c = std::toupper(c);
    }

    return res;
}

auto str::makeUpper(String *s) -> String &
{
    if (!s) throw std::logic_error("Passed a null string to str::makeUpper");

    for (auto &c : *s)
    {
        c = std::toupper(c);
    }

    return *s;
}

auto str::toUpper(WstringView s) -> Wstring
{
    if (!s.data()) return L"";

    Wstring res;
    res.reserve(s.length());

    for (auto c : s)
    {
        res += std::towupper(c);
    }

    return res;
}

auto str::makeUpper(Wstring *s) -> Wstring &
{
    if (!s) throw std::logic_error("Passed a null string to str::makeUpper");

    for (auto &c : *s)
    {
        c = std::towupper(c);
    }

    return *s;
}

auto str::toWstring(const String &s) -> Wstring
{
    return toWstring(StringView(s.data(), s.length()));
}

auto str::toWstring(StringView s) -> Wstring
{
    if (s.empty())
    {
        return {};
    }

#if KAZE_PLATFORM_WINDOWS
    auto length = MultiByteToWideChar(CP_UTF8, 0, s.data(), static_cast<int>(s.size()), Null, 0);
    Wstring wstr(length, L'\0');

    MultiByteToWideChar(CP_UTF8, 0, s.data(), static_cast<int>(s.size()), &wstr[0], length);

    return wstr;
#else
    auto length = std::mbstowcs(Null, s.data(), 0);
    if (length == static_cast<size_t>(-1))
    {
        KAZE_PUSH_ERR(Error::RuntimeErr, "Failure during conversion of string to wstring");
        return {};
    }

    Wstring wstr(length, L'\0');
    std::mbstowcs(wstr.data(), s.data(), length);

    return wstr;
#endif
}

auto str::toWstring(CStringView s) -> Wstring
{
    return toWstring(StringView(s.data(), s.size()));
}

auto str::toString(const Wstring &ws) -> String
{
    return toString(WstringView(ws.data(), ws.length()));
}

auto str::toString(WstringView ws) -> String
{
#if KAZE_PLATFORM_WINDOWS
    Int sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, ws.data(), static_cast<Int>(ws.size()), Null, 0, Null, Null);
    String s(sizeNeeded, 0);
    WideCharToMultiByte(CP_UTF8, 0, ws.data(), static_cast<Int>(ws.size()), &s[0], sizeNeeded, Null, Null);

    return s;
#else
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    return String(converter.to_bytes(wstr));
#endif
}

auto str::toString(StringView view) -> String
{
    return view.data() ? String(view.data(), view.size()) : String();
}

auto str::toString(CStringView view) -> String
{
    return view.data() ? String(view.data(), view.size()) : String();
}

KAZE_NS_END
