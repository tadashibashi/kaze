#pragma once
#include <kaze/snd/lib.h>
#include <kaze/snd/AudioSource.h>

KSND_NS_BEGIN

class AudioBus final : public AudioSource {
public:
    AudioBus() = default;
    AudioBus(AudioBus &&other) noexcept;
    ~AudioBus() override = default;

    // pool lifetime functions
    auto init_(AudioContext *context, const Handle<AudioBus> &parent, Bool paused) -> Bool;
    auto release(Bool recursive = True) -> void;
    auto release_() -> void override;

    static auto connect(const Handle<AudioBus> &bus, const Handle<AudioSource> &source) -> Bool;

    static auto disconnect(const Handle<AudioBus> &bus, const Handle<AudioSource> &source) -> Bool;

    [[nodiscard]]
    auto getOutputBus() -> Handle<AudioBus> { return m_parent; }

    [[nodiscard]]
    auto isMasterBus() const noexcept -> Bool { return m_isMaster; }

private:
    auto readImpl(Ubyte *output, Int64 length) -> Int64 override;

    friend class AudioContext;
    auto updateParentClock(Uint64 parentClock) -> Bool override;
    auto processRemovals() -> void; // only AudioContext, during AudioContext::update should call this

    // ----- Commands ---------------------------------------------------------
    friend commands::BusConnectSource;
    static auto connectSourceImpl(const Handle<AudioBus> &bus, const Handle<AudioSource> &source) -> void;

    friend commands::BusDisconnectSource;
    static auto disconnectSourceImpl(const Handle<AudioBus> &bus, const Handle<AudioSource> &source) -> void;

    // ----- Data members -----------------------------------------------------
    /// Sub-mix graph
    List< Handle<AudioSource> > m_sources{};

    /// Temp buffer to calculate mix
    AlignedList<Float, 16> m_buffer{};

    /// Output bus parent
    Handle<AudioBus> m_parent{};

    /// Whether this is the master bus or not
    Bool m_isMaster{};
};

KSND_NS_END
