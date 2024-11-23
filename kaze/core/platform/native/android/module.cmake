set (KAZE_MODULE KAZE_ANDROID)

set(KAZE_ANDROID_SOURCES_PUBLIC
    AndroidNative.cpp
    AndroidNative.h
)

if (KAZE_GFX)
    list(APPEND KAZE_ANDROID_LINK_LIBS_PUBLIC
        GLESv2
    )
endif()

if (KAZE_SND)
    list(APPEND KAZE_ANDROID_LINK_LIBS_PUBLIC
            GLESv2
    )
endif()