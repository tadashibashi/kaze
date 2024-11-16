#include "AudioCommands.h"

#include "sources/AudioBus.h"
#include "AudioEffect.h"
#include "AudioSource.h"

#include <kaze/core/debug.h>
#include <kaze/core/traits.h>

// Contains the functions for each command

KAUDIO_NS_BEGIN

auto commands::SetEffectParameter::operator()() -> void
{
    using Param = AudioEffect::Param;
    effect->receiveParam(paramIndex, Param(value));
}

auto commands::SetSourcePause::operator()() -> void
{
    source->setPauseImpl(True, clock, releaseOnPause);
}

auto commands::SetSourceUnpause::operator()() -> void
{
    source->setPauseImpl(False, clock, False);
}

auto commands::SourceAddEffect::operator()() -> void
{
    source->addEffectImpl(insertIndex, effect);
}

auto commands::SourceRemoveEffect::operator()() -> void
{
    source->removeEffectImpl(effect);
}

auto commands::SourceAddFadePoint::operator()() -> void
{
    source->addFadePointImpl(clock, value);
}

auto commands::SourceRemoveFadePoint::operator()() -> void
{
    source->removeFadePointImpl(beginClock, endClock);
}

auto commands::SourceFadeTo::operator()() -> void
{
    source->fadeToImpl(clock, value);
}

auto commands::BusConnectSource::operator()() -> void
{
    AudioBus::connectSourceImpl(bus, source);
}

auto commands::BusDisconnectSource::operator()() -> void
{
    AudioBus::disconnectSourceImpl(bus, source);
}

KAUDIO_NS_END
