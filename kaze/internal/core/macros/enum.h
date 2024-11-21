#pragma once

#include <kaze/internal/core/lib.h>
#include "map.h"

#include <ostream>

#define KAZE_DEFINE_FLAGNAME_CASE(FlagName) \
    case FlagName: return #FlagName;

#define KAZE_DEFINE_FLAGNAME_SWITCH(ParamName, ...) \
    switch (ParamName) { \
        KAZE_MAP(KAZE_DEFINE_FLAGNAME_CASE, __VA_ARGS__) \
        default: return ""; \
    }
#define KAZE_DEFINE_FLAGNAME_GETTER(Class, FuncName, ...) \
    static constexpr const Char *FuncName(Class::Flags_ flag) { \
        KAZE_DEFINE_FLAGNAME_SWITCH(flag, __VA_ARGS__) \
    }

#define KAZE_DEFINE_FLAG(Flag) Flag = 1u << (int)Enum_::Flag

#define KAZE_DEFINE_FLAG_MEMBERS(Name, ...) \
private:                                                                          \
    enum class Enum_ : KAZE_NAMESPACE::Int                                        \
    {                                                                             \
        __VA_ARGS__,                                                              \
        PRIVATE_Enum_CoUnT_                                                       \
    };                                                                            \
public:                                                                           \
    KAZE_NAMESPACE::Uint value;                                                   \
    enum Flags_ : KAZE_NAMESPACE::Uint                                            \
    {                                                                             \
        KAZE_MAP_LIST(KAZE_DEFINE_FLAG, __VA_ARGS__)                              \
    };                                                                            \
    /* NOLINT(*-explicit-constructor) */                                          \
    explicit constexpr Name(Flags_ flag) : value(flag) { }                        \
    explicit constexpr Name(KAZE_NAMESPACE::Uint val = 0) : value(val) { }        \
    Name &operator=(const Name &flags) { value = flags.value; return *this; }     \
    Name &operator=(Flags_ flag) { value = flag; return *this; }                  \
                                                                                  \
    constexpr operator KAZE_NAMESPACE::Uint() const { return value; }                       \
                                                                                            \
    constexpr Name operator|(const Name& other) const { return Name(value | other.value); } \
    constexpr Name operator&(const Name& other) const { return Name(value & other.value); } \
    constexpr Name operator^(const Name& other) const { return Name(value ^ other.value); } \
                                                                                  \
    Name& operator|=(const Name& other) { value |= other.value; return *this; }   \
    Name& operator&=(const Name& other) { value &= other.value; return *this; }   \
    Name& operator^=(const Name& other) { value ^= other.value; return *this; }   \
    constexpr Name operator|(const Name::Flags_ flag) const { return Name(value | flag); }  \
    constexpr Name operator&(const Name::Flags_ flag) const { return Name(value & flag); }  \
    constexpr Name operator^(const Name::Flags_ flag) const { return Name(value ^ flag); }  \
                                                                                  \
    Name& operator|=(const Name::Flags_ flag) { value |= flag; return *this; }    \
    Name& operator&=(const Name::Flags_ flag) { value &= flag; return *this; }    \
    Name& operator^=(const Name::Flags_ flag) { value ^= flag; return *this; }    \
    constexpr Name operator~() const { return Name(~value); }                                                          \
    Name& reset() { value = 0; return *this; }                                                                         \
    Name& toggle(Name::Flags_ flag) { value ^= static_cast<KAZE_NAMESPACE::Uint>(flag); return *this; }                \
    [[nodiscard]] constexpr bool isSet(Name::Flags_ flag) const { return static_cast<bool>(value & flag); }            \
    [[nodiscard]] constexpr bool isSet(const Name &flag) const { return (value & flag.value) == flag.value; }          \
    [[nodiscard]] constexpr bool isOneSet(const Name &flag) const { return (value & flag.value) > 0; }                 \
    Name &set(Name::Flags_ flag, bool setVal) { if (setVal) value |= flag; else value &= ~flag; return *this; }        \
    static constexpr KAZE_NAMESPACE::Int count() { return static_cast<int>(Enum_::PRIVATE_Enum_CoUnT_) - 1; }          \
    /* Convenience function to get a flag container all fields set. */                                                 \
    static constexpr Name all() { auto flags = Name(); flags.value = 0xffffffff; return flags; }                       \
    [[nodiscard]] constexpr bool operator==(Name::Flags_ flag) const { return value == flag; }                         \
    [[nodiscard]] constexpr bool operator!=(Name::Flags_ flag) const { return value != flag; }                         \
    [[nodiscard]] constexpr bool operator==(const Name &other) const { return value == other.value; }                  \
    [[nodiscard]] constexpr bool operator!=(const Name &other) const { return value != other.value; }                  \
    [[nodiscard]] constexpr bool empty() const { return value == 0; }

