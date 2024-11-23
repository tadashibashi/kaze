#pragma once
#include <kaze/core/lib.h>

KAZE_NS_BEGIN

class CStringView {
public:
    CStringView() : m_data(), m_length() { }
    CStringView(StringView str) : m_data(str.data()), m_length(str.length())
    {
        if (*(str.data() + str.length()) != '\0')
        {
            throw std::runtime_error("CStringView, StringView must be null terminated");
        }
    }

    CStringView(const String &str) : m_data(str.data()), m_length(str.length()) { }

    CStringView(const char *str) : m_data(str), m_length() {
        const char *p = str;
        while (*p != '\0')
        {
            ++p;
            ++m_length;
        }
    }

    CStringView(const char *str, Size length) : m_data(str), m_length(length) {
        if (*(str + length) != '\0')
        {
            throw std::runtime_error("CStringView, string passed must be null terminated");
        }
    }

    auto data() const noexcept -> const char * { return m_data; }
    auto length() const noexcept -> Size { return m_length; }
    auto size() const noexcept -> Size { return m_length; }
    auto empty() const noexcept -> Bool { return m_length == 0; }
private:
    const char *m_data;
    Size m_length;
};

KAZE_NS_END
