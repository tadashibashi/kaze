#pragma once
#include <kaze/core/lib.h>

KAZE_NS_BEGIN

/// TODO: make a create function, and hide the implementation details
struct HttpResponse
{
    Int status;
    String body;
    String error;
    Dictionary<String, String> headers {};

    List<String> cookies {}; // Raw cookie strings

    auto ok() const noexcept
    {
        return status >= 200 && status < 300;
    }
};

KAZE_NS_END
