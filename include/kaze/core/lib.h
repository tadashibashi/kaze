#pragma once
#include <kaze/internal/core/lib.h>
#include <kaze/internal/core/Action.h>
#include <kaze/internal/core/AlignedList.h>
#include <kaze/internal/core/AssetLoader.h>
#include <kaze/internal/core/CommandQueue.h>
#include <kaze/internal/core/ConditionalAction.h>
#include <kaze/internal/core/ServiceProvider.h>

KAZE_PUBLIC_NS_BEGIN

using KAZE_NS_INTERNAL::Action;
using KAZE_NS_INTERNAL::AlignedList;
using KAZE_NS_INTERNAL::AssetLoader;
using KAZE_NS_INTERNAL::CommandQueue;
using KAZE_NS_INTERNAL::ConditionalAction;
using KAZE_NS_INTERNAL::ServiceProvider;

using KAZE_NS_INTERNAL::Int8;
using KAZE_NS_INTERNAL::Int16;
using KAZE_NS_INTERNAL::Int64;
using KAZE_NS_INTERNAL::Int;
using KAZE_NS_INTERNAL::Uint8;
using KAZE_NS_INTERNAL::Uint16;
using KAZE_NS_INTERNAL::Uint;
using KAZE_NS_INTERNAL::Uint64;

using KAZE_NS_INTERNAL::Byte;
using KAZE_NS_INTERNAL::Ubyte;
using KAZE_NS_INTERNAL::Size;

using KAZE_NS_INTERNAL::Char;
using KAZE_NS_INTERNAL::Uchar;

using KAZE_NS_INTERNAL::Cstring;

using KAZE_NS_INTERNAL::Float;
using KAZE_NS_INTERNAL::Double;

using KAZE_NS_INTERNAL::Bool;

using KAZE_NS_INTERNAL::Variant;
using KAZE_NS_INTERNAL::Dictionary;
using KAZE_NS_INTERNAL::HashSet;
using KAZE_NS_INTERNAL::List;
using KAZE_NS_INTERNAL::Map;
using KAZE_NS_INTERNAL::Array;
using KAZE_NS_INTERNAL::String;
using KAZE_NS_INTERNAL::StringView;

template <typename Class, typename T>
Size byteOffsetOf(T Class::*member)
{
    return KAZE_NS_INTERNAL::byteOffsetOf(member);
}

using KAZE_NS_INTERNAL::True;
using KAZE_NS_INTERNAL::False;
using KAZE_NS_INTERNAL::Null;

KAZE_PUBLIC_NS_END
