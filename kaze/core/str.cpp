#include "str.h"

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

auto str::toLower(StringView s) -> String
{
    if ( !s.data() ) return "";

    String res(s.data(), s.length());

    for (auto &c : res)
    {
        c = std::tolower(c);
    }

    return res;
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

KAZE_NS_END
