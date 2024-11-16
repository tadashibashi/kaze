set(KAZE_MODULE KAZE_AUDIO_CONV)

if (KAZE_AUDIO_DECODE_VORBIS)
    # Download Ogg and Vorbis (prefer this to stb_vorbis due to better performance)
    FetchContent_Declare(ogg
        GIT_REPOSITORY https://github.com/xiph/ogg.git
        GIT_TAG        "v1.3.5"
    )

    FetchContent_Declare(vorbis
        GIT_REPOSITORY https://github.com/xiph/vorbis.git
        GIT_TAG        "v1.3.7"
    )

    set(OGG_LIBRARY $<TARGET_FILE:ogg>)
    set(OGG_INCLUDE_DIR ${ogg_SOURCE_DIR}/include)
    set(BUILD_TESTING OFF)

    FetchContent_MakeAvailable(ogg)
    FetchContent_MakeAvailable(vorbis)

    # Add the vorbisfile library, which includes the necessary ogg/vorbis libs
    list(APPEND KAZE_AUDIO_CONV_LINK_LIBS_PRIVATE vorbisfile)

    # Add source for the vorbis decoder backend
    list(APPEND KAZE_AUDIO_CONV_SOURCES_PRIVATE
        extern/miniaudio/miniaudio_libvorbis.cpp
        extern/miniaudio/miniaudio_libvorbis.h)
endif()

# Setup compile defines
kaze_normalize_bool(KAZE_AUDIO_DECODE_FLAC KAZE_AUDIO_DECODE_FLAC)
kaze_normalize_bool(KAZE_AUDIO_DECODE_MP3 KAZE_AUDIO_DECODE_MP3)
kaze_normalize_bool(KAZE_AUDIO_DECODE_VORBIS KAZE_AUDIO_DECODE_VORBIS)
kaze_normalize_bool(KAZE_AUDIO_DECODE_WAV KAZE_AUDIO_DECODE_WAV)
list(APPEND KAZE_AUDIO_CONV_COMPILE_DEFS_PRIVATE
    KAZE_AUDIO_DECODE_FLAC=${KAZE_AUDIO_DECODE_FLAC}
    KAZE_AUDIO_DECODE_MP3=${KAZE_AUDIO_DECODE_MP3}
    KAZE_AUDIO_DECODE_VORBIS=${KAZE_AUDIO_DECODE_VORBIS}
    KAZE_AUDIO_DECODE_WAV=${KAZE_AUDIO_DECODE_WAV}
)

# Source code
list(APPEND KAZE_AUDIO_CONV_SOURCES_PRIVATE
    extern/miniaudio/miniaudio.cpp
    extern/miniaudio/miniaudio.h
    extern/miniaudio/miniaudio_decoder_backends.h
    extern/miniaudio/miniaudio_ext.h

    AudioDecoder.cpp
    AudioDecoder.h
)
