#include <kaze/core/debug.h>
#include <kaze/core/io/stream/Rstream.h>
#include <kaze/core/io/stream/Rstreamable.h>

#include "miniaudio_ext.h"

#define MA_ENABLE_ONLY_SPECIFIC_BACKENDS
#define MA_NO_DEVICE_IO
#define MA_NO_RESOURCE_MANAGER
#define MA_NO_NODE_GRAPH
#define MA_NO_THREADING
#define MA_NO_ENGINE
#define MA_NO_GENERATION
#define MA_NO_SPATIAL

#if !KAZE_SND_DECODE_WAV
#define MA_NO_WAV
#endif

#if !KAZE_SND_DECODE_FLAC
#define MA_NO_FLAC
#endif

#if !KAZE_SND_DECODE_MP3
#define MA_NO_MP3
#endif

#define MA_NO_VORBIS // we'll use the libvorbis backend implementation instead
#define MA_NO_OPUS

#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

#ifdef MA_NO_LIBVORBIS
#undef MA_NO_LIBVORBIS
#endif

KSND_NS_BEGIN

static size_t dr_wav_read_callback(void* pUserData, void* pBufferOut, size_t bytesToRead)
{
    const auto streamable = static_cast<KAZE_NS::Rstreamable *>(pUserData);
    return static_cast<size_t>(
        streamable->read(
            static_cast<uint8_t *>(pBufferOut),
            static_cast<int64_t>(bytesToRead)));
}

static ma_bool32 dr_wav_seek_callback(void* pUserData, int offset, ma_dr_wav_seek_origin origin)
{
    const auto streamable = static_cast<KAZE_NS::Rstreamable *>(pUserData);
    if (origin == ma_dr_wav_seek_origin_current)
    {
        offset += static_cast<int>(streamable->tell());
    }

    return streamable->seek(offset);
}

static auto kaze_ma_dr_wav_get_markers_impl(
    Rstreamable *stream,
    List<AudioMarker> *outMarkers,
    Int *outFreq) -> Bool
{
    ma_dr_wav wav;
    if (!ma_dr_wav_init_with_metadata(
        &wav,
        dr_wav_read_callback,
        dr_wav_seek_callback,
        stream,
        0,
        nullptr))
    {
        // silent fail, for attempts to use with differing file type
        return false;
    }

    // Collect marker metadata
    Map<Uint, AudioMarker> markers;
    try
    {
        const auto bytesPerFrame = wav.channels * (wav.bitsPerSample / CHAR_BIT);


        for (auto meta = wav.pMetadata, metaEnd = wav.pMetadata + wav.metadataCount;
            meta != metaEnd;
            ++meta)
        {
            switch(meta->type)
            {
                case ma_dr_wav_metadata_type_cue:
                {
                    const auto count = meta->data.cue.cuePointCount;
                    for (auto cue = meta->data.cue.pCuePoints, cueEnd = cue + count;
                        cue != cueEnd; ++cue)
                    {
                        markers[cue->id].position = cue->sampleByteOffset / bytesPerFrame;
                    }
                } break;

                case ma_dr_wav_metadata_type_list_label:
                {
                    const auto &[cuePointId, stringLength, pString] = meta->data.labelOrNote;
                    markers[cuePointId].label = std::string(pString, stringLength);
                } break;

                default:
                    break;
            }
        }
    }
    catch(const std::exception &e)
    {
        KAZE_PUSH_ERR(Error::StdExcept, "Error parsing WAV marker data: {}", e.what());
        ma_dr_wav_uninit(&wav);
        return False;
    }
    catch(...)
    {
        KAZE_PUSH_ERR(Error::Unknown,
            "an unknown exception was thrown while parsing WAV marker data");
        ma_dr_wav_uninit(&wav);
        return False;
    }

    // Return markers
    if (outMarkers)
    {
        outMarkers->clear();
        outMarkers->reserve(markers.size());
        for (auto &[id, marker] : markers)
        {
            outMarkers->emplace_back(marker);
        }
    }

    if (outFreq)
    {
        *outFreq = static_cast<int>(wav.fmt.sampleRate);
    }

    ma_dr_wav_uninit(&wav);
    return True;
}

auto kaze_ma_dr_wav_get_markers(const String &filepath,
    List<AudioMarker> *outMarkers,
    Int *outFreq) -> Bool
{
    Rstream stream;
    if (!stream.openFile(filepath))
    {
        KAZE_PUSH_ERR(Error::FileOpenErr,
            "cannot get marker data the file failed to open: \"{}\"",
            filepath);
        return False;
    }

    return kaze_ma_dr_wav_get_markers_impl(stream.stream(), outMarkers, outFreq);
}

auto kaze_ma_dr_wav_get_markers_mem(
    const MemView<const void> mem,
    List<AudioMarker> *outMarkers,
    Int *outFreq) -> Bool
{
    Rstream stream;
    if (!stream.openConstMem(mem))
    {
        KAZE_PUSH_ERR(Error::FileOpenErr,
            "cannot get marker data because the memory failed to open");
        return False;
    }

    return kaze_ma_dr_wav_get_markers_impl(stream.stream(), outMarkers, outFreq);
}

KSND_NS_END
