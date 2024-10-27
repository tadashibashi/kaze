include(CheckCXXCompilerFlag)

# ===== Check for CPU Intrinsic Support =====
check_cxx_source_compiles("
#ifdef __AVX__
    int main() { return 0; }
#else
    #error __AVX__ is not defined
#endif
" KAZE_COMPILER_SUPPORTS_AVX)

check_cxx_source_compiles("
#ifdef __AVX2__
    int main() { return 0; }
#else
    #error __AVX2__ is not defined
#endif
" KAZE_COMPILER_SUPPORTS_AVX2)

check_cxx_source_compiles("
#ifdef __SSE__
    int main() { return 0; }
#else
    #error __SSE__ is not defined
#endif
" KAZE_COMPILER_SUPPORTS_SSE)

check_cxx_source_compiles("
#ifdef __SSE2__
    int main() { return 0; }
#else
    #error __SSE2__ is not defined
#endif
" KAZE_COMPILER_SUPPORTS_SSE2)

check_cxx_source_compiles("
#ifdef __SSE3__
    int main() { return 0; }
#else
    #error __SSE3__ is not defined
#endif
" KAZE_COMPILER_SUPPORTS_SSE3)

check_cxx_source_compiles("
#ifdef __SSSE3__
    int main() { return 0; }
#else
    #error __SSSE3__ is not defined
#endif
" KAZE_COMPILER_SUPPORTS_SSSE3)

check_cxx_source_compiles("
#ifdef __SSE4_1__
    int main() { return 0; }
#else
    #error __SSE4_1__ is not defined
#endif
" KAZE_COMPILER_SUPPORTS_SSE4_1)

check_cxx_source_compiles("
#ifdef __SSE4_2__
    int main() { return 0; }
#else
    #error __SSE4_2__ is not defined
#endif
" KAZE_COMPILER_SUPPORTS_SSE4_2)

check_cxx_source_compiles("
#ifdef __ARM_NEON
    int main() { return 0; }
#else
    #error __ARM_NEON is not defined
#endif
" KAZE_COMPILER_SUPPORTS_ARM_NEON)

if (EMSCRIPTEN)
    check_cxx_source_compiles("
        #include <wasm_simd128.h>
        int main() {
            wasm_f32x4_splat(0);
            return 0;
        }
    " KAZE_COMPILER_SUPPORTS_WASM_SIMD)
else()
    set(KAZE_COMPILER_SUPPORTS_WASM_SIMD 0)
endif()
