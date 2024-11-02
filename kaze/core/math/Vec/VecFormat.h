#pragma once
#include <kaze/core/lib.h>
#include <kaze/core/math/Vec/VecBase.h>

template<KAZE_NS::Arithmetic T, KAZE_NS::Size S> requires (S > 0)
struct std::formatter<KAZE_NS::Vec<T, S>> : std::formatter<std::string> {
    auto format(const KAZE_NS::Vec<T, S>& v, std::format_context& ctx) {
        std::format_to(ctx.out(), "{{");
        for (KAZE_NS::Size i = 0; i < S; ++i)
        {
            std::format_to(ctx.out(), "{}", v[i]);
            if (i < S - 1UL)
            {
                std::format_to(ctx.out(), ", ");
            }
        }

        return std::format_to(ctx.out(), "}}");
    }
};


