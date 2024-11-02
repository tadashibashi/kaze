#pragma once

#include <kaze/core/lib.h>
#include <kaze/core/concepts.h>
#include <kaze/core/endian.h>
#include <kaze/core/memory.h>
#include <kaze/core/traits.h>

#include <limits>
#include <typeindex>
#include <type_traits>

KAZE_NS_BEGIN

struct StructLayoutInit {
    /// Default endianness of source arithmetic data types
    Endian::Type arithmeticEndian = Endian::Little;
    /// Default endianness of string types
    Endian::Type stringEndian = Endian::Big;
};

/// Layout or recipe to populate a POD data type from binary source data
/// \note does not support pointer types
class StructLayout
{
public:
    struct Entry {
        Size offset;                     ///< offset of the data in bytes
                                         ///< determines the value in the union
        Bool hasMultiple{};              ///< it's a List or Array to be populated
        funcptr_t<void *(void *)> getDataPtr{}; ///< get the contiguous data array pointer from the offset pointer
        funcptr_t<void (void *, Size)> setDataElemSize{}; ///< callback to set size of callback when count == VaryingLength => varying List
        Size count{};                    ///< number of fixed elements, or if set to VaryingLength it has varying length
        Size countMemberOffset{};        ///< member to get the count from, this is either set manually, or retrieved through the StructLayout, it must be a `Size` type
        std::type_index countMemberType{typeid(void)}; ///< count member type index, used to retrieve the correct type
        const std::type_index type;      ///< if `hasMultiple` is false, it is the type of object stripped of its pointer,
                                         ///< if `hasMultiple` is true, this is the type it contains stipped of pointer
        Bool isPrimitive;                ///< `true` value is primitive, `false` value is composite;
        union {
            struct {
                Size size;               ///< size of the data in bytes (if string its the string length,
                                         ///< with VaryingLength == a varyign length string)
                Endian::Type srcEndian;  ///< endianness of the source data
                Bool isString;           ///< if size is SIZE_MAX, it's a var-length string, otherwise its fixed to the size
            } primitive;

            struct {
                Size size;                ///< size of the composite type in bytes
                const StructLayout *layout;
            } composite;
        };
    };

    /// When length is set to VaryingLength, it can vary, otherwise it is fixed to the value
    static constexpr Size VaryingLength = std::numeric_limits<Size>::max();

    StructLayout() = default;
    ~StructLayout() = default;

    StructLayout(const StructLayout &other) = default;
    auto operator= (const StructLayout &other) -> StructLayout & = default;

    StructLayout(StructLayout &&other) noexcept;
    auto operator= (StructLayout &&other) noexcept -> StructLayout &
    {
        m_entries = std::move(other.m_entries);
        m_arithmeticEndian = other.m_arithmeticEndian;
        m_stringEndian = other.m_stringEndian;

        return *this;
    }

    template <typename Class, typename T>
        requires std::is_enum_v<T>
    auto add(T Class::*member, Endian::Type endian = Endian::Unknown) -> StructLayout &
    {
        using Underlying = std::underlying_type_t<T>;

        if ( !addGuard(typeid(Class)) )
            return *this;

        m_entries.emplace_back(Entry {
            .offset = byteOffsetOf(member),
            .hasMultiple = KAZE_FALSE,
            .count = 0,
            .type = typeid(Underlying),
            .isPrimitive = KAZE_TRUE,
            .primitive = {
                .size = sizeof(Underlying),
                .srcEndian = endian == Endian::Unknown ? m_arithmeticEndian : endian,
                .isString = KAZE_FALSE,
            }
        });

        return *this;
    }

