#include "AudioDecoder.h"
#include <kaze/core/io/stream/Rstreamable.h>
#include <kaze/core/math/mathf.h>
#include "extern/miniaudio/miniaudio_decoder_backends.h"

KAUDIO_NS_BEGIN

/// miniaudio decoder read callback
static ma_result ma_decoder_on_read_rstream(
    ma_decoder *decoder,
    void *bufferOut,
    const size_t bytesToRead,
    size_t *outBytesRead)
{
    // Read from the stream
    const auto stream = static_cast<Rstreamable *>(decoder->pUserData);
    const auto bytesRead = stream->read(bufferOut, bytesToRead);
    if (bytesRead < 0)
    {
        *outBytesRead = 0;
        return MA_ERROR;
    }

    *outBytesRead = static_cast<size_t>(bytesRead);
    return (bytesRead < bytesToRead) ? MA_AT_END : MA_SUCCESS;
}

/// miniaudio decoder seek callback
static ma_result ma_decoder_on_seek_rstream(
    ma_decoder *decoder,
    const int64_t offset,
    const ma_seek_origin origin)
{
    const auto stream = static_cast<Rstreamable *>(decoder->pUserData);

    SeekBase base;
    switch(origin)
    {
    case ma_seek_origin_start:   base = SeekBase::Begin; break;
    case ma_seek_origin_current: base = SeekBase::Current; break;
    case ma_seek_origin_end:     base = SeekBase::End; break;
    default:
        KAZE_CORE_WARN("Warning, invalid ma_seek_origin value passed to "
            "ma_decoder_on_seek_rstream callback; falling back to Begin");
        base = SeekBase::Begin;
        break;
    }

    return stream->seek(offset, base) ? MA_SUCCESS : MA_BAD_SEEK;
}

/// miniaudio custom backends list
static List<ma_decoding_backend_vtable *> customBackendVTables{};
static Bool initBackendVTables{}; ///< whether backends were initialized

AudioDecoder::AudioDecoder()
{
    if ( !initBackendVTables ) // setup backends once
    {
        // Add custom backends here
        customBackendVTables = {
#if KAZE_AUDIO_DECODE_VORBIS
            &g_ma_decoding_backend_vtable_libvorbis,
#endif
        };

        initBackendVTables = True;
    }
}

AudioDecoder::~AudioDecoder()
{

}

AudioDecoder::AudioDecoder(AudioDecoder &&other) noexcept :
    m_decoder(other.m_decoder), m_stream(std::move(other.m_stream)),
    m_pcmSize(other.m_pcmSize), m_looping(other.m_looping),
    m_spec(other.m_spec), m_targetSpec(other.m_targetSpec)
{
    other.m_decoder = nullptr;
}

auto AudioDecoder::operator=(AudioDecoder &&other) noexcept -> AudioDecoder &
{
    if (this == &other) return *this;

    if (m_decoder != nullptr)
    {
        ma_decoder_uninit(m_decoder);
        delete m_decoder;
    }

    m_decoder = other.m_decoder;
    m_stream = std::move(other.m_stream);
    m_looping = other.m_looping;
    m_spec = other.m_spec;
    m_targetSpec = other.m_targetSpec;
    m_pcmSize = other.m_pcmSize;

    other.m_decoder = nullptr;

    return *this;
}

auto AudioDecoder::openFile(const String &path, const AudioSpec &targetSpec, Bool inMemory) -> Bool
{
    if ( !m_stream.openFile(path, inMemory))
    {
        return False;
    }

    return postOpen(targetSpec);
}

auto AudioDecoder::openConstMem(MemView<void> mem, const AudioSpec &targetSpec) -> Bool
{
    if ( !m_stream.openConstMem(mem))
    {
        return False;
    }

    return postOpen(targetSpec);
}

auto AudioDecoder::openMem(const ManagedMem mem, const AudioSpec &targetSpec) -> Bool
{
    if ( !m_stream.openMem(mem) )
    {
        return False;
    }

    return postOpen(targetSpec);
}

