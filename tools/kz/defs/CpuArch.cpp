#include "CpuArch.h"
#include "../lib/str.h"

auto kz::CpuArch::fromName(std::string_view name) -> Enum
{
    Enum type = Unknown;
    if (str::compareNoCase(name, "x86_64") == 0) type = Enum::x86_64;
    else if (str::compareNoCase(name, "arm64") == 0) type = Enum::Arm64;
    else if (str::compareNoCase(name, "universal") == 0) type = Enum::Universal;
    return type;
}

auto kz::CpuArch::getName(Enum type) -> std::string_view
{
    switch(type)
    {
    case Enum::Arm64: return "Arm64";
    case Enum::x86_64: return "x86_64";
    case Enum::Universal: return "Universal";
    default: return "Unknown";
    }
}
