#pragma once
#include <kaze/core/lib.h>
#include <kaze/core/math/Vec/VecBase.h>

template<KAZE_NS::Arithmetic T, KAZE_NS::Size S> requires (S > 0)
struct fmt_lib::formatter<KAZE_NS::Vec<T, S>> : fmt_lib::formatter<KAZE_NS::String> {
    auto format(const KAZE_NS::Vec<T, S>& v, fmt_lib::format_context& ctx) {
        fmt_lib::format_to(ctx.out(), "{{");
        for (KAZE_NS::Size i = 0; i < S; ++i)
        {
            fmt_lib::format_to(ctx.out(), "{}", v[i]);
            if (i < S - 1UL)
            {
                fmt_lib::format_to(ctx.out(), ", ");
            }
        }

        return fmt_lib::format_to(ctx.out(), "}}");
    }
};


