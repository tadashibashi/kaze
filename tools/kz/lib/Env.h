#pragma once
#include "Comparators.h"
#include <charconv>
#include <map>
#include <string>
#include <string_view>
#include <type_traits>

namespace kz {
   struct EnvLoadOpts {
        bool silent = true;
        bool require = false;    ///< if file specified in load function does not exist
    };

    class Env {
    public:
        Env() = default;
        explicit Env(std::string_view filepath, const EnvLoadOpts &opts = {});
        ~Env() = default;

        Env(Env &&other) noexcept;
        auto operator=(Env &&other) noexcept -> Env &;

        /// Load data into the Env container. Overwrites any duplicate keys, but
        /// maintains any kvp's currently in the container.
        /// \param[in]  filepath    path to load env data from. Target file must only contain key-value pairs.
        /// \param[in]  silent      whether to log output on error.
        /// \returns whether load succeeded
        auto load(std::string_view filepath = ".env", const EnvLoadOpts &opts = {}) -> bool;

        /// Load data into the Env container. Throws on failure. Overwirtes any duplicate keys,
        /// but maintaining any kvp's currently in the container.
        auto loadRequire(std::string_view filepath = ".env") -> void;

        auto clear() noexcept -> void;

        /// Get a value and attempt to convert it to a numeric value
        /// \param[in]  key      key of value to find
        /// \param[out] outValue retrieves converted value
        /// \returns whether retrieval succeeded; may fail if key is not found,
        ///          or found value cannot successfully convert to the arithmetic type.
        template <typename T> requires std::is_arithmetic_v<T>
        auto tryGet(std::string_view key, T *outValue) -> bool
        {
            if ( !outValue )
                return false;

            const auto it = m_env.find(key);
            if (it == m_env.end())
                return false;

            T result;
            auto [ptr, errorCode] = std::from_chars(
                it->second.data(), it->second.data() + it->second.size(),
                result);
            if (errorCode != std::errc())
                return false;

            *outValue = result;
            return true;
        }

        [[nodiscard]]
        auto begin() { return m_env.begin(); }
        [[nodiscard]]
        auto begin() const { return m_env.cbegin(); }
        [[nodiscard]]
        auto end() { return m_env.end(); }
        [[nodiscard]]
        auto end() const { return m_env.cend(); }

        /// Check if key exists in the container. Useful for checking if something is defined
        [[nodiscard]]
        auto contains(std::string_view &key) const { return m_env.contains(key); }

        /// Try to get an environment variable value (loaded file values take priority and falls back to environment)
        /// \param[in]   key       key of the value to find
        /// \param[out]  outValue  valid as long as the value is not overwritten in the container, or
        ///                        this container is destroyed. Copy to std::string if you need to store it.
        /// \returns whether the key exists in the container and the value was retrieved.
        auto tryGet(std::string_view key, std::string_view *outValue) const -> bool;

        /// Try to get an environment variable value (loaded file values take priority & falls back to environment
        /// then it falls back to provided `defaultVal`)
        /// \param[in]  key         key of the value to find
        /// \param[in]  defaultVal  fallback if value is non-existant in both loaded file vars and environment
        auto getOrDefault(std::string_view key, std::string_view defaultVal = "") -> std::string_view
        {
            tryGet(key, &defaultVal); // if tryGet fails, defaultVal is left unmutated and returns
            return defaultVal;
        }

        [[nodiscard]]
        auto empty() const noexcept { return m_env.empty(); }

        /// \returns Number of times this object successfully loaded a file via `load` or `loadRequire`
        [[nodiscard]]
        auto timesLoaded() const noexcept { return m_timesLoaded; }
    private:
            std::map<std::string, std::string, StringViewComparator> m_env{};
            int m_timesLoaded = 0;
    };

}
