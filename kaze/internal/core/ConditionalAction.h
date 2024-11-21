#pragma once
#include <kaze/internal/core/lib.h>
#include <algorithm>
#include <ranges>

KAZE_NS_BEGIN

/// Forwards events that others can subscribe to.
/// All callbacks take a user pointer, with optional priority.
/// Inspired by multi-cast delegates in C#.
template <typename... Args>
class ConditionalAction {
    class Callback {
    public:
        Callback(funcptr_t<Bool(Args..., void *)> callback, void *userptr = nullptr, Int priority = 0) :
            m_callback(callback), m_userptr(userptr), m_priority(priority) { }

        [[nodiscard]]
        auto operator ==(const Callback &other) const noexcept -> Bool {
            return other.m_callback == m_callback && other.m_userptr == m_userptr;
        }

        [[nodiscard]]
        auto operator !=(const Callback &other) const noexcept -> Bool {
            return !operator==(other);
        }

        auto operator ()(Args... args) const -> Bool
        {
            KAZE_ASSERT(m_callback != nullptr);
            return m_callback(args..., m_userptr);
        }

        auto priority() const noexcept -> Int { return m_priority; }

    private:
        funcptr_t<Bool(Args..., void *)> m_callback;
        void *m_userptr;
        Int m_priority;
    };

    struct Command {
        enum Type { Remove, Add };
        Command(Type type, Callback callback) : type(type), callback(callback) { }

        Type type;
        Callback callback;
    };

public:
    ConditionalAction() : m_callbacks(), m_commands(), m_isCalling(), m_wasAdded() { }

    KAZE_NO_COPY(ConditionalAction);

    auto operator()(Args... args) -> Bool
    {
        if (m_isCalling) return False;

        sortByPriority();

        if ( !m_callbacks.empty() )
        {
            m_isCalling = true;
            for (const auto &callback : m_callbacks)
            {
                if ( !callback(args...) )
                {
                    m_isCalling = false;
                    return False;
                }
            }
            m_isCalling = false;
        }

        processCommands();
        return True;
    }

    /// Call the callbacks in reverse order
    auto reverseInvoke(Args... args) -> Bool
    {
        if (m_isCalling) return False;

        sortByPriority();

        if (!m_callbacks.empty())
        {
            m_isCalling = true;
            for (auto &callback : std::views::reverse(m_callbacks))
            {
                if ( !callback(args...) )
                    return False;
            }
            m_isCalling = false;
        }

        processCommands();
        return True;
    }

    auto add(funcptr_t<Bool(Args..., void *)> func, void *userptr = nullptr, Int priority = 0)
    {
        KAZE_ASSERT(func != nullptr, "added callback must not be null");
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

    auto add(funcptr_t<Bool(Args..., void *)> func, Int priority)
    {
        add(func, nullptr, priority);
    }

    auto remove(funcptr_t<Bool(Args..., void *)> func, void *userptr = nullptr)
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
    auto contains(funcptr_t<Bool(Args..., void *)> func, void *userptr = nullptr) noexcept -> Bool
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
