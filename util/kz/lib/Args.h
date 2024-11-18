#pragma once
#include "Comparators.h"
#include <map>
#include <optional>
#include <string>
#include <vector>

namespace kz {
    class Args {
    public:
        Args() = default;
        Args(int argc, char **argv);
        ~Args() = default;

        Args(Args &&other) noexcept;
        auto operator=(Args &&other) noexcept -> Args &;

        using OptionMap = std::map<std::string, std::string, StringViewComparator>;

        [[nodiscard]]
        auto getOpt(std::string_view key) const -> std::optional<std::string_view>
        {
            const auto it = m_opts.find(key);
            if (it != m_opts.end())
                return it->second;
            return {};
        }

        [[nodiscard]]
        auto getOpt(std::initializer_list<std::string_view> keys) const -> std::optional<std::string_view>
        {
            for (auto key : keys)
            {
                const auto it = m_opts.find(key);
                if (it != m_opts.end())
                    return it->second;
            }

            return {};
        }

        [[nodiscard]]
        auto getOpt(std::string_view key, std::string_view defaultVal) const -> std::string_view
        {
            const auto it = m_opts.find(key);
            if (it != m_opts.end())
                return it->second;
            return defaultVal;
        }

        [[nodiscard]]
        auto getOpt(std::initializer_list<std::string_view> keys, std::string_view defaultVal) const -> std::string_view
        {
            for (auto key : keys)
            {
                const auto it = m_opts.find(key);
                if (it != m_opts.end())
                    return it->second;
            }

            return defaultVal;
        }

        [[nodiscard]]
        auto opts() const noexcept -> const OptionMap & { return m_opts; }

        [[nodiscard]]
        auto values() const noexcept -> const std::vector<std::string> & { return m_values; }

    private:
        std::map<std::string, std::string, StringViewComparator> m_opts;
        std::vector<std::string> m_values;
    };
}


