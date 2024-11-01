#include "Env.h"
#include "../lib/str.h"

#include <iostream>
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

kz::Env::Env(std::string_view filepath, const EnvLoadOpts &opts)
{
    load(filepath, opts);
}

kz::Env::Env(Env &&other) noexcept :
    m_env(std::move(other.m_env)), m_timesLoaded(other.m_timesLoaded)
{
    other.m_timesLoaded = 0;
}

auto kz::Env::operator=(Env &&other) noexcept -> Env &
{
    if (this == &other) return *this;

    m_env = std::move(other.m_env);
    m_timesLoaded = other.m_timesLoaded;

    other.m_timesLoaded = 0;
    return *this;
}

auto kz::Env::load(std::string_view filepath, const EnvLoadOpts &opts) -> bool
{
    if ( !fs::exists(filepath.data()) )
    {
        if ( !opts.silent )
            std::cerr << "Missing environment file at: \"" << filepath << "\"\n";
        if (opts.require)
            throw std::runtime_error("Missing env file");
        return false;
    }

    std::ifstream file(filepath.data());
    if ( !file.is_open() )
    {
        if ( !opts.silent )
            std::cerr << "Failed to open environment file at: \"" <<
                filepath << "\"\n";
        if (opts.require)
            throw std::runtime_error("Failed to open env file");
        return false;
    }

    std::string linebuf; linebuf.reserve(512);
    while (std::getline(file, linebuf))
    {
        std::string_view line = linebuf;
        if (line.empty()) continue;

        const auto equalSignPos = line.find_first_of('=');
        if (equalSignPos == std::string::npos || equalSignPos == 0)
        {
            continue;
        }

        auto name = str::trim(line.substr(0, equalSignPos));
        auto value = str::trim(line.substr(equalSignPos + 1));

        if (name.empty())
        {
            continue;
        }

        m_env[std::string(name.data(), name.size())] =
            std::string(value.data(), value.size());
    }

    ++m_timesLoaded;
    return true;
}

auto kz::Env::loadRequire(std::string_view filepath) -> void
{
    load(filepath, {
        .silent = false,
        .require = true
    });
}

auto kz::Env::clear() noexcept -> void
{
    m_env.clear();
}

auto kz::Env::tryGet(std::string_view key, std::string_view *outValue) const -> bool
{
    if ( !outValue )
        return false;

    const auto it = m_env.find(key);
    if (it == m_env.end())
    {
        if (const auto envVal = std::getenv(key.data())) // fallback to environment
        {
            *outValue = envVal;
            return true;
        }

        return false;
    }

    *outValue = it->second;
    return true;
}
