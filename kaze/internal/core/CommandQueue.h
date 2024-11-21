#pragma once
#include <kaze/internal/audio/lib.h>
#include <kaze/internal/core/concepts.h>

KAZE_NS_BEGIN

template<typename T>
class CommandQueue;

/// Thread-safe command queue
/// \tparam TArgs  The types of commands this queue supports. These types are
///                added to an internal Variant list. Each command type must have a default function operator();
///                All command functions must not push additional commands to the queue or it
///                will cause a lock freeze.
template <PlainFunctor...TArgs>
class CommandQueue< Variant<TArgs...> >
{
public:
    /// Combined command variant
    using Command = Variant<TArgs...>;

    CommandQueue() : m_commands(), m_mutex() { }

    KAZE_NO_COPY(CommandQueue);

    auto processCommandsLocked() -> void
    {
        const auto lock = std::unique_lock(m_mutex);
        processCommands();
    }

    auto processCommands() -> void
    {
        for (auto &command : m_commands)
            command();
        m_commands.clear();
    }

    template<PlainFunctor T> requires std::disjunction_v<std::is_same<T, TArgs>...>
    auto pushCommand(const T &cmd) -> void
    {
        const auto lock = std::lock_guard(m_mutex);
        m_commands.emplace_back([cmd]() { const_cast<T &>(cmd)(); });
    }

    [[nodiscard]]
    auto lockGuard() { return std::lock_guard(m_mutex); }
private:
    List<std::function<void()>> m_commands{};
    std::mutex m_mutex{};
};

KAZE_NS_END
