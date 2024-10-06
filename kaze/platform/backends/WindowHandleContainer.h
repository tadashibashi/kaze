#pragma once
#ifndef kaze_platform_backend_windowhandlecontainer_h_
#define kaze_platform_backend_windowhandlecontainer_h_

#include <kaze/debug.h>
#include <kaze/kaze.h>
#include <kaze/platform/backend.h>

#include <mutex>

KAZE_NAMESPACE_BEGIN

namespace backend
{
    /// @tparam T type of associated userdata
    template <typename T>
    class WindowHandleContainer {
        std::mutex m_windowMutex{};
        Dictionary<WindowHandle, T> m_windows{};
    public:
        /// Add a window to the container
        /// @param window window to add
        /// @param data   associated data to emplace
        /// @returns whether the operation was successful.
        bool emplace(WindowHandle window, T &&data) noexcept;

        /// Check if the container contains a window handle
        /// @param [in]  window   window to check for
        /// @param [out] outContains whether the container has the window inside
        /// @returns whether the retrieval was successful.
        bool contains(WindowHandle window, bool *outContains) noexcept;

        /// Erase a window from the container
        /// @param [in]  window       the handle to remove from the s_windows set
        /// @param [out] outWasErased retrieves whether the WindowHandle was erased from the container
        /// @returns whether retrieval succeeded.
        bool erase(WindowHandle window, bool *outWasErased = nullptr) noexcept;

        /// Get data associated with a window
        /// @param [in]  window
        /// @param [out] outData   retrieves the data pointer associated with the window, or `nullptr` if the window
        ///                        does not exist in this container.
        /// @returns whether retrieval succeeded.
        bool getData(WindowHandle window, T **outData) noexcept;
    };

    template <typename T>
    bool WindowHandleContainer<T>::emplace(const WindowHandle window, T &&data) noexcept
    {
        std::lock_guard lockGuard(m_windowMutex);
        try
        {
            auto it = m_windows.find(window);
            if (it == m_windows.end())
            {
                m_windows[window] = std::move(data);
            }
            else
            {
                KAZE_CORE_ERRCODE(Error::BE_LogicError, "A duplicate window was passed to WindowHandleContainer<T>::emplace. Action was prevented.");
            }

            return true;
        }
        catch(const std::exception &e)
        {
            KAZE_CORE_ERRCODE(Error::StdExcept, "Error thrown while adding WindowHandle to internal HashSet: {}", e.what());
            return false;
        }
        catch(...)
        {
            KAZE_CORE_ERRCODE(Error::Unknown, "Unknown error occurred while adding WindowHandle to internal HashSet");
            return false;
        }
    }

    template <typename T>
    bool WindowHandleContainer<T>::contains(const WindowHandle window, bool *outContains) noexcept
    {
        std::lock_guard lockGuard(m_windowMutex);
        try
        {
            if (outContains)
                *outContains = m_windows.contains(window);
            return true;
        }
        catch(const std::exception &e)
        {
            KAZE_CORE_ERRCODE(Error::StdExcept, "Exception occurred while checking WindowHandle validity: {}", e.what());
            return false;
        }
        catch(...)
        {
            KAZE_CORE_ERRCODE(Error::Unknown, "Unknown error was thrown while checking WindowHandle validity");
            return false;
        }
    }

    template <typename T>
    bool WindowHandleContainer<T>::erase(const WindowHandle window, bool *outWasErased) noexcept
    {
        std::lock_guard lockGuard(m_windowMutex);
        try
        {
            const auto wasErased = m_windows.erase(window) > 0;
            if (outWasErased)
                *outWasErased = wasErased;
            return true;
        }
        catch(const std::exception &e)
        {
            KAZE_CORE_ERRCODE(Error::StdExcept, "Exception occurred while erasing window: {}", e.what());
            if (outWasErased)
                *outWasErased = false;
            return false;
        }
        catch(...)
        {
            KAZE_CORE_ERRCODE(Error::Unknown, "Unknown exception occurred while erasing window");
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
            auto it = m_windows.find(window);
            if (it == m_windows.end())
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
            KAZE_CORE_ERRCODE(Error::StdExcept, "Exception occurred while fetching window data: {}", e.what());
            return false;
        }
        catch(...)
        {
            KAZE_CORE_ERRCODE(Error::Unknown, "Unknown error occurred while fetching window data");
            return false;
        }
    }
}

KAZE_NAMESPACE_END

#endif // kaze_platform_backend_windowhandlecontainer_h_
