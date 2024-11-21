#pragma once
#include "AudioSpec.h"

#include <kaze/internal/audio/lib.h>
#include <kaze/internal/core/traits.h>
#include <kaze/internal/core/AlignedList.h>

KAUDIO_NS_BEGIN

using AudioCallback = funcptr_t<void(void *userdata, AlignedList<Uint8, 16> *buffer)>;

struct AudioDeviceOpen {
    Int frequency;               ///< Requested sample rate
    Int frameBufferSize;
    AudioCallback audioCallback;
    void *userdata;
};

/// Interface over an audio i/o backend
class AudioDevice {
public:
    virtual ~AudioDevice() = default;

    /// Create a platform-specific AudioDevice.
    /// Make sure to call `delete` on it when done with.
    static auto create() -> AudioDevice *;

    virtual auto open(const AudioDeviceOpen &config) -> Bool = 0;

    virtual auto close() -> void = 0;

    virtual auto suspend() -> void = 0;

    virtual auto resume() -> void = 0;

    virtual auto update() -> void {};

    [[nodiscard]]
    virtual auto getDefaultSampleRate() const -> Int = 0;

    /// \returns whether this device is currently open
    [[nodiscard]]
    virtual auto isOpen() const -> Bool = 0;

    /// \returns whether this device is running (not suspended)
    [[nodiscard]]
    virtual auto isRunning() const -> Bool = 0;

    /// \returns device id
    [[nodiscard]]
    virtual auto getId() const -> Uint = 0;

    [[nodiscard]]
    virtual auto getSpec() const -> const AudioSpec & = 0;

    [[nodiscard]]
    virtual auto getBufferSize() const -> Int = 0;
};

KAUDIO_NS_END