    /// Add an individual arithmetic type member
    /// \param[in]  member       the arithemtic member to add
    /// \param[in]  endian       override the default arithmetic endian type if set [optional]
    /// \returns this object.
    template <typename Class, Arithmetic T>
    auto add(T Class::*member, Endian::Type endian = Endian::Unknown) -> StructLayout &
    {
        if ( !addGuard(typeid(Class)) )
            return *this;

        m_entries.emplace_back(Entry {
            .offset = byteOffsetOf(member),
            .hasMultiple = KAZE_FALSE,
            .count = 0,
            .type = typeid(T),
            .isPrimitive = KAZE_TRUE,
            .primitive = {
                .size = sizeof(T),
                .srcEndian = endian == Endian::Unknown ? m_arithmeticEndian : endian,
                .isString = KAZE_FALSE,
            }
        });

        return *this;
    }

    /// Add an individual `kaze::String` member
    /// \param[in]  member       the kaze::String member to add
    /// \param[in]  fixedLength  fixed length of the target string; variable length if set to `VaryingLength`
    /// \param[in]  endian       override the default string endian type if set [optional]
    /// \returns this object.
    template <typename Class>
    auto add(String Class::*member, Size fixedLength = VaryingLength,
        Endian::Type endian = Endian::Unknown) -> StructLayout &
    {
        if ( !addGuard(typeid(Class)) )
            return *this;

        m_entries.emplace_back(Entry {
            .offset = byteOffsetOf(member),
            .hasMultiple = KAZE_FALSE,
            .count = 0,
            .type = typeid(String),
            .isPrimitive = KAZE_TRUE,
            .primitive = {
                .size = fixedLength,
                .srcEndian = m_stringEndian,
                .isString = true,
            }
        });

        return *this;
    }

    /// Add an individual composite data type
    /// \param[in]  member   the composite type member
    /// \param[in]  layout   the layout that represents the member
    /// \returns this object.
    template <typename Class, typename T>
    auto add(T Class::*member, const StructLayout *layout) -> StructLayout &
    {
        if ( !addGuard(typeid(Class)) )
            return *this;

        m_entries.emplace_back(Entry {
            .offset = byteOffsetOf(member),
            .hasMultiple = KAZE_FALSE,
            .count = 0,
            .type = typeid(T),
            .isPrimitive = KAZE_FALSE,
            .composite = {
                .size = sizeof(T),
                .layout = layout,
            }
        });
        return *this;
    }

    /// Add a varying-length List of arithmetic types
    /// \param[in]  member       the container member to add
    /// \param[in]  countMember  member to receive the List length from
    /// \param[in]  endian       override the default string endian type if set [optional]
    /// \returns this object.
    template <typename Class, typename T, Integral ListSize>
        requires std::is_arithmetic_v<T>
    auto add(List<T> Class:: *member, ListSize Class:: *countMember, Endian::Type endian = Endian::Unknown) -> StructLayout &
    {
        if ( !addGuard(typeid(Class)) )
            return *this;

        m_entries.emplace_back(Entry {
            .offset = byteOffsetOf(member),
            .hasMultiple = KAZE_TRUE,
            .getDataPtr = [](void *ptr) { return (void *)((List<T> *)ptr)->data(); },
            .setDataElemSize = [](void *ptr, Size size) { ((List<T> *)ptr)->assign(size, {}); },
            .count = VaryingLength,
            .type = typeid(T),
            .isPrimitive = KAZE_TRUE,
            .countMemberOffset = byteOffsetOf(countMember),
            .countMemberType = typeid(std::remove_const_t<ListSize>),
            .primitive = {
                .size = sizeof(T),
                .srcEndian = endian == Endian::Unknown ? m_arithmeticEndian : endian,
                .isString = KAZE_FALSE,
            }
        });

        return *this;
    }


