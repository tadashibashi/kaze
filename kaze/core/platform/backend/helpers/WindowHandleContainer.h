/// \file WindowHandleContainer.h
/// Usable by backends to conveniently manage multiple windows
#pragma once
#include <kaze/core/lib.h>
#include <kaze/core/debug.h>
#include <kaze/core/platform/backend/backend.h>

#include <mutex>

KAZE_NS_BEGIN

namespace backend
{
    /// @tparam T type of associated userdata
    template <typename T>
    class WindowHandleContainer {
        std::mutex m_windowMutex{};
        Dictionary<WindowHandle, T> m_data{};
        List<WindowHandle> m_windows;
        WindowHandle m_mainWindow{};
    public:
        /// Add a window to the container
        /// \param[in] window window to add
        /// \param[in] data   associated data to emplace
        /// \returns whether the operation was successful.
        bool emplace(WindowHandle window, T &&data) noexcept;

        /// Check if the container contains a window handle
        /// \param[in]  window   window to check for
        /// \param[out] outContains whether the container has the window inside
        /// \returns whether the retrieval was successful.
        bool contains(WindowHandle window, bool *outContains) noexcept;

        /// Erase a window from the container
        /// \param[in]  window       the handle to remove from the s_windows set
        /// \param[out] outWasErased retrieves whether the WindowHandle was erased from the container
        /// \returns whether retrieval succeeded.
        bool erase(WindowHandle window, bool *outWasErased = nullptr) noexcept;

        /// Get data associated with a window
        /// \param[in]  window
        /// \param[out] outData   retrieves the data pointer associated with the window, or `nullptr` if the window
        ///                        does not exist in this container.
        /// \returns whether retrieval succeeded.
        bool getData(WindowHandle window, T **outData) noexcept;

        [[nodiscard]]
        auto begin() noexcept { return m_windows.begin(); }
        [[nodiscard]]
        auto end() noexcept { return m_windows.end(); }

        auto data() const noexcept -> const Dictionary<WindowHandle, T> & { return m_data; }
        auto data() noexcept -> Dictionary<WindowHandle, T> & { return m_data; }

        [[nodiscard]]
        auto begin() const noexcept { return m_windows.cbegin(); }
        [[nodiscard]]
        auto end() const noexcept { return m_windows.cend(); }

        [[nodiscard]]
        auto size() const noexcept -> Size;
        [[nodiscard]]
        auto empty() const noexcept -> bool;
        [[nodiscard]]
        auto getMainWindow() -> WindowHandle;
    };

    template <typename T>
    bool WindowHandleContainer<T>::emplace(const WindowHandle window, T &&data) noexcept
    {
        std::lock_guard lockGuard(m_windowMutex);
        try
        {
            auto it = m_data.find(window);
            if (it == m_data.end())
            {
                if (m_data.empty())
                    m_mainWindow = window;
                m_data[window] = std::move(data);
                m_windows.emplace_back(window);
            }
            else
            {
                KAZE_PUSH_ERR(Error::BE_LogicError, "A duplicate window was passed to WindowHandleContainer<T>::emplace. Action was prevented.");
            }

            return true;
        }
        catch(const std::exception &e)
        {
            KAZE_PUSH_ERR(Error::StdExcept, "Error thrown while adding WindowHandle to internal HashSet: {}", e.what());
            return false;
        }
        catch(...)
        {
            KAZE_PUSH_ERR(Error::Unknown, "Unknown error occurred while adding WindowHandle to internal HashSet");
            return false;
        }
    }

    template <typename T>
    auto WindowHandleContainer<T>::contains(const WindowHandle window, bool *outContains) noexcept -> bool
    {
        std::lock_guard lockGuard(m_windowMutex);
        try
        {
            if (outContains)
                *outContains = m_data.contains(window);
            return true;
        }
        catch(const std::exception &e)
        {
            KAZE_PUSH_ERR(Error::StdExcept, "Exception occurred while checking WindowHandle validity: {}", e.what());
            return false;
        }
        catch(...)
        {
            KAZE_PUSH_ERR(Error::Unknown, "Unknown error was thrown while checking WindowHandle validity");
            return false;
        }
    }

    template <typename T>
    bool WindowHandleContainer<T>::erase(const WindowHandle window, bool *outWasErased) noexcept
    {
        std::lock_guard lockGuard(m_windowMutex);
        try
        {
            const auto wasErased = m_data.erase(window) > 0;
            if (outWasErased) // report whether a window was actually removed form the container
                *outWasErased = wasErased;

            if (window == m_mainWindow && !m_data.empty()) // replace main window if it just got closed
                m_mainWindow = m_data.begin()->first;

            if (const auto it = std::find(m_windows.begin(), m_windows.end(), window);
                it != m_windows.end())
            {
                m_windows.erase(it);
            }

            return true;
        }
        catch(const std::exception &e)
        {
            KAZE_PUSH_ERR(Error::StdExcept, "Exception occurred while erasing window: {}", e.what());
            if (outWasErased)
                *outWasErased = false;
            return false;
        }
        catch(...)
        {
            KAZE_PUSH_ERR(Error::Unknown, "Unknown exception occurred while erasing window");
            if (outWasErased)
                *outWasErased = false;
            return false;
        }
    }

    template <typename T>
    bool WindowHandleContainer<T>::getData(const WindowHandle window, T **outData) noexcept
    {
        try
        {
            auto it = m_data.find(window);
            if (it == m_data.end())
            {
                if (outData)
                    *outData = nullptr;
            }
            else
            {
                if (outData)
                    *outData = &it->second;
            }
            return true;
        }
        catch(const std::exception &e)
        {
            KAZE_PUSH_ERR(Error::StdExcept, "Exception occurred while fetching window data: {}", e.what());
            return false;
        }
        catch(...)
        {
            KAZE_PUSH_ERR(Error::Unknown, "Unknown error occurred while fetching window data");
            return false;
        }
    }

    template <typename T>
    auto WindowHandleContainer<T>::size() const noexcept -> Size
    {
        return m_data.size();
    }

    template <typename T>
    auto WindowHandleContainer<T>::empty() const noexcept -> bool
    {
        return m_data.empty();;
    }

    template <typename T>
    auto WindowHandleContainer<T>::getMainWindow() -> WindowHandle
    {
        return m_mainWindow;;
    }
}

KAZE_NS_END
