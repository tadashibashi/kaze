#pragma once
#include "miniaudio.h"

#if KAZE_AUDIO_DECODE_VORBIS
extern ma_decoding_backend_vtable g_ma_decoding_backend_vtable_libvorbis;
#endif