#define KAZE_DEFINE_FLAGS(Name, ...) \
    struct Name { \
        KAZE_DEFINE_FLAG_MEMBERS(Name, __VA_ARGS__); \
    }

#define KAZE_DEFINE_FLAGS_WITH_NAMES(Name, ...) \
    struct Name { \
        KAZE_DEFINE_FLAG_MEMBERS(Name, __VA_ARGS__); \
    public: \
        KAZE_DEFINE_FLAGNAME_GETTER(Name, getName, __VA_ARGS__) \
    }
// Define global bitwise operators for a specific type that was previously declared via KAZE_DEFINE_FLAGS
// Please make sure that the Name is the exact scoping to where the it is defined, and that this macro
// is called outside of the class scope, and in a namespace or global scope after the former declaration.
#define KAZE_DEFINE_FLAGOPS(Name) \
constexpr Name operator|(Name::Flags_ a, Name::Flags_ b)                       \
{                                                                           \
    return Name(static_cast<KAZE_NAMESPACE::Uint>(a) | static_cast<KAZE_NAMESPACE::Uint>(b));       \
}                                                                           \
constexpr Name operator&(Name::Flags_ a, Name::Flags_ b)                       \
{                                                                           \
    return Name(static_cast<KAZE_NAMESPACE::Uint>(a) & static_cast<KAZE_NAMESPACE::Uint>(b));       \
}                                                                           \
constexpr Name operator^(Name::Flags_ a, Name::Flags_ b)                       \
{                                                                           \
    return Name(static_cast<KAZE_NAMESPACE::Uint>(a) ^ static_cast<KAZE_NAMESPACE::Uint>(b));       \
}

#define KAZE_DEFINE_FLAGOPS_WITH_NAMES(Name) \
    inline std::ostream &operator<<(std::ostream &os, Name::Flags_ flag) { \
        os << Name::getName(flag); \
        return os; \
    }

/// Create a flags enumeration in the current scope. It cannot be placed inside
/// of class/struct scope because it provides namespace/global-scoped operators.
/// The default type of the flags are `KAZE_NAMESPACE::Uint`, 32-bit unsigned integers.
///
/// Because the underlying type of the flag enum is not the class itself, make sure to instantiate flags through
/// the explicit constructor. Once instantiated the operaters will behave as normal.
/// @code
///   FlagName flags1(FlagName::ValueA);
///   FlagName flags2(FlagName::ValueA | FlagName::ValueB);
///   auto flags3 = FlagName(FlagName::ValueA);
///   auto flags4 = FlagName(FlagName::ValueA | FlagName::ValueB);
/// @endcode
///
/// Here is an example of flags in typical usage:
/// @code
/// KAZE_FLAGS_LEAN(AbilityFlag, CanJump, CanFly, CanSwim);
///
/// int main()
/// {
///     auto flags = AbilityFlag::CanJump | AbilityFlag::CanFly;
///
///     flags.reset(); // reset the flags to 0
///     flags.value = 3u; // setting the flags directly
///     ...
/// }
/// @endcode
/// \param Name the name of the flags enum (without enclosing quotation marks)
/// \param ...  the name of each value (without enclosing quotation marks)
///             the actual values will start from 1 and bitshifted incrementally.
///             e.g. KAZE_FLAGS_LEAN(Abilities, CanFly, CanJump, CanSwim)
///             Abilities::CanFly = 1
///             Abilities::CanJump = 2
///             Abilities::CanSwim = 4
#define KAZE_FLAGS_LEAN(Name, ...)          \
KAZE_DEFINE_FLAGS(Name, __VA_ARGS__); \
KAZE_DEFINE_FLAGOPS(Name)

#define KAZE_FLAGS(Name, ...) \
    KAZE_DEFINE_FLAGS_WITH_NAMES(Name, __VA_ARGS__); \
    KAZE_DEFINE_FLAGOPS(Name) \
    KAZE_DEFINE_FLAGOPS_WITH_NAMES(Name)
