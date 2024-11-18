#include "BuildType.h"
#include "../lib/str.h"

auto kz::BuildType::fromName(std::string_view name) -> Enum
{
    Enum type = Unknown;
    if (str::compareNoCase(name, "debug") == 0) type = Enum::Debug;
    if (str::compareNoCase(name, "release") == 0) type = Enum::Release;
    if (str::compareNoCase(name, "relwithdebinfo") == 0) type = Enum::RelWithDebInfo;
    if (str::compareNoCase(name, "minsizerel") == 0) type = Enum::MinSizeRel;
    return type;
}

auto kz::BuildType::getName(Enum type) -> std::string_view
{
    switch(type)
    {
    case Enum::Release: return "Release";
    case Enum::Debug: return "Debug";
    case Enum::RelWithDebInfo: return "RelWithDebInfo";
    case Enum::MinSizeRel: return "MinSizeRel";
    default: return "Unknown";
    }
}