// This helper handles setting up the decoder
auto AudioDecoder::postOpen(const AudioSpec &targetSpec) -> Bool
{
    // Allocate the decoder
    const auto decoder = new ma_decoder();

    // Set up the config
    auto config = ma_decoder_config_init(
        static_cast<ma_format>(targetSpec.format.toMaFormat()),
        targetSpec.channels,
        targetSpec.freq);

    if ( !customBackendVTables.empty() )
    {
        config.ppCustomBackendVTables = customBackendVTables.data();
        config.customBackendCount = customBackendVTables.size();
    }

    // Init the decoder
    if (const auto result = ma_decoder_init(
            ma_decoder_on_read_rstream,
            ma_decoder_on_seek_rstream,
            m_stream.stream(),
            &config,
            decoder);
        result != MA_SUCCESS)
    {
        KAZE_PUSH_ERR(Error::RuntimeErr, "ma_decoder_init error: {}",
            ma_result_description(result));
        delete decoder;
        return False;
    }

    // Get format data
    ma_format format;
    ma_uint32 channels;
    ma_uint32 samplerate;
    if (const auto result = ma_decoder_get_data_format(
            decoder, &format, &channels, &samplerate, nullptr, 0);
        result != MA_SUCCESS)
    {
        KAZE_PUSH_ERR(Error::RuntimeErr, "ma_decoder_get_data_format error: {}",
            ma_result_description(result));
        ma_decoder_uninit(decoder);
        delete decoder;
        return False;
    }

    // Done! Commit results.
    if (m_decoder != Null)
    {
        ma_decoder_uninit(m_decoder);
        delete m_decoder;
    }

    m_decoder = decoder;
    m_looping = False;
    m_spec.channels = static_cast<Int>(channels);
    m_spec.freq = static_cast<Int>(samplerate);
    m_spec.format = SampleFormat::fromMaFormat(format);
    m_targetSpec = targetSpec;
    m_pcmSize = -1LL;
    return True;
}

auto AudioDecoder::close() -> void
{
    if (m_decoder != Null)
    {
        ma_decoder_uninit(m_decoder);
        delete m_decoder;
        m_decoder = Null;
        m_pcmSize = -1LL;

        m_stream.close();
    }
}

auto AudioDecoder::isOpen() const -> Bool
{
    return m_decoder != Null;
}

auto AudioDecoder::readFrames(void *buffer, Int64 frames) -> Int64
{
    if ( !isOpen() )
    {
        KAZE_PUSH_ERR(Error::NotInitialized,
            "attempted to use AudioDecoder::readFrames in uninit state");
        return -1LL;
    }

    const auto frameLength = getPCMFrameLength();
    if (frameLength < 0)
    {
        return -1LL;
    }

    auto currentFrame = getCurrentPCMFrame();
    if (currentFrame < 0)
    {
        return -1LL;
    }

    ma_uint64 framesRead = 0;
    auto framesLeft = frameLength - currentFrame;
    if (framesLeft <= 0) // ended
    {
        if (!m_looping)
            return 0;
    }
    else
    {
        if (const auto result = ma_decoder_read_pcm_frames(
                m_decoder, buffer, mathf::min(framesLeft, frames), &framesRead);
            result != MA_SUCCESS && result != MA_AT_END)
        {
            KAZE_PUSH_ERR(Error::RuntimeErr,
                "ma_decoder_read_pcm_frames failed: {}",
                ma_result_description(result));
            return -1LL;
        }
    }

    if (m_looping)
    {
        while(True)
        {
            currentFrame = getCurrentPCMFrame();
            if (currentFrame < 0)
                return -1LL;

            framesLeft = frameLength - currentFrame;
            if (framesLeft == 0)
            {
                if (const auto result = ma_decoder_seek_to_pcm_frame(m_decoder, 0);
                    result != MA_SUCCESS)
                {
                    KAZE_PUSH_ERR(Error::RuntimeErr,
                        "ma_decoder_seek_to_pcm_frame failed: {}",
                        ma_result_description(result));
                    return static_cast<Int64>(framesRead);
                }
            }

            if (framesRead >= frames)
                break;
            framesRead += readFrames(
                (Ubyte *)buffer + framesRead,
                frames - static_cast<Int64>(framesRead));
        }
    }

    return static_cast<Int64>(framesRead);
}

