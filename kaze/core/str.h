#pragma once
#include <kaze/core/lib.h>

KAZE_NS_BEGIN

namespace str {
    /// trim whitespace off either end of a string
    /// \param[in]  str        the string
    /// \param[in]  trimStart  whether to trim whitespace off of the start of the string
    /// \param[in]  trimEnd    whether to trim whitespace off of the end of the string
    /// \returns the trimmed string
    auto trim(StringView str, Bool trimStart, Bool trimEnd) -> StringView;

    /// strcmp function, except non-case-sensitive
    /// \param[in]  a  first string
    /// \param[in]  b  second string
    /// \returns 1 if a is lexigraphically larger, -1 if a is lexigraphically smaller,
    ///          and 0 if both strings are equal.
    auto compareCaseless(StringView a, StringView b) -> Int;

    /// \returns a lowercased copy of a string
    auto toLower(StringView s) -> String;

    /// \returns an uppercased copy of a string
    auto toUpper(StringView s) -> String;
}

KAZE_NS_END
