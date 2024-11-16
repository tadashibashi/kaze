set(KAZE_MODULE KAZE_EMSCRIPTEN)

set(KAZE_EMSCRIPTEN_LINK_OPTS_PUBLIC
    -sALLOW_MEMORY_GROWTH=1

     # Provide spacious initial buffer for potentially large asset files
    -sINITIAL_MEMORY=2GB

    # Provide generous space in case of deeply nested recursion
    -sSTACK_SIZE=4MB

    # We'll expose native functions for accessing URLs via fetch
    -sFETCH
)

if (KAZE_DEBUG)
    list(APPEND KAZE_EMSCRIPTEN_LINK_OPTS_PUBLIC
        # Provide source maps to the browser on debug builds
        -gsource-map
    )
endif()

if (${KAZE_EMSCRIPTEN_PTHREAD_POOL_SIZE} GREATER 0)
    if (KAZE_AUDIO)
        if (${KAZE_EMSCRIPTEN_PTHREAD_POOL_SIZE} LESS 4)
            set(KAZE_EMSCRIPTEN_PTHREAD_POOL_SIZE 4)
        endif()

        list (APPEND KAZE_EMSCRIPTEN_LINK_OPTS_PUBLIC
            -sAUDIO_WORKLET=1
            -sWASM_WORKERS=1
        )
    else()
        if (KAZE_EMSCRIPTEN_WASM_WORKERS)
            list (APPEND KAZE_EMSCRIPTEN_LINK_OPTS_PUBLIC
                -sWASM_WORKERS=1
            )
        endif()
    endif()

    list(APPEND KAZE_EMSCRIPTEN_LINK_OPTS_PUBLIC
        -pthread
        -sPTHREAD_POOL_SIZE=${KAZE_EMSCRIPTEN_PTHREAD_POOL_SIZE}
        -sALLOW_BLOCKING_ON_MAIN_THREAD=1
    )
    list(APPEND KAZE_EMSCRIPTEN_COMPILE_OPTS_PUBLIC -pthread)
endif()

if (KAZE_CPU_INTRINSICS)
    list(APPEND KAZE_EMSCRIPTEN_LINK_OPTS_PUBLIC -msimd128)
    list(APPEND KAZE_EMSCRIPTEN_COMPILE_OPTS_PUBLIC -msimd128)
endif()
