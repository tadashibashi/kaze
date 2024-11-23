#pragma once
#include <kaze/core/lib.h>
#include <kaze/core/CStringView.h>

KAZE_NS_BEGIN

namespace str {
    /// trim whitespace off either end of a string
    /// \param[in]  str        the string
    /// \param[in]  trimStart  whether to trim whitespace off of the start of the string
    /// \param[in]  trimEnd    whether to trim whitespace off of the end of the string
    /// \returns the trimmed string
    [[nodiscard]]
    auto trim(StringView str, Bool trimStart, Bool trimEnd) -> StringView;

    /// trim white space off either end of a wstring
    /// \param[in]  str        the wstring
    /// \param[in]  trimStart  whether to trim whitespace off of the start of the string
    /// \param[in]  trimEnd    whether to trim whitespace off of the end of the string
    /// \returns the trimmed wstring
    [[nodiscard]]
    auto trim(WstringView str, Bool trimStart, Bool trimEnd) -> WstringView;

    /// strcmp function, except non-case-sensitive
    /// \param[in]  a  first string
    /// \param[in]  b  second string
    /// \returns 1 if a is lexigraphically larger, -1 if a is lexigraphically smaller,
    ///          and 0 if both strings are equal.
    [[nodiscard]]
    auto compareCaseless(StringView a, StringView b) -> Int;

    /// strcmp function, except non-case-sensitive
    /// \param[in]  a  first string
    /// \param[in]  b  second string
    /// \returns 1 if a is lexigraphically larger, -1 if a is lexigraphically smaller,
    ///          and 0 if both strings are equal.
    [[nodiscard]]
    auto compareCaseless(WstringView a, WstringView b) -> Int;

    /// \param[in] s  string to convert
    /// \returns a lowercased copy of the string
    [[nodiscard]]
    auto toLower(StringView s) -> String;

    /// Transform a string to lower-case, mutating the passed in string
    /// \param[inout] s  string to transform
    /// \returns a reference to the passed in string
    auto makeLower(String *s) -> String &;

    /// \param[in] s  wstring to convert
    /// \returns a lowercased copy of the wstring
    [[nodiscard]]
    auto toLower(WstringView s) -> Wstring;

    /// Transform a wstring to lower-case, mutating the passed in wstring
    /// \param[inout] s  wstring to transform
    /// \returns a reference to the passed in wstring
    auto makeLower(Wstring *s) -> Wstring &;

    /// \param[in] s  string to convert
    /// \returns an uppercased copy of the string
    [[nodiscard]]
    auto toUpper(StringView s) -> String;

    /// Transform a string to upper-case, mutating the passed in string
    /// \param[inout] s  string to transform
    /// \returns a reference to the passed in string
    auto makeUpper(String *s) -> String &;

    /// \param[in] s  wstring to convert
    /// \returns an uppercased copy of the wstring
    [[nodiscard]]
    auto toUpper(WstringView s) -> Wstring;

    /// Transform a wstring to upper-case, mutating the passed in wstring
    /// \param[inout] s  wstring to transform
    auto makeUpper(Wstring *s) -> Wstring &;

    [[nodiscard]]
    auto toWstring(const String &s) -> Wstring;

    /// \param[in] s  string to convert
    /// \returns a converted copy of a StringView data to a Wstring
    [[nodiscard]]
    auto toWstring(StringView s) -> Wstring;

    /// \param[in] s  string to convert
    /// \returns a converted copy of a CStringView to a Wstring
    [[nodiscard]]
    auto toWstring(CStringView s) -> Wstring;

    /// \param[in] ws  wide string to convert
    /// \returns a converted copy of a WstringView to a String
    [[nodiscard]]
    auto toString(const Wstring &ws) -> String;

    /// \param[in] ws  wide string to convert
    /// \returns a converted copy of a WstringView to a String
    [[nodiscard]]
    auto toString(WstringView ws) -> String;

    /// \param[in] view  string to convert
    /// \returns a copy of the StringView into a String
    [[nodiscard]]
    auto toString(StringView view) -> String;

    /// \param[in] view  string to convert
    /// \returns a copy of the CStringView into a String
    [[nodiscard]]
    auto toString(CStringView view) -> String;
}

KAZE_NS_END
