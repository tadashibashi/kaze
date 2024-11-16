#pragma once

#include <kaze/audio/AudioDevice.h>
#include <kaze/audio/lib.h>

KAUDIO_NS_BEGIN

class PortAudioDevice : public AudioDevice {
public:
    PortAudioDevice();
    ~PortAudioDevice() override;

    auto open(const AudioDeviceOpen &config) -> Bool override;
    auto close() -> void override;
    auto suspend() -> void override;
    auto resume() -> void override;

    [[nodiscard]] auto isOpen() const -> Bool override;
    [[nodiscard]] auto isRunning() const -> Bool override;
    [[nodiscard]] auto getId() const -> Uint override;
    [[nodiscard]] auto getSpec() const -> const AudioSpec & override;
    [[nodiscard]] auto getBufferSize() const -> Int override;
    [[nodiscard]] auto getDefaultSampleRate() const -> Int override;

    void update() override;
private:
    struct Impl;
    Impl *m;
};

KAUDIO_NS_END
