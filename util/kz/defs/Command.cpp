#include "Command.h"
#include "../lib/str.h"

auto kz::Command::fromName(std::string_view name) -> Enum
{
    if (str::compareNoCase(name, "config") == 0) return Config;
    if (str::compareNoCase(name, "build") == 0) return Build;
    if (str::compareNoCase(name, "run") == 0) return Run;
    return Unknown;
}

auto kz::Command::getName(Enum type) -> std::string_view
{
    switch(type)
    {
    case Enum::Build: return "build";
    case Enum::Config: return "config";
    case Enum::Run: return "run";
    default: return "unknown";
    }
}

