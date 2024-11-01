#include "str.h"

auto kz::str::trim(std::string_view str, bool trimStart, bool trimEnd) -> std::string_view
{
    if (str.empty()) return "";

    size_t start = 0;
    if (trimStart)
        while (std::isspace(str[start])) ++start;

    size_t end = str.size() - 1;
    if (trimEnd)
        while (std::isspace(str[end])) --end;

    if (start >= end) return "";
    return str.substr(start, end - start + 1);
}

auto kz::str::compareNoCase(std::string_view a, std::string_view b) -> int
{
    const auto sizeA = a.length();
    const auto sizeB = b.length();

    if (sizeA < sizeB)
        return -1;
    else if (sizeA > sizeB)
        return 1;

    for (size_t i = 0; i < sizeA; ++i)
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

auto kz::str::fromView(std::string_view view) -> std::string
{
    return {view.data(), view.size()};
}
