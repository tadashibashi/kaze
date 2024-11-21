#pragma once
#include <kaze/internal/core/lib.h>
#include <kaze/internal/core/math/Vec/VecBase.h>

template<KAZE_NS_INTERNAL::Arithmetic T, KAZE_NS_INTERNAL::Size S> requires (S > 0)
struct fmt_lib::formatter<KAZE_NS_INTERNAL::Vec<T, S>> : fmt_lib::formatter<KAZE_NS_INTERNAL::String> {
    auto format(const KAZE_NS_INTERNAL::Vec<T, S>& v, fmt_lib::format_context& ctx) {
        fmt_lib::format_to(ctx.out(), "{{");
        for (KAZE_NS_INTERNAL::Size i = 0; i < S; ++i)
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
