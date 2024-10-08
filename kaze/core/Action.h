#pragma once
#ifndef kaze_core_action_h_
#define kaze_core_action_h_

#include <kaze/kaze.h>
#include <kaze/traits.h>

KAZE_NAMESPACE_BEGIN

template <typename... Args>
class Action {
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
        funcptr_t<void(Args..., void *)> func{};
        void *userptr;
    };
    struct Command {
        enum Type { Remove, Add };
        Command(Type type, Callback callback) : type(type), callback(callback) { }

        Type type;
        Callback callback;
    };
    List<Callback> m_callbacks;
    List<Command> m_commands;
    Bool m_isCalling;
public:
    Action() : m_callbacks(), m_commands(), m_isCalling() {}

    KAZE_NO_COPY(Action);

    auto operator()(Args... args) -> void
    {
        m_isCalling = true;
        for (const auto &func : m_callbacks)
            func(args...);
        m_isCalling = false;

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

    auto empty() const noexcept -> Bool {
        return m_callbacks.empty();
    }

    auto size() const noexcept -> Size {
        return m_callbacks.size();
    }

    auto clear() noexcept -> void {
        m_callbacks.clear();
    }

    /// Check if a callback exists in the container
    /// @param func function pointer
    /// @param userptr associated user data context pointer
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
};

KAZE_NAMESPACE_END

#endif // kaze_core_action_h_
