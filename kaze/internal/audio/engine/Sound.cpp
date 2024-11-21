#include "Sound.h"
#include <kaze/internal/audio/engine/AudioContext.h>

#include <kaze/internal/audio/conv/extern/miniaudio/miniaudio_ext.h>
#include <kaze/internal/core/io/io.h>
#include <kaze/internal/core/platform/defines.h>

#include "sources/AudioBus.h"
#include "sources/StreamSource.h"

KAUDIO_NS_BEGIN

// ===== WAV Marker retrieval code =====
static bool loadAudioMarkersImpl(
    void *userdata,
    bool(*getMarkersCallback)(void *, List<AudioMarker> *, int *),
    const AudioSpec &targetSpec,
    List<AudioMarker> *outMarkers)
{
    // Collect and return markers if out value provided
    if (outMarkers)
    {
        List<AudioMarker> markers;

        int freq = targetSpec.freq;
        getMarkersCallback(userdata, &markers, &freq);

        if (targetSpec.freq != freq)
        {
            const auto sizeFactor = (float)targetSpec.freq / (float)freq;
            for (auto &marker : markers)
            {
                marker.position = static_cast<Uint64>(std::round((float)marker.position * sizeFactor));
            }
        }

        outMarkers->swap(markers);
    }

    return True;
}

static auto getMarkersFilepath(void *userdata, List<AudioMarker> *outMarkers, Int *outFreq) -> Bool
{
    const auto &filepath = *static_cast<std::string *>(userdata);
    return kaze_ma_dr_wav_get_markers(filepath, outMarkers, outFreq);
}

static auto getMarkersConstMem(void *userdata, List<AudioMarker> *outMarkers, Int *outFreq) -> Bool
{
    const auto &mem = *static_cast<MemView<const void> *>(userdata);
    return kaze_ma_dr_wav_get_markers_mem(mem, outMarkers, outFreq);
}

static auto loadAudioMarkers(const String &filepath, const AudioSpec &targetSpec, List<AudioMarker> *outMarkers) -> Bool
{
    return loadAudioMarkersImpl((void *)&filepath, getMarkersFilepath, targetSpec, outMarkers);
}

static auto loadAudioMarkers(
    MemView<void> mem,
    const AudioSpec &targetSpec,
    List<AudioMarker> *outMarkers) -> Bool
{
    return loadAudioMarkersImpl(&mem, getMarkersConstMem, targetSpec, outMarkers);
}


// ===== Sound::Impl =====
struct Sound::Impl {
    List<AudioMarker> markers{};
    Variant< ManagedMem, MemView<void>, String > data{};
    AudioSpec targetSpec{};
    InitFlags flags{};
    Bool isOpen{};

    enum class Type : size_t {
        ManagedMem,
        MemView,
        StreamPath
    };

    ~Impl()
    {
        if (isOpen)
        {
            if ((Impl::Type)data.index() == Impl::Type::ManagedMem)
            {
                std::get<ManagedMem>(data).release();
            }
        }
    }
};

#if KAZE_DEBUG
#define SOUND_INIT_GUARD(ret) do { if (!m->isOpen) { \
    KAZE_PUSH_ERR(Error::NotInitialized, "attempted to access uninitialized sound in {}", __FUNCTION__); \
    return (ret); \
} } while(0)
#else
#define SOUND_INIT_GUARD(ret) KAZE_NOOP
#endif

Sound::Sound() : m(new Impl)
{ }

Sound::~Sound()
{
    delete m;
}

Sound::Sound(Sound &&other) noexcept : m(other.m)
{
    other.m = nullptr;
}

auto Sound::openFile(const String &filename, InitFlags flags, const AudioSpec &targetSpec) -> Bool
{
    KAZE_HANDLE_GUARD_RET(False);

    // Grab marker data (currently only applies if it is a WAV file)
    List<AudioMarker> markers;
    if (!loadAudioMarkers(filename, targetSpec, &markers))
    {
        KAZE_PUSH_ERR(Error::RuntimeErr, "failed to load markers");
        return False;
    }

#if KAZE_PLATFORM_EMSCRIPTEN
    // force in-memory streams in Emscripten builds, since it does not support streaming via the virtual FS.
    flags |= Sound::InMemory;
#endif
    if (flags & InMemory)
    {
        Ubyte *fileData;
        Size fileSize;
        if ( !file::load(filename, &fileData, &fileSize) )
        {
            return False;
        }

        m->data = ManagedMem(fileData, fileSize);
    }
    else
    {
        m->data = filename;
    }

    m->targetSpec = targetSpec;
    m->flags = flags;
    m->markers.swap(markers);
    m->isOpen = True;
    return True;
}

