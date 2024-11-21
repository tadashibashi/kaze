/// Contains some functions using private implementation of miniaudio
#pragma once
#include <kaze/snd/lib.h>
#include <kaze/snd/AudioMarker.h>
#include <kaze/core/MemView.h>

KSND_NS_BEGIN

/// Get marker data from a WAV file
/// \param[in]  filepath     filepath of the wav file to open
/// \param[out] outMarkers   pointer to assign marker list to
/// \param[out] outFreq      pointer to get sample rate of the WAV file,
///                              (to be used for adjusting marker positions to other samplerates)
/// \returns whether operation succeeded.
auto kaze_ma_dr_wav_get_markers(
    const String &filepath,
    List<AudioMarker> *outMarkers,
    int *outFreq
) -> Bool;

/// Get marker data from a WAV file in const memory
/// \param[in]   mem         memory of WAV file
/// \param[out]  outMarkers  receives list of markers
/// \param[out]  outFreq     receives sample rate frequency of WAV file;
///                              (can be used for adjusting marker positions to other sample rates)
/// \returns whether operation succeeded.
auto kaze_ma_dr_wav_get_markers_mem(
    MemView<const void> mem,
    List<AudioMarker> *outMarkers,
    int *outFreq
) -> Bool;

KSND_NS_END
