#include "AudioEffect.h"
#include <kaze/snd/AudioCommands.h>

#include <kaze/core/errors.h>

KSND_NS_BEGIN

AudioEffect::AudioEffect(AudioEffect &&other) noexcept : m_context(other.m_context)
{}

auto AudioEffect::sendParam(const Int index, const Param::Type value) -> Bool
{
    KAZE_HANDLE_GUARD_RET(False);

    m_context->pushCommand(commands::EffectSetParameter {
        .effect = this,
        .paramIndex = index,
        .value = value,
    });

    return True;
}

KSND_NS_END