auto AudioDecoder::read(void *buffer, const Int64 bytes) -> Int64
{
    const auto k = m_spec.format.bytes() * m_spec.channels;
    const auto frames = bytes / k;

    const auto framesRead = readFrames(buffer, frames);
    return (framesRead < 0) ? -1LL : framesRead * k;
}

auto AudioDecoder::tell(const AudioTime::Unit units) const -> Double
{
    if ( !isOpen() )
    {
        KAZE_PUSH_ERR(Error::NotInitialized,
            "attempted to use AudioDecoder::seek in uninit state");
        return False;
    }

    return AudioTime::convert(
        getCurrentPCMFrame(),
        AudioTime::PCMFrames,
        units,
        m_targetSpec
    );
}

auto AudioDecoder::seek(const Int64 position, const AudioTime::Unit units, const SeekBase base) -> Bool
{
    if ( !isOpen() )
    {
        KAZE_PUSH_ERR(Error::NotInitialized,
            "attempted to use AudioDecoder::seek in uninit state");
        return False;
    }

    // Get frame
    auto frame = mathf::round(
        AudioTime::convert(position, units, AudioTime::PCMFrames, m_targetSpec));

    // Apply seek base
    if (base == SeekBase::Current)
        frame += tell(AudioTime::PCMFrames);
    else if (base == SeekBase::End)
        frame += getPCMFrameLength();

    return setCurrentPCMFrame(frame);
}

auto AudioDecoder::getPCMFrameLength() const -> Int64
{
    if (m_pcmSize == -1LL)
    {
        ma_uint64 frames;
        if (const auto result = ma_decoder_get_length_in_pcm_frames(
                m_decoder, &frames);
            result != MA_SUCCESS)
        {
            KAZE_PUSH_ERR(Error::RuntimeErr,
                "ma_decoder_get_length_in_pcm_frames failed: {}",
                ma_result_description(result));
            return -1LL;
        }

        m_pcmSize = static_cast<Int64>(frames);
    }

    return m_pcmSize;
}

auto AudioDecoder::getCurrentPCMFrame() const -> Int64
{
    ma_uint64 frame;
    if (const auto result = ma_decoder_get_cursor_in_pcm_frames(
            m_decoder, &frame);
        result != MA_SUCCESS)
    {
        KAZE_PUSH_ERR(Error::RuntimeErr,
            "ma_decoder_get_cursor_in_pcm_frames failed: {}",
            ma_result_description(result));
        return -1LL;
    }

    return static_cast<Int64>(frame);
}

auto AudioDecoder::getAvailableFrames() const -> Int64
{
    const auto frameLength = getPCMFrameLength();
    if (frameLength < 0)
        return -1LL;

    const auto currentFrame = getCurrentPCMFrame();
    if (currentFrame < 0)
        return -1LL;

    return frameLength - currentFrame;
}

auto AudioDecoder::setCurrentPCMFrame(const Int64 frame) -> Bool
{
    // Seek to frame
    if (const auto result = ma_decoder_seek_to_pcm_frame(
            m_decoder, static_cast<ma_uint64>(frame));
        result != MA_SUCCESS)
    {
        KAZE_PUSH_ERR(Error::RuntimeErr, "ma_decoder_seek_to_pcm_frame failed: {}",
            ma_result_description(result));
        return False;
    }

    return True;
}

auto AudioDecoder::isEnded() const -> Bool
{
    if ( !isOpen() )
    {
        KAZE_PUSH_ERR(Error::NotInitialized,
            "attempted to call AudioDecoder::isEnded in uninit state");
        return False;
    }

    return m_stream.isEof() || getAvailableFrames() <= 0;
}

auto AudioDecoder::size() const -> Int64
{
    return m_stream.size();
}

KAUDIO_NS_END
