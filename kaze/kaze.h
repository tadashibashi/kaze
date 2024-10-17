/// @file kaze.h
/// Contains type aliases, and basic engine defines
#pragma once
#ifndef kaze_h_
#define kaze_h_

#include "platform/defines.h"

#include <array>
#include <cassert>
#include <cstdint>
#include <map>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <variant>

#ifndef KAZE_NAMESPACE
#define KAZE_NAMESPACE kaze
#endif

#if KAZE_NO_NAMESPACE
#define KAZE_NAMESPACE_BEGIN
#define KAZE_NAMESPACE_END
#define USING_KAZE_NAMESPACE
#else
#define KAZE_NAMESPACE_BEGIN namespace KAZE_NAMESPACE {
#define KAZE_NAMESPACE_END }
#define USING_KAZE_NAMESPACE using namespace KAZE_NAMESPACE
#endif

KAZE_NAMESPACE_BEGIN

using Int8 = int8_t;
using Int16 = int16_t;
using Int64 = int64_t;
using Int = int32_t;
using Uint8 = uint8_t;
using Uint16 = uint16_t;
using Uint = uint32_t;
using Uint64 = uint64_t;

using Byte = Int8;
using Ubyte = Uint8;
using Size = size_t;

using Char = char;
using Uchar = unsigned char;

using Cstring = const char *;

using Float = float;
using Double = double;

using Bool = bool;

template <typename ...Types>
using Variant = std::variant<Types...>;

template <typename K, typename V, typename Hash = std::hash<K>, typename Pred = std::equal_to<K>,
          typename Alloc = std::allocator<std::pair<const K, V> >>
using Dictionary = std::unordered_map<K, V>;

template <typename T, typename Hash = std::hash<T>, typename Pred = std::equal_to<T>, typename Alloc = std::allocator<T>>
using HashSet = std::unordered_set<T, Hash, Pred, Alloc>;

template <typename T, typename Allocator = std::allocator<T>>
using List = std::vector<T, Allocator>;

template <typename K, typename V, typename Hash = std::hash<K>, typename Pred = std::equal_to<K>,
          typename Alloc = std::allocator<std::pair<const K, V> >>
using Map = std::map<K, V>;

template <typename T, Uint Size>
using Array = std::array<T, Size>;

using String = std::string;
using StringView = std::string_view;

KAZE_NAMESPACE_END

#if KAZE_COMPILER_CLANG || KAZE_COMPILER_GCC
#   define KAZE_PACKED __attribute__((packed))
#elif KAZE_COMPILER_MSVC
#   define KAZE_PACKED __declspec(align(1))
#else
#   define KAZE_PACKED
#endif

#define KAZE_TRUE static_cast<Bool>(true)
#define KAZE_FALSE static_cast<Bool>(false)

// No-op
#if KAZE_COMPILER_MSVC
#define KAZE_NOOP __noop
#else
#define KAZE_NOOP do {} while(0)
#endif

#ifndef KAZE_DEBUG
#define KAZE_DEBUG 0
#endif

#if KAZE_DEBUG

#   define KAZE__GET_ASSERT(_1, _2, NAME, ...) NAME
#   define KAZE__ASSERT_1(statement) assert(statement)
#   define KAZE__ASSERT_2(statement, message) do { \
    if ( !(statement) ) \
    { \
        KAZE_CORE_ERR("{}", (message) ); \
        assert(statement); \
    } \
} while(0)
#   define KAZE_ASSERT(...) KAZE__GET_ASSERT(__VA_ARGS__, KAZE__ASSERT_2, KAZE__ASSERT_1)(__VA_ARGS__)

#include <kaze/debug.h>
#else
#   define KAZE_ASSERT(...) KAZE_NOOP
#endif

#define KAZE_NO_COPY(classname) \
    classname(const classname &) = delete; \
    auto operator=(const classname &)->classname & = delete

#endif // kaze_h_
