include(CheckCXXCompilerFlag)

# ===== CPU Processor Checks =====
check_cxx_source_compiles("
#if defined(i386) || defined(__i386) || defined(__i386__) || defined(__i486__) ||    \
  defined(__i586__) || defined(__i686__) || defined(__IA32__)|| defined(_M_I86) ||   \
  defined(_M_IX86)|| defined(__X86__) || defined(_X86_) || defined(__THW_INTEL__) || \
  defined(__I86__) || defined(__INTEL__) || defined(__386)
    int main() { return 0; }
#else
    #error Not an Intel cpu
#endif
" KAZE_ARCH_INTEL)

check_cxx_source_compiles("
#if defined(__arm__) || defined(__thumb__) || defined(__TARGET_ARCH_ARM) ||               \
  defined(__TARGET_ARCH_THUMB) || defined(__ARM) || defined(_M_ARM) || defined(_M_ARM_T) || \
  defined(__ARM_ARCH)
    int main() { return 0; }
#else
    #error Not an Arm cpu
#endif
" KAZE_ARCH_ARM)

check_cxx_source_compiles("
#if defined(__x86_64__) || defined(_M_X64) || defined(__aarch64__) || defined(_M_ARM64)
    int main() { return 0; }
#else
    #error Not 64-bit cpu
#endif
" KAZE_ARCH_64BIT)

if (KAZE_ARCH_64BIT)
    set(KAZE_ARCH_BITS 64)
else()
    set(KAZE_ARCH_BITS 32)
endif()

# ===== Check for CPU Intrinsic Support =====
if (KAZE_ARCH_INTEL)

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
else()
    set(KAZE_COMPILER_SUPPORTS_AVX    0)
    set(KAZE_COMPILER_SUPPORTS_AVX2   0)
    set(KAZE_COMPILER_SUPPORTS_SSE    0)
    set(KAZE_COMPILER_SUPPORTS_SSE2   0)
    set(KAZE_COMPILER_SUPPORTS_SSE3   0)
    set(KAZE_COMPILER_SUPPORTS_SSSE3  0)
    set(KAZE_COMPILER_SUPPORTS_SSE4_1 0)
    set(KAZE_COMPILER_SUPPORTS_SSE4_2 0)
endif()

if (KAZE_ARCH_ARM)
    check_cxx_source_compiles("
    #if defined(__ARM_NEON) || defined(_M_ARM) || defined(_M_ARM64)
        int main() { return 0; }
    #else
        #error arm neon is not defined
    #endif
    " KAZE_COMPILER_SUPPORTS_ARM_NEON)
else()
    set(KAZE_COMPILER_SUPPORTS_ARM_NEON 0)
endif()

if (EMSCRIPTEN)
    set(CMAKE_REQURIED_FLAGS "-msimd128")
    check_cxx_source_compiles("
        #include <wasm_simd128.h>
        int main() {
            wasm_f32x4_splat(0);
            return 0;
        }
    " KAZE_COMPILER_SUPPORTS_WASM_SIMD)
    unset(CMAKE_REQUIRED_FLAGS)
else()
    set(KAZE_COMPILER_SUPPORTS_WASM_SIMD 0)
endif()
