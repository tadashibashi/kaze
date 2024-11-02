#pragma once
#include <kaze/core/lib.h>
#include <kaze/core/traits.h>

#include <algorithm>
#include <ranges>

KAZE_NS_BEGIN

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
        Callback(funcptr_t<void(Args..., void *)> callback, void *userptr = nullptr, Int priority = 0) :
            m_callback(callback), m_userptr(userptr), m_priority(priority) { }

        [[nodiscard]]
        auto operator ==(const Callback &other) const noexcept -> Bool {
            return m_callback == other.m_callback && m_userptr == other.m_userptr;
        }

        [[nodiscard]]
        auto operator !=(const Callback &other) const noexcept -> Bool {
            return !operator==(other);
        }

        auto operator ()(Args... args) const -> void
        {
            KAZE_ASSERT(m_callback != nullptr);
            return m_callback(args..., m_userptr);
        }

        [[nodiscard]]
        auto priority() const noexcept -> Int { return m_priority; }

    private:
        funcptr_t<void(Args..., void *)> m_callback{}; ///< function pointer callback
        void *m_userptr;                           ///< user pointer for context and signature
        Int m_priority;                            ///< lower numbers come first, higher numbers later
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

        sortByPriority();

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

        sortByPriority();

        if (!m_callbacks.empty())
        {
            m_isCalling = true;
            for (auto &callback : std::views::reverse(m_callbacks))
                callback(args...);
            m_isCalling = false;
        }

        processCommands();
    }

    auto add(funcptr_t<void(Args..., void *)> func, void *userptr = nullptr, Int priority = 0)
    {
        if (m_isCalling)
        {
            // Defer the addition of the callback
            m_commands.emplace_back(Command::Add, Callback(func, userptr, priority));
        }
        else
        {
            // Add the callback now
            m_callbacks.emplace_back(func, userptr, priority);
            m_wasAdded = true;
        }
    }

    auto add(funcptr_t<void(Args..., void *)> func, Int priority)
    {
        add(func, nullptr, priority);
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
                return True;
        }
        return False;
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
                m_wasAdded = True;
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

        sortByPriority();
        m_commands.clear();
    }

    auto sortByPriority() -> void
    {
        if (m_wasAdded)
        {
            std::stable_sort(m_callbacks.begin(), m_callbacks.end(), [](const Callback &a, const Callback &b) {
                return a.priority() < b.priority();
            });

            m_wasAdded = False;
        }
    }

    List<Callback> m_callbacks;
    List<Command> m_commands;
    Bool m_isCalling;
    Bool m_wasAdded;
};

KAZE_NS_END
