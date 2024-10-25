#pragma once
#include <kaze/core/lib.h>
#include <kaze/core/traits.h>

#include <ranges>

KAZE_NAMESPACE_BEGIN

/// @description
/// Container of callbacks with no return value.
/// (Inspired by and similar to multi-cast Action delegates in C#)
///
/// \note
/// Each callback contains a user pointer useful for providing context to the callback. This means that you
/// must add a `void *` parameter at the end of each callback function subscribing to the event.
/// For example an `Action<int>` must have callbacks with this signature `void(int, void *)`.
///
/// @tparam  Args... types of parameters for the callbacks
template <typename... Args>
class Action {
    /// Individual callback container
    class Callback {
    public:
        Callback(funcptr_t<void(Args..., void *)> func, void *userptr) : func(func), userptr(userptr) {}

        auto operator ==(const Callback &other) const noexcept -> Bool {
            return func == other.func && userptr == other.userptr;
        }

        auto operator !=(const Callback &other) const noexcept -> Bool {
            return !operator==(other);
        }

        auto operator ()(Args... args) const -> void
        {
            KAZE_ASSERT(func != nullptr);
            return func(args..., userptr);
        }

    private:
        funcptr_t<void(Args..., void *)> func{}; ///< function pointer callback
        void *userptr;                           ///< user pointer for context and signature
    };

    struct Command {
        enum Type { Remove, Add };
        Command(Type type, Callback callback) : type(type), callback(callback) { }

        Type type;
        Callback callback;
    };

public:
    Action() : m_callbacks(), m_commands(), m_isCalling() {}

    KAZE_NO_COPY(Action);

    auto operator()(Args... args) -> void
    {
        if (m_isCalling) return;

        if ( !m_callbacks.empty() )
        {
            m_isCalling = true;
            for (const auto &callback : m_callbacks)
                callback(args...);
            m_isCalling = false;
        }

        processCommands();
    }

    /// Call the callbacks in reverse order
    auto reverseInvoke(Args... args) -> void
    {
        if (m_isCalling) return;

        if (!m_callbacks.empty())
        {
            m_isCalling = true;
            for (auto &callback : std::views::reverse(m_callbacks))
                callback(args...);
            m_isCalling = false;
        }

        processCommands();
    }

    auto add(funcptr_t<void(Args..., void *)> func, void *userptr = nullptr)
    {
        if (m_isCalling)
        {
            // Defer the addition of the callback
            m_commands.emplace_back(Command::Add, Callback(func, userptr));
        }
        else
        {
            // Add the callback now
            m_callbacks.emplace_back(func, userptr);
        }
    }

    auto remove(funcptr_t<void(Args..., void *)> func, void *userptr = nullptr)
    {
        const auto callback = Callback(func, userptr);
        if (m_isCalling)
        {
            // Defer the removal of the callback
            m_commands.emplace_back(Command::Remove, callback);
        }
        else
        {
            // Remove the callback now
            for (auto it = m_callbacks.begin(); it != m_callbacks.end(); ++it)
            {
                if (*it == callback)
                {
                    m_callbacks.erase(it);
                    break;
                }
            }
        }
    }

    /// Whether there are no callbacks in the container
    auto empty() const noexcept -> Bool
    {
        return m_callbacks.empty();
    }

    /// Number of callbacks in the container
    auto size() const noexcept -> Size
    {
        return m_callbacks.size();
    }

    /// Clear the container of all callbacks
    auto clear() noexcept -> void
    {
        m_callbacks.clear();
    }

    /// Check if a callback exists in the container
    /// \param[in] func function pointer
    /// \param[in] userptr associated user data context pointer
    /// @return whether callback exists in container or not
    auto contains(funcptr_t<void(Args..., void *)> func, void *userptr = nullptr) noexcept -> Bool
    {
        const auto target = Callback(func, userptr);
        for (const auto &callback : m_callbacks)
        {
            if (callback == target)
                return KAZE_TRUE;
        }
        return KAZE_FALSE;
    }

private:
    auto processCommands() -> void
    {
        if (m_commands.empty()) return;

        for (auto &[type, callback] : m_commands)
        {
            if (type == Command::Add)
            {
                m_callbacks.emplace_back(callback);
            }
            else if (type == Command::Remove)
            {
                for (auto it = m_callbacks.begin(); it != m_callbacks.end(); ++it)
                {
                    if (*it == callback)
                    {
                        m_callbacks.erase(it);
                        break;
                    }
                }
            }
        }

        m_commands.clear();
    }

    List<Callback> m_callbacks;
    List<Command> m_commands;
    Bool m_isCalling;
};

KAZE_NAMESPACE_END
