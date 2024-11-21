#pragma once
#include <kaze/snd/lib.h>
#include <kaze/snd/AudioCommands.h>
#include <kaze/snd/AudioDevice.h>

#include <kaze/core/AlignedList.h>
#include <kaze/core/CommandQueue.h>
#include <kaze/core/MultiPool.h>
#include <kaze/core/debug.h>

KSND_NS_BEGIN
class AudioBus;

/// Private-facing shared context by Audio-related objects.
/// Audio* objects provide the public-facing interface.
class AudioContext {
public:
    AudioContext();
    ~AudioContext();

    KAZE_NO_COPY(AudioContext);

    // TODO: move ctor and assignment

    [[nodiscard]]
    auto isOpen() const -> Bool { return m_device && m_device->isOpen(); }

    template <PlainFunctor T> // must be a subtype of AudioCommand
    auto pushCommand(const T &command) -> void
    {
        m_deferredCmds.pushCommand(command);
    }

    template <PlainFunctor T> // must be a subtype of AudioCommand
    auto pushImmediateCommand(const T &command) -> void
    {
        m_immediateCmds.pushCommand(command);
    }

    /// Create a poolable object. AudioSources, AudioEffects, are the primary object
    /// types that are created via this function.
    template <Poolable T, typename... TArgs>
    [[nodiscard]]
    auto createObject(TArgs &&...args) -> Handle<T>
    {
        const auto lockGuard = std::lock_guard(m_mixMutex);
        return createObjectImpl<T>(std::forward<TArgs>(args)...);
    }

    template <Poolable T, typename... TArgs>
    [[nodiscard]]
    auto createObjectImpl(TArgs &&...args) -> Handle<T>
    {
        return m_pool.allocate<T>(std::forward<TArgs>(args)...);
    }

    /// Release/destroy a poolable object back to the pool.
    template <typename T>
    auto releaseObject(Handle<T> handle) -> Bool
    {
        const auto lockGuard = std::lock_guard(m_mixMutex);
        return releaseObjectImpl(handle);
    }

    template <typename T>
    auto releaseObjectImpl(Handle<T> handle) -> Bool
    {
        return m_pool.deallocate(handle);
    }

    /// Release/destroy a poolable object back to the pool. Concrete overload.
    /// \param[in]  object   The Poolable object to release
    /// \returns whether the operation succeeded.
    template <typename T> requires (!std::is_abstract_v<T>)
    auto releaseObject(T *object) -> Bool
    {
        const auto lockGuard = std::lock_guard(m_mixMutex);
        Handle<T> handle;
        if ( !m_pool.tryFind(object, &handle) )
        {
            KAZE_PUSH_ERR(Error::MissingKeyErr,
                "Could not find handle for object of type {}",
                typeid(T).name());
            return False;
        }

        return m_pool.deallocate(handle);
    }

    /// Release/destroy a poolable object back to the pool. Abstract overload
    /// \note Far less efficient than the Handle<T> overload, but may be necessary in certain scenarios
    ///       where the Handle or specific concrete type is not available.
    /// \returns whether the operation succeeded
    template <typename T>
    auto releaseObject(T *object) -> Bool
    {
        const auto lockGuard = std::lock_guard(m_mixMutex);

        PoolID id;
        PoolBase *poolBase;
        if ( !m_pool.tryFindGeneric(object, &poolBase, &id, Null) )
        {
            return False;
        }

        return m_pool.deallocate(Handle<T>(id, poolBase));
    }

    /// \returns the current clock time in Hz =>
    ///          sample rate * seconds since context was init
    [[nodiscard]]
    auto getClock() const noexcept -> Uint64 { return m_clock; }

    /// Let the AudioContext know that a sub AudioSource from the master
    /// AudioBus was removed.
    auto flagRemoveSource() -> void;

    auto getMasterBus() -> Handle<AudioBus> { return m_masterBus; }
    auto getMasterBus() const -> Handle<const AudioBus> { return Handle<AudioBus>::makeConst(m_masterBus); }

    [[nodiscard]]
    auto getBufferSize() const -> Int;

    [[nodiscard]]
    auto getSpec() const -> const AudioSpec &;

    [[nodiscard]]
    auto getDeviceId() const -> Uint { return m_device->getId(); }
private:
    friend class AudioEngine; // TODO: put other "driver" classes here that needs to access driving features
    friend class commands::ContextFlagRemovals;

    static auto audioCallback(void *userptr, AlignedList<Ubyte, 16> *outBuffer) -> void;

    struct AudioContextOpen {
        Int frequency = 0;
        Int samples = 1024;
    };
    auto open(const AudioContextOpen &config) -> Bool;
    auto close() -> void;

    auto update() -> void;

    MultiPool m_pool{};
    CommandQueue<AudioCommand> m_deferredCmds{}, m_immediateCmds{};
    Handle<AudioBus> m_masterBus{};

    std::mutex m_mixMutex{};

    Uint64 m_clock{};
    AudioDevice *m_device{};

    Bool m_removeSourceFlag{}; ///< Lets us know a source from the master bus was removed
};

KSND_NS_END
