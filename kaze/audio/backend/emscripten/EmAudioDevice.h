#pragma once
#include <kaze/audio/AudioDevice.h>
#include <kaze/audio/lib.h>

KAUDIO_NS_BEGIN

class EmAudioDevice final : public AudioDevice {
public:
    EmAudioDevice();
    ~EmAudioDevice() override;

    auto open(const AudioDeviceOpen &config) -> Bool override;
    auto close() -> void override;
    auto suspend() -> void override;
    auto resume() -> void override;
    [[nodiscard]] auto isRunning() const -> Bool override;
    [[nodiscard]] auto getId() const -> Uint override;
    [[nodiscard]] auto getSpec() const -> const AudioSpec & override;
    [[nodiscard]] auto getBufferSize() const -> Int override;
    [[nodiscard]] auto getDefaultSampleRate() const -> Int override;

public: // EmAudioDevice-specific functions (somewhat private since the AudioEngine only has a pointer to an AudioDevice)

    void read(const Float **data, Int length);

    /// Convenience function checking if `SharedArrayBuffer` and `WebAudio` are available.
    /// This can only be called from the main thread.
    [[nodiscard]]
    static auto isPlatformSupported() -> Bool;

    [[nodiscard]]
    auto isOpen() const -> Bool override;
private:
    struct Impl;
    Impl *m;
};

KAUDIO_NS_END