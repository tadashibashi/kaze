#pragma once
#include <kaze/snd/lib.h>
#include <kaze/snd/AudioDevice.h>

KSND_NS_BEGIN

class iOSAudioDevice : public AudioDevice {
public:
    iOSAudioDevice();
    ~iOSAudioDevice() override;

    auto open(const AudioDeviceOpen &config) -> bool override;
    auto close() -> void override;
    auto suspend() -> void override;
    auto resume() -> void override;
    [[nodiscard]] auto isRunning() const -> Bool override;
    [[nodiscard]] auto getId() const -> Uint override;
    [[nodiscard]] auto getSpec() const -> const AudioSpec & override;
    [[nodiscard]] auto getBufferSize() const -> Int override;
    [[nodiscard]] auto getDefaultSampleRate() const -> Int override;
    [[nodiscard]] auto isOpen() const -> Bool override;
private:
    struct Impl;
    Impl *m;
};

KSND_NS_END
