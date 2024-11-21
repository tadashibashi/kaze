#pragma once
#include <kaze/internal/audio/lib.h>
#include <kaze/internal/audio/AudioSpec.h>
#include <kaze/internal/audio/AudioTime.h>

#include <kaze/internal/core/io/stream/Rstream.h>
#include <kaze/internal/core/io/stream/SeekBase.h>
#include <kaze/internal/core/MemView.h>
#include <kaze/internal/core/ManagedMem.h>
#include <kaze/internal/core/traits.h>

struct ma_decoder;

KAUDIO_NS_BEGIN

class AudioDecoder {
public:
    AudioDecoder();
    ~AudioDecoder();

    KAZE_NO_COPY(AudioDecoder);

    // This object is movable
    AudioDecoder(AudioDecoder &&other) noexcept;
    auto operator=(AudioDecoder &&other) noexcept -> AudioDecoder &;

    /// Open an audio file for streaming.
    /// \param[in]  path        filepath to open
    /// \param[in]  targetSpec  spec to convert the audio to
    /// \param[in]  inMemory    whether to stream file in-memory
    ///                             `true`:  copy file data entirely into RAM and stream from that memory;
    ///                                      this can use significant RAM, but is much faster. Best for short sfx.
    ///                             `false`: stream directly from file (default);
    ///                                      this is slower, but uses much less RAM. Best for music and ambiences.
    /// \returns whether open succeeded, and audio file type is supported.
    auto openFile(
        const String &path,
        const AudioSpec &targetSpec,
        Bool inMemory = False
    ) -> Bool;

    /// Open audio file loaded into memory for streaming. Memory must be valid and non-mutating for the duration
    /// that this is decoder uses it.
    /// \param[in]  mem         memory to stream
    /// \param[in]  targetSpec  target audio spec to convert to
    /// \returns whether open succeeded, and audio file type is supported.
    auto openConstMem(
        MemView<void> mem,
        const AudioSpec &targetSpec
    ) -> Bool;

    /// Open audio file loaded into memory for streaming. User hands responsibility for management over to the
    /// decoder, and closes when `close` is called, or this object is destroyed.
    /// \param[in]  mem         memory to stream
    /// \param[in]  targetSpec  target audio spec to convert to
    /// \param[in]  deallocator callback to free memory; defaults to `memory::free` if unspecified [optional]
    /// \returns whether open succeeded, and audio file type is supported.
    auto openMem(
        ManagedMem mem,
        const AudioSpec &targetSpec
    ) -> Bool;

    /// Close the stream
    auto close() -> void;

    /// \returns whether the decoder has an open audio stream.
    [[nodiscard]]
    auto isOpen() const -> Bool;

    /// \param[in] units  unit type to get the current position of (native units is AudioTime::PCMFrames)
    /// \returns the current position
    [[nodiscard]]
    auto tell(AudioTime::Unit units) const -> Double;

    /// Read pcm frames into a buffer. Make sure to check target spec for details on sample size, channels, etc.
    /// \param[in] buffer   buffer to fill
    /// \param[in] frames   number of pcm frames to read
    /// \returns actual number of pcm frames read
    auto readFrames(void *buffer, Int64 frames) -> Int64;

    /// Raw read in byte count. Make sure to check target spec for details on sample size, channels, etc.
    /// \param[in] buffer  buffer to fill
    /// \param[in] bytes   number of bytes to read
    /// \returns actual number of bytes read into `buffer`
    auto read(void *buffer, Int64 bytes) -> Int64;

    /// Seek to a position in the audio stream
    /// \param[in]  position   logical offset
    /// \param[in]  units      units of `position` to offset
    /// \param[in]  base       location from which to set `position`
    /// \returns whether seek was successful.
    auto seek(
        Int64 position,
        AudioTime::Unit units,
        SeekBase base = SeekBase::Begin
    ) -> Bool;

    /// Set decoder to loop or stop on end.
    /// \param[in] looping  Whether decoder should loop:
    ///                     if `true`: decoder loops seamlessly at end;
    ///                     if `false`: decoder stops at end (default)
    auto setLooping(const Bool looping) -> void { m_looping = looping; }

    /// \returns whether decoder is in looping mode:
    ///              if `true`: decoder is set to loop seamlessly at end;
    ///              if `false`: decoder is set to stop at end (default)
    [[nodiscard]]
    auto isLooping() const -> Bool { return m_looping; }

    /// \returns the target spec that the data is being converted to.
    [[nodiscard]]
    auto getTargetSpec() const -> const AudioSpec & { return m_targetSpec; }

    /// \returns the audio spec of the source audio file data.
    [[nodiscard]]
    auto getSpec() const -> const AudioSpec & { return m_spec; }

    /// \returns whether the audio decoder has ended.
    ///          The will only be set when this decoder is in non-looping mode.
    [[nodiscard]]
    auto isEnded() const -> Bool;

    [[nodiscard]]
    auto size() const -> Int64;
private:
    [[nodiscard]]
    auto getCurrentPCMFrame() const -> Int64;
    [[nodiscard]]
    auto getPCMFrameLength() const -> Int64;
    [[nodiscard]]
    auto getAvailableFrames() const -> Int64;

    /// Seek to a pcm frame
    /// \param[in] frame frame to set
    /// \returns whether seek operation succeeded.
    auto setCurrentPCMFrame(Int64 frame) -> Bool;


    auto postOpen(const AudioSpec &targetSpec) -> Bool;

    AudioSpec m_spec{}, m_targetSpec{};
    ma_decoder *m_decoder{};
    Rstream m_stream{};
    Bool m_looping{};
    mutable Int64 m_pcmSize{-1LL};
};

KAUDIO_NS_END