    /// Add a varying-length List of enum types
    /// \param[in]  member       the container member to add
    /// \param[in]  countMember  member to receive the List length from
    /// \param[in]  endian       override the default string endian type if set [optional]
    /// \returns this object.
    template <typename Class, typename T, Integral ListSize>
        requires std::is_enum_v<T>
    auto add(List<T> Class:: *member, ListSize Class:: *countMember, Endian::Type endian = Endian::Unknown) -> StructLayout &
    {
        using Underlying = std::underlying_type_t<T>;

        if ( !addGuard(typeid(Class)) )
            return *this;

        m_entries.emplace_back(Entry {
            .offset = byteOffsetOf(member),
            .hasMultiple = KAZE_TRUE,
            .getDataPtr = [](void *ptr) { return (void *)((List<T> *)ptr)->data(); },
            .setDataElemSize = [](void *ptr, Size size) { ((List<T> *)ptr)->assign(size, {}); },
            .count = VaryingLength,
            .type = typeid(Underlying),
            .isPrimitive = KAZE_TRUE,
            .countMemberOffset = byteOffsetOf(countMember),
            .countMemberType = typeid(std::remove_const_t<ListSize>),
            .primitive = {
                .size = sizeof(Underlying),
                .srcEndian = endian == Endian::Unknown ? m_arithmeticEndian : endian,
                .isString = KAZE_FALSE,
            }
        });

        return *this;
    }

    /// Add a varying-length List of Strings
    /// \param[in]  member           the container member to add
    /// \param[in]  countMember      member to receive the List length from
    /// \param[in]  stringLength     the fixed length of each string, or varying length if set to `VaryingLength`
    /// \param[in]  endian       override the default string endian type if set [optional]
    /// \returns this object.
    template <typename Class, Integral ListSize>
    auto add(List<String> Class:: *member, ListSize Class:: *countMember, Size stringLength = VaryingLength,
        Endian::Type endian = Endian::Unknown) -> StructLayout &
    {
        if ( !addGuard(typeid(Class)) )
            return *this;

        m_entries.emplace_back(Entry {
            .offset = byteOffsetOf(member),
            .hasMultiple = KAZE_TRUE,
            .getDataPtr = [](void *ptr) { return (void *)((List<String> *)ptr)->data(); },
            .setDataElemSize = [](void *ptr, Size size) { ((List<String> *)ptr)->assign(size, {}); },
            .count = VaryingLength,
            .countMemberOffset = byteOffsetOf(countMember),
            .countMemberType = typeid(std::remove_const_t<ListSize>),
            .type = typeid(String),
            .isPrimitive = KAZE_TRUE,
            .primitive = {
                .size = stringLength,
                .srcEndian = endian == Endian::Unknown ? m_stringEndian : endian,
                .isString = KAZE_TRUE,
            }
        });

        return *this;
    }

    /// Add a varying-length List of composite type items
    /// \param[in]  member       the List member to add
    /// \param[in]  countMember  the member to get the List length from
    /// \param[in]  layout       a layout type
    /// \returns this object.
    template <typename Class, typename T, Integral ListSize>
    auto add(List<T> Class:: *member, ListSize Class:: *countMember, const StructLayout *layout) -> StructLayout &
    {
        if ( !addGuard(typeid(Class)) )
            return *this;

        m_entries.emplace_back(Entry {
            .offset = byteOffsetOf(member),
            .hasMultiple = KAZE_TRUE,
            .getDataPtr = [](void *ptr) { return (void *)((List<T> *)ptr)->data(); },
            .setDataElemSize = [](void *ptr, Size size) { ((List<T> *)ptr)->assign(size, {}); },
            .count = VaryingLength,
            .countMemberOffset = byteOffsetOf(countMember),
            .countMemberType = typeid(std::remove_const_t<ListSize>),
            .type = typeid(T),
            .isPrimitive = KAZE_FALSE,
            .composite = {
                .size = sizeof(T),
                .layout = layout
            }
        });

        return *this;
    }

    /// Add a fixed-length Array of arithmetic types
    /// \param[in]  member       the container member to add
    /// \param[in]  fixedLength  the fixed number of strings of fixed length to add;
    ///                          set to `VaryingLength` for varying length
    /// \param[in]  endian       override the default string endian type if set [optional]
    /// \returns this object.
    template <typename Class, typename T, Size S>
        requires std::is_arithmetic_v<T>
    auto add(Array<T, S> Class:: *member, Endian::Type endian = Endian::Unknown) -> StructLayout &
    {
        if ( !addGuard(typeid(Class)) )
            return *this;

        m_entries.emplace_back(Entry {
            .offset = byteOffsetOf(member),
            .hasMultiple = KAZE_TRUE,
            .getDataPtr = [](void *ptr) { return (void *)((Array<T, S> *)ptr)->data(); },
            .count = S,
            .type = typeid(T),
            .isPrimitive = KAZE_TRUE,
            .primitive = {
                .size = sizeof(T),
                .srcEndian = endian == Endian::Unknown ? m_arithmeticEndian : endian,
                .isString = KAZE_FALSE,
            }
        });

        return *this;
    }

