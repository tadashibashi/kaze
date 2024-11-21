#include "AudioCommands.h"

#include "sources/AudioBus.h"
#include "AudioEffect.h"
#include "AudioSource.h"

#include <kaze/core/debug.h>
#include <kaze/core/traits.h>

// Contains the functions for each command

KSND_NS_BEGIN
auto commands::ContextFlagRemovals::operator()() -> void
{
    context->m_removeSourceFlag = True;
}

auto commands::EffectSetParameter::operator()() -> void
{
    using Param = AudioEffect::Param;
    effect->receiveParam(paramIndex, Param(value));
}

auto commands::SourceSetPause::operator()() -> void
{
    source->setPauseImpl(True, clock, releaseOnPause);
}

auto commands::SourceSetUnpause::operator()() -> void
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

KSND_NS_END
