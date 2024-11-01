#include "TargetPlatform.h"
#include "../lib/str.h"
#include "HostPlatform.h"

auto kz::TargetPlatform::fromName(std::string_view name) -> Enum
{
    Enum type = Unknown;
    if (str::compareNoCase(name, "desktop") == 0)
    {
        switch(HostPlatform::get())
        {
        case HostPlatform::MacOS: type = Enum::MacOS; break;
        case HostPlatform::Linux: type = Enum::Linux; break;
        case HostPlatform::Windows: type = Enum::Windows; break;
        default: type = Enum::Unknown; break;
        }
    }
    else if (str::compareNoCase(name, "emscripten") == 0) type = Enum::Emscripten;
    else if (str::compareNoCase(name, "ios") == 0)        type = Enum::iOS;
    else if (str::compareNoCase(name, "android") == 0)    type = Enum::Android;
    else if (str::compareNoCase(name, "tvos") == 0)       type = Enum::tvOS;
    else if (str::compareNoCase(name, "watchos") == 0)    type = Enum::watchOS;
    else if (str::compareNoCase(name, "visionos") == 0)   type = Enum::visionOS;
    return type;
}

auto kz::TargetPlatform::getName(Enum type) -> std::string_view
{
    switch(type)
    {
    case Enum::Android:    return "android";
    case Enum::Emscripten: return "emscripten";
    case Enum::iOS:        return "iOS";
    case Enum::Linux:      return "desktop";
    case Enum::MacOS:      return "desktop";
    case Enum::tvOS:       return "tvOS";
    case Enum::watchOS:    return "watchOS";
    case Enum::Windows:    return "desktop";
    case Enum::visionOS:   return "visionOS";
    default:               return "unknown";
    }
}
