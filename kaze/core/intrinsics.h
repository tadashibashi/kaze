#pragma once

#if KAZE_CPU_INTRINSICS

#   if defined(__ARM_NEON) || defined(_M_ARM) || defined(_M_ARM64) // ARM NEON
#       define KAZE_CPU_ARM_NEON 1
#       include <arm_neon.h>

        inline float32x4_t vdivq_f32_approx(float32x4_t a, float32x4_t b) {
            // Reciprocal estimate
            float32x4_t reciprocal = vrecpeq_f32(b);

            // One iteration of Newton-Raphson refinement
            reciprocal = vmulq_f32(vrecpsq_f32(b, reciprocal), reciprocal);

            // Perform division as multiplication by the reciprocal
            return vmulq_f32(a, reciprocal);
        }

#   elif defined(__EMSCRIPTEN__) // WASM SIMD
#       define KAZE_CPU_WASM_SIMD 1
#       include <wasm_simd128.h>
#   elif defined(__AVX__)        // Intel
#       define KAZE_CPU_AVX 1
#       include <immintrin.h>
#   elif defined(__SSE__)        // Intel
#       define KAZE_CPU_SSE 1
#       include <immintrin.h>
#   endif

#endif // KAZE_CPU_INTRINSICS


#if !defined(KAZE_CPU_ARM_NEON)
#   define KAZE_CPU_ARM_NEON 0
#endif

#if !defined(KAZE_CPU_WASM_SIMD)
#   define KAZE_CPU_WASM_SIMD 0
#endif

#if !defined(KAZE_CPU_AVX)
#   define KAZE_CPU_AVX 0
#endif

#if !defined(KAZE_CPU_SSE)
#   define KAZE_CPU_SSE 0
#endif
