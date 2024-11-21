#pragma once
#include <kaze/snd/lib.h>
#include <kaze/snd/AudioDevice.h>

KSND_NS_BEGIN

class AAudioDevice : public AudioDevice {
public:
    AAudioDevice();
    ~AAudioDevice() override;

    auto open(const AudioDeviceOpen &config) -> Bool override;
    auto close() -> void override;
    auto suspend() -> void override;
    auto resume() -> void override;
    [[nodiscard]] auto isRunning() const -> Bool override;
    [[nodiscard]] auto getId() const -> Uint override;
    [[nodiscard]] auto getSpec() const -> const AudioSpec & override;
    [[nodiscard]] auto getBufferSize() const -> Int override;
    [[nodiscard]] auto isOpen() const -> Bool override;
    [[nodiscard]] auto getDefaultSampleRate() const -> Int override;
private:
    struct Impl;
    Impl *m;
};

KSND_NS_END