auto Sound::openMem(
    const ManagedMem mem,
    InitFlags flags,
    const AudioSpec &targetSpec) -> Bool
{
    KAZE_HANDLE_GUARD_RET(False);

    List<AudioMarker> markers;
    if (const auto result = loadAudioMarkers(
            {mem.data(), mem.size()},
            targetSpec,
            &markers);
        result != True)
    {
        KAZE_PUSH_ERR(Error::RuntimeErr, "failed to load markers");
        return False;
    }

    flags |= InitFlags::InMemory; // this is in-memory data

    m->data = mem;
    m->targetSpec = targetSpec;
    m->flags = flags;
    m->markers.swap(markers);
    m->isOpen = True;

    return True;
}

auto Sound::openConstMem(const MemView<void> mem, InitFlags flags, const AudioSpec &targetSpec) -> Bool
{
    KAZE_HANDLE_GUARD_RET(False);

    List<AudioMarker> markers;
    if (!loadAudioMarkers(mem, targetSpec, &markers))
    {
        KAZE_PUSH_ERR(Error::RuntimeErr, "failed to load markers");
        return False;
    }

    flags |= Sound::InMemory;  // this is in-memory data

    m->data = mem;
    m->targetSpec = targetSpec;
    m->flags = flags;
    m->markers.swap(markers);
    m->isOpen = True;
    return True;
}

auto Sound::addMarker(const Double position, const AudioTime::Unit units, const String &label) -> Bool
{
    KAZE_HANDLE_GUARD_RET(False);
    SOUND_INIT_GUARD(False);

    const auto framePos = static_cast<Uint64>(
        std::round(AudioTime::convert(position, units, AudioTime::PCMFrames, m->targetSpec)));

    AudioMarker marker(label, framePos);

    // find position to insert
    bool wasAdded = false;
    for (auto it = m->markers.begin(), end = m->markers.end(); it != end; ++it)
    {
        if (it->position > framePos)
        {
            m->markers.insert(it, marker);
            wasAdded = true;
            break;
        }
    }

    if (!wasAdded) // push to last position
        m->markers.emplace_back(marker);

    return True;
}

auto Sound::getMarker(Size index) const -> AudioMarker
{
    KAZE_HANDLE_GUARD_RET(AudioMarker{});
    SOUND_INIT_GUARD(AudioMarker{});

    if (index >= m->markers.size())
    {
        KAZE_PUSH_ERR(Error::OutOfRange, "Marker index is out of range");
        return {};
    }

    return m->markers[index];
}

auto Sound::getMarkerCount() const -> Size
{
    KAZE_HANDLE_GUARD_RET(0);
    SOUND_INIT_GUARD(0);
    return m->markers.size();
}

auto Sound::getSpec() const -> AudioSpec
{
    KAZE_HANDLE_GUARD_RET(AudioSpec{});
    SOUND_INIT_GUARD(AudioSpec{});
    return m->targetSpec;
}


auto Sound::init_() -> Bool
{
    m->data = MemView<void>{};
    m->flags = InitFlags::None;
    m->isOpen = False;
    m->targetSpec = {};
    
    return True;
}

void Sound::release_()
{
    if (m->isOpen)
    {
        if ((Impl::Type)m->data.index() == Impl::Type::ManagedMem)
        {
            std::get<ManagedMem>(m->data).release();
        }
    }

    m->flags = InitFlags::None;
    m->markers.clear();
    m->isOpen = False;
    m->targetSpec = {};
}

auto Sound::isOpen() const -> Bool
{
    return m->isOpen;
}

auto Sound::instantiate(AudioContext *context, Bool paused, Handle<AudioBus> bus, Handle<AudioSource> *outSource) -> Bool
{
    KAZE_HANDLE_GUARD_RET(False);
    SOUND_INIT_GUARD(False);

    if (!bus)
    {
        bus = context->getMasterBus();
    }

    if (!bus.isValid())
    {
        KAZE_PUSH_ERR(Error::RuntimeErr, "invalid bus");
        return False;
    }

    const Uint64 parentClock = bus->getClock();
    const Bool looping = m->flags & Sound::Looping;
    const Bool oneShot = m->flags & Sound::OneShot;
    const Bool inMemory = m->flags & Sound::InMemory;

    Variant< ManagedMem, MemView<void>, String > data;
    if (m->data.index() == 0)
    {
        const auto mem = std::get<ManagedMem>(m->data);
        data = MemView<void>(mem.data(), mem.size());
    }
    else
    {
        data = m->data;
    }

    const Handle<AudioSource> source = context->createObjectImpl<StreamSource>(
        StreamSourceInit {
            .context = context,
            .pathOrMemory = data,
            .parentClock = parentClock,
            .paused = paused,
            .isLooping = looping,
            .isOneShot = oneShot,
            .inMemory = inMemory,
        }
    ).cast<AudioSource>();

    if ( !source )
        return False;

    context->pushImmediateCommand(
        commands::BusConnectSource(bus, source));

    if (outSource)
        *outSource = source.cast<AudioSource>();

    return True;
}

KAUDIO_NS_END