    /// Add a fixed-length Array of enum types
    /// \param[in]  member       the container member to add
    /// \param[in]  fixedLength  the fixed number of strings of fixed length to add;
    ///                          set to `VaryingLength` for varying length
    /// \param[in]  endian       override the default string endian type if set [optional]
    /// \returns this object.
    template <typename Class, typename T, Size S>
        requires std::is_enum_v<T>
    auto add(Array<T, S> Class:: *member, Endian::Type endian = Endian::Unknown) -> StructLayout &
    {
        using Underlying = std::underlying_type_t<T>;

        if ( !addGuard(typeid(Class)) )
            return *this;

        m_entries.emplace_back(Entry {
            .offset = byteOffsetOf(member),
            .hasMultiple = KAZE_TRUE,
            .getDataPtr = [](void *ptr) { return (void *)((Array<T, S> *)ptr)->data(); },
            .count = S,
            .type = typeid(Underlying),
            .isPrimitive = KAZE_TRUE,
            .primitive = {
                .size = sizeof(Underlying),
                .srcEndian = endian == Endian::Unknown ? m_arithmeticEndian : endian,
                .isString = KAZE_FALSE,
            }
        });

        return *this;
    }

    /// Add a fixed-length Array of Strings
    /// \param[in]  member         the container member to add
    /// \param[in]  stringLength   the fixed length of each string, or varying length if set to `VaryingLength`
    /// \param[in]  endian         override the default string endian type if set [optional]
    /// \returns this object.
    template <typename Class, Size S>
    auto add(Array<String, S> Class:: *member, Size stringLength = VaryingLength,
        Endian::Type endian = Endian::Unknown) -> StructLayout &
    {
        if ( !addGuard(typeid(Class)) )
            return *this;

        m_entries.emplace_back(Entry {
            .offset = byteOffsetOf(member),
            .hasMultiple = KAZE_TRUE,
            .getDataPtr = [](void *ptr) { return (void *)((Array<String, S> *)ptr)->data(); },
            .count = S,
            .type = typeid(String),
            .isPrimitive = KAZE_TRUE,
            .primitive = {
                .size = stringLength,
                .srcEndian = endian == Endian::Unknown ? m_stringEndian : endian,
                .isString = KAZE_TRUE,
            }
        });

        return *this;
    }

    /// Add a varying-length List of composite type items
    /// \param[in]  member       the container member to add
    /// \param[in]  layout       a layout type
    /// \returns this object.
    template <typename Class, typename T, Size S>
    auto add(Array<T, S> Class:: *member, const StructLayout *layout) -> StructLayout &
    {
        if ( !addGuard(typeid(Class)) )
            return *this;

        m_entries.emplace_back(Entry {
            .offset = byteOffsetOf(member),
            .hasMultiple = KAZE_TRUE,
            .getDataPtr = [](void *ptr) { return (void *)((Array<T, S> *)ptr)->data(); },
            .count = S,
            .type = typeid(T),
            .isPrimitive = KAZE_FALSE,
            .composite = {
                .size = sizeof(T),
                .layout = layout
            }
        });

        return *this;
    }

    /// Start the definition; clears any definition created before.
    /// \note Must be called before using `StructLayout::add`
    /// \returns this object.
    auto begin(const StructLayoutInit &config = {}) -> StructLayout &
    {
        if ( m_beginCalled )
        {
            KAZE_PUSH_ERR(Error::LogicErr, "StructLayout::begin()/end() mismatch. "
                "Did you forget to call `end`?");
            return *this;
        }

        m_entries.clear();
        m_beginCalled = KAZE_TRUE;
        m_arithmeticEndian = config.arithmeticEndian;
        m_stringEndian = config.stringEndian;
        m_class = typeid(void);
        return *this;
    }

