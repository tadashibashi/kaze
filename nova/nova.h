#pragma once
#ifndef nova_nova_h_
#define nova_nova_h_

#include <array>
#include <cstdint>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#ifndef NOVA_NAMESPACE
#define NOVA_NAMESPACE nova
#endif

#if defined(NOVA_NO_NAMESPACE)
#define NOVA_NAMESPACE_BEGIN
#define NOVA_NAMESPACE_END
#else
#define NOVA_NAMESPACE_BEGIN namespace NOVA_NAMESPACE {
#define NOVA_NAMESPACE_END }
#endif

NOVA_NAMESPACE_BEGIN

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

template <typename K, typename V, typename Hash = std::hash<K>, typename Pred = std::equal_to<K>,
          typename Alloc = std::allocator<std::pair<const K, V> >>
using Dictionary = std::unordered_map<K, V>;

template <typename T, typename Allocator = std::allocator<T>>
using Array = std::vector<T, Allocator>;

template <typename T, Uint Size>
using FixedArray = std::array<T, Size>;

using String = std::string;
using StringView = std::string_view;

NOVA_NAMESPACE_END

#if defined (__clang__)
#   define NOVA_COMPILER_CLANG 1
#   define NOVA_COMPILER_GCC 0
#   define NOVA_COMPILER_MSVC 0
#elif defined(__GNUC__)
#   define NOVA_COMPILER_CLANG 0
#   define NOVA_COMPILER_GCC 1
#   define NOVA_COMPILER_MSVC 0
#elif defined(_MSC_VER)
#   define NOVA_COMPILER_CLANG 0
#   define NOVA_COMPILER_GCC 0
#   define NOVA_COMPILER_MSVC 1
#else
#   define NOVA_COMPILER_CLANG 0
#   define NOVA_COMPILER_GCC 0
#   define NOVA_COMPILER_MSVC 0
#endif

#if NOVA_COMPILER_CLANG || NOVA_COMPILER_GCC
#   define NOVA_PACKED __attribute__((packed))
#elif NOVA_COMPILER_MSVC
#   define NOVA_PACKED __declspec(align(1))
#else
#   define NOVA_PACKED
#endif

#endif // nova_nova_h_
