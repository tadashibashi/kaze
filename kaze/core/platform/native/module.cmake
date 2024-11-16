set(KAZE_MODULE KAZE_NATIVE)

if (KAZE_PLATFORM_ANDROID)
    kaze_add_submodule(android)
elseif(KAZE_PLATFORM_EMSCRIPTEN)
    kaze_add_submodule(emscripten)
endif()
