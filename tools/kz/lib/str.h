#pragma once
#include <string>
#include <string_view>

/// \note these operations use string_view to prevent allocations, make sure to
/// consider the lifetimes of your string data when using these functions.
namespace kz::str {
    /// trim whitespace off either end of a string
    /// \param[in]  str        the string
    /// \param[in]  trimStart  whether to trim whitespace off of the start of the string
    /// \param[in]  trimEnd    whether to trim whitespace off of the end of the string
    /// \returns the trimmed string
    auto trim(std::string_view str, bool trimStart = true, bool trimEnd = true) -> std::string_view;

    /// strcmp function, except non-case-sensitive
    /// \param[in]  a  first string
    /// \param[in]  b  second string
    /// \returns 1 if a is lexigraphically larger, -1 if a is lexigraphically smaller,
    ///          and 0 if both strings are equal.
    auto compareNoCase(std::string_view a, std::string_view b) -> int;

    auto fromView(std::string_view view) -> std::string;
}
