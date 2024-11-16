#include "AudioEffect.h"
#include <kaze/audio/engine/AudioCommands.h>

#include <kaze/core/errors.h>

KAUDIO_NS_BEGIN

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

KAUDIO_NS_END
