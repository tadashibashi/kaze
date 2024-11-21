# This module handles audio encoding & decoding
set(KAZE_MODULE KAZE_SND_CONV)

if (KAZE_SND_DECODE_VORBIS)
    # Download Ogg and Vorbis (prefer this to stb_vorbis due to better performance)
    FetchContent_Declare(ogg
        GIT_REPOSITORY https://github.com/xiph/ogg.git
        GIT_TAG        "v1.3.5"
    )

    FetchContent_Declare(vorbis
        GIT_REPOSITORY https://github.com/xiph/vorbis.git
        GIT_TAG        "v1.3.7"
    )

    set(OGG_LIBRARY                   $<TARGET_FILE:ogg>          CACHE STRING "")
    set(OGG_INCLUDE_DIR               ${ogg_SOURCE_DIR}/include   CACHE STRING "")
    set(BUILD_TESTING                 OFF                         CACHE BOOL   "")
    set(INSTALL_DOCS                  OFF                         CACHE BOOL   "")
    set(INSTALL_CMAKE_PACKAGE_MODULE  OFF                         CACHE BOOL   "")
    set(INSTALL_PKG_CONFIG_MODULE     OFF                         CACHE BOOL   "")
    FetchContent_MakeAvailable(ogg)
    FetchContent_MakeAvailable(vorbis)

    # Add the vorbisfile library, which includes the necessary ogg/vorbis libs
    list(APPEND KAZE_SND_CONV_LINK_LIBS_PRIVATE vorbisfile)

    # Add source for the vorbis decoder backend
    list(APPEND KAZE_SND_CONV_SOURCES_PRIVATE
        extern/miniaudio/miniaudio_libvorbis.cpp
        extern/miniaudio/miniaudio_libvorbis.h)
endif()

# Setup compile defines
kaze_normalize_bool(KAZE_SND_DECODE_FLAC KAZE_SND_DECODE_FLAC)
kaze_normalize_bool(KAZE_SND_DECODE_MP3 KAZE_SND_DECODE_MP3)
kaze_normalize_bool(KAZE_SND_DECODE_VORBIS KAZE_SND_DECODE_VORBIS)
kaze_normalize_bool(KAZE_SND_DECODE_WAV KAZE_SND_DECODE_WAV)
list(APPEND KAZE_SND_CONV_COMPILE_DEFS_PRIVATE
    KAZE_SND_DECODE_FLAC=${KAZE_SND_DECODE_FLAC}
    KAZE_SND_DECODE_MP3=${KAZE_SND_DECODE_MP3}
    KAZE_SND_DECODE_VORBIS=${KAZE_SND_DECODE_VORBIS}
    KAZE_SND_DECODE_WAV=${KAZE_SND_DECODE_WAV}
)

# Source code
list(APPEND KAZE_SND_CONV_SOURCES_PRIVATE
    extern/miniaudio/miniaudio.cpp
    extern/miniaudio/miniaudio.h
    extern/miniaudio/miniaudio_decoder_backends.h
    extern/miniaudio/miniaudio_ext.h

    AudioDecoder.cpp
    AudioDecoder.h
)