    /// End the definition; protects any additional entries from being added.
    /// \returns this object.
    auto end() -> StructLayout &
    {
        if ( !m_beginCalled )
        {
            KAZE_PUSH_ERR(Error::LogicErr, "StructLayout::begin()/end() mismatch. "
                "Did you forget to call `begin`?");
            return *this;
        }

        m_beginCalled = KAZE_FALSE;
        return *this;
    }

    /// \returns whether there are any entries in the container
    [[nodiscard]]
    auto empty() const noexcept -> Size { return m_entries.empty(); }

    /// \returns the number of entries in the container
    [[nodiscard]]
    auto size() const noexcept -> Size { return m_entries.size(); }

    /// Clear all struct layout entries
    auto clear() noexcept -> void
    {
        m_entries.clear();
        m_beginCalled = KAZE_FALSE;
        m_class = typeid(void);
    }

    [[nodiscard]]
    auto operator[] (Size index) const noexcept -> const Entry & { return m_entries[index]; }

    [[nodiscard]]
    auto getEntries() const noexcept -> const List<Entry> &
    {
        if (m_beginCalled)
            KAZE_PUSH_ERR(Error::LogicErr,
                "StructLayout::end() was not called before calling `getEntries`");
        return m_entries;
    }
    [[nodiscard]]
    auto getDefaultArithmeticEndian() const noexcept -> Endian::Type { return m_arithmeticEndian; }
    [[nodiscard]]
    auto getDefaultStringEndian() const noexcept -> Endian::Type { return m_stringEndian; }

    [[nodiscard]]
    auto getClassType() const noexcept -> std::type_index { return m_class; }

    /// \note outEntry is only valid as long as the container isn't cleared or mutated
    template <typename Class, typename T>
    auto tryGetEntry(T Class::*member, const Entry **outEntry) const noexcept -> Bool
    {
        KAZE_ASSERT(outEntry != nullptr);
        KAZE_ASSERT(m_class == typeid(Class), "class/struct must match that which was used to init layout");

        for (const auto &entry : m_entries)
        {
            if (entry.offset == byteOffsetOf(member))
            {
                *outEntry = &entry;
                return KAZE_TRUE;
            }
        }

        return KAZE_FALSE;
    }

    template <typename Class, typename T>
    [[nodiscard]]
    auto contains(T Class::*member) const noexcept -> Bool
    {
        for (const auto &entry : m_entries)
        {
            if (entry.offset == byteOffsetOf(member))
            {
                return KAZE_TRUE;
            }
        }

        return KAZE_FALSE;
    }
private:
    [[nodiscard]]
    auto addGuard(const std::type_index type) const noexcept -> Bool
    {
        if ( !m_beginCalled )
        {
            KAZE_PUSH_ERR(Error::LogicErr, "StructLayout::begin() must be called before calling `add`");
            return KAZE_FALSE;
        }

        if ( !checkClass(type) )
        {
            KAZE_PUSH_ERR(Error::LogicErr, "StructLayout class/struct type mismatch: "
                "was expecting {}, but got {}", m_class.name(), type.name());
            return KAZE_FALSE;
        }

        return KAZE_TRUE;
    }

    [[nodiscard]]
    auto checkClass(const std::type_index type) const noexcept -> Bool
    {
        if (m_entries.empty())
        {
            m_class = type;
            return KAZE_TRUE;
        }

        return type == m_class;
    }

    List<Entry> m_entries           = {};
    Endian::Type m_arithmeticEndian = Endian::Little; ///< endianness for arithmetic types
    Endian::Type m_stringEndian     = Endian::Big;    ///< endianness for
    Bool m_beginCalled              = {KAZE_FALSE};
    mutable std::type_index m_class = typeid(void);
};

KAZE_NS_END
