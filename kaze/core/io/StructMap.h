#pragma once
#ifndef kaze_core_io_structlayout_h_
#define kaze_core_io_structlayout_h_

#include <kaze/core/lib.h>
#include <kaze/core/endian.h>

#include <limits>
#include <type_traits>

KAZE_NAMESPACE_BEGIN

class StructLayout
{
public:
    struct Entry {
        Size offset;             ///< offset of the data in bytes
        Bool isPrimitive;
        Bool hasMultiple;                ///< it's a List or Array to be populated
        Size count;                      ///< number of fixed elements
        union {
            struct {
                Size size;               ///< size of the data in bytes (if string its the string length,
                                         ///< with VaryingLength == a varyign length string)
                Endian::Type srcEndian;  ///< endianness of the source data
                Bool isString;           ///< if size is SIZE_MAX, it's a var-length string, otherwise its fixed to the size
            } primitive;

            struct {
                const StructLayout *map;
            } composite;
        };
    };

    static constexpr Size VaryingLength = std::numeric_limits<Size>::max();

    StructLayout() = default;
    ~StructLayout() = default;

    StructLayout(StructLayout &&other) noexcept;
    auto operator= (StructLayout &&other) noexcept -> StructLayout &
    {
        m_entries = std::move(other.m_entries);
        m_arithmeticEndian = other.m_arithmeticEndian;
        m_stringEndian = other.m_stringEndian;

        return *this;
    }

    /// Add an arithmetic type member
    /// \param[in]  member       the arithemtic member to add
    /// \param[in]  endian       override the default arithmetic endian type if set [optional]
    /// \returns this object.
    template <typename Class, typename T> requires std::is_arithmetic_v<T>
    auto add(T Class::*member, Endian::Type endian = Endian::Unknown) -> StructLayout &
    {
        if ( !m_beginCalled ) {
            KAZE_CORE_ERRCODE(Error::LogicErr, "StructMap::begin() must be called before calling `add`");
            return *this;
        }

        m_entries.emplace_back({
            .offset = byteOffsetOf(member),
            .isPrimitive = KAZE_TRUE,
            .hasMultiple = KAZE_FALSE,
            .count = 0,
            .primitive = {
                .size = sizeof(T),
                .srcEndian = endian == Endian::Unknown ? m_arithmeticEndian : endian,
                .isString = KAZE_FALSE,
            }
        });

        return *this;
    }

    /// Add a `kaze::String` member
    /// \param[in]  member       the kaze::String member to add
    /// \param[in]  fixedLength  fixed length of the target string
    /// \param[in]  endian       override the default string endian type if set [optional]
    /// \returns this object.
    template <typename Class>
    auto add(String Class::*member, Size fixedLength, Endian::Type endian = Endian::Unknown) -> StructLayout &
    {
        if ( !m_beginCalled ) {
            KAZE_CORE_ERRCODE(Error::LogicErr, "StructMap::begin() must be called before calling `add`");
            return *this;
        }

        m_entries.emplace_back( Entry {
            .offset = byteOffsetOf(member),
            .isPrimitive = KAZE_TRUE,
            .hasMultiple = KAZE_FALSE,
            .count = 0,
            .primitive = {
                .size = fixedLength,
                .srcEndian = m_stringEndian,
                .isString = true,
            }
        });

        return *this;
    }

    /// Add a variable-length `kaze::String` member
    /// \param[in]  member       the kaze::String member to add
    /// \param[in]  endian       override the default string endian type if set [optional]
    /// \returns this object.
    template <typename Class>
    auto add(String Class::*member, Endian::Type endian = Endian::Unknown) -> StructLayout &
    {
        return add(member, VaryingLength, endian);
    }

    /// Add a complex data type
    template <typename Class, typename T>
    auto add(T Class::*member, const StructLayout *map) -> StructLayout &
    {
        if ( !m_beginCalled ) {
            KAZE_CORE_ERRCODE(Error::LogicErr, "StructMap::begin() must be called before calling `add`");
            return *this;
        }

        m_entries.emplace_back(Entry {
            .offset = byteoOffsetOf(member),
            .isPrimitive = KAZE_FALSE,
            .hasMultiple = KAZE_FALSE,
            .count = 0,
            .composite = {
                .map = map
            }
        });
        return *this;
    }

    /// Add a varying length list of arithmetic types
    /// \param[in]  member       the container member to add
    /// \param[in]  fixedLength  the fixed number of strings of fixed length to add; set to `VaryingLength` for varying length
    /// \param[in]  endian       override the default string endian type if set [optional]
    /// \returns this object.
    template <typename Class, typename T>
        requires std::is_arithmetic_v<T>
    auto add(List<T> Class:: *member, Endian::Type endian = Endian::Unknown) -> StructLayout &
    {
        if ( !m_beginCalled ) {
            KAZE_CORE_ERRCODE(Error::LogicErr, "StructMap::begin() must be called before calling `add`");
            return *this;
        }

        m_entries.emplace_back(Entry {
            .offset = byteOffsetOf(member),
            .isPrimitive = KAZE_TRUE,
            .hasMultiple = KAZE_TRUE,
            .count = VaryingLength,
            .primitive = {
                .size = sizeof(T),
                .srcEndian = endian == Endian::Unknown ? m_arithmeticEndian : endian,
                .isString = KAZE_FALSE,
            }
        });

        return *this;
    }

    /// Add a container of strings
    /// \param[in]  member           the container member to add
    /// \param[in]  containerLength  the fixed number of strings of fixed length to add (if SIZE_MAX, it's varying length)
    /// \param[in]  stringLength     the fixed length of each string, or varying length if set to SIZE_MAX
    /// \param[in]  endian       override the default string endian type if set [optional]
    /// \returns this object.
    template <typename Class>
    auto add(List<String> Class:: *member, Size stringLength, Endian::Type endian = Endian::Unknown) -> StructLayout &
    {
        if ( !m_beginCalled ) {
            KAZE_CORE_ERRCODE(Error::LogicErr, "StructMap::begin() must be called before calling `add`");
            return *this;
        }

        m_entries.emplace_back(Entry {
            .offset = byteOffsetOf(member),
            .isPrimitive = KAZE_TRUE,
            .hasMultiple = KAZE_TRUE,
            .count = VaryingLength,
            .primitive = {
                .size = stringLength,
                .srcEndian = endian == Endian::Unknown ? m_stringEndian : endian,
                .isString = KAZE_TRUE,
            }
        });

        return *this;
    }

    /// Add a fixed length Array of arithmetic types
    /// \param[in]  member       the container member to add
    /// \param[in]  fixedLength  the fixed number of strings of fixed length to add; set to `VaryingLength` for varying length
    /// \param[in]  endian       override the default string endian type if set [optional]
    /// \returns this object.
    template <typename Class, typename T, Size S>
        requires std::is_arithmetic_v<T>
    auto add(Array<T, S> Class:: *member, Endian::Type endian = Endian::Unknown) -> StructLayout &
    {
        if ( !m_beginCalled ) {
            KAZE_CORE_ERRCODE(Error::LogicErr, "StructMap::begin() must be called before calling `add`");
            return *this;
        }

        m_entries.emplace_back(Entry {
            .offset = byteOffsetOf(member),
            .isPrimitive = KAZE_TRUE,
            .hasMultiple = KAZE_TRUE,
            .count = S,
            .primitive = {
                .size = sizeof(T),
                .srcEndian = endian == Endian::Unknown ? m_arithmeticEndian : endian,
                .isString = KAZE_FALSE,
            }
        });

        return *this;
    }

    /// Add a container of strings
    /// \param[in]  member           the container member to add
    /// \param[in]  containerLength  the fixed number of strings of fixed length to add (if SIZE_MAX, it's varying length)
    /// \param[in]  stringLength     the fixed length of each string, or varying length if set to SIZE_MAX
    /// \param[in]  endian       override the default string endian type if set [optional]
    /// \returns this object.
    template <typename Class, Size S>
    auto add(Array<String, S> Class:: *member, Size stringLength, Endian::Type endian = Endian::Unknown) -> StructLayout &
    {
        if ( !m_beginCalled ) {
            KAZE_CORE_ERRCODE(Error::LogicErr, "StructMap::begin() must be called before calling `add`");
            return *this;
        }

        m_entries.emplace_back(Entry {
            .offset = byteOffsetOf(member),
            .isPrimitive = KAZE_TRUE,
            .hasMultiple = KAZE_TRUE,
            .count = S,
            .primitive = {
                .size = stringLength,
                .srcEndian = endian == Endian::Unknown ? m_stringEndian : endian,
                .isString = KAZE_TRUE,
            }
        });

        return *this;
    }

    /// Add a container of composite type items
    /// \param[in]  member       the container member to add
    /// \param[in]  map          the previously mapped type
    /// \param[in]  fixedLength  the fixed number of containers to add, or a varying length if SIZE_MAX.
    ///                          [optional, default: SIZE_MAX]
    /// \returns this object.
    template <typename Class, typename Container,
        typename T = std::remove_pointer_t<decltype(std::declval<Container>().data())>>
    auto add(Container Class:: *member, const StructLayout *map,
        Size fixedLength = VaryingLength) -> StructLayout &
    {
        if ( !m_beginCalled ) {
            KAZE_CORE_ERRCODE(Error::LogicErr, "StructMap::begin() must be called before calling `add`");
            return *this;
        }

        m_entries.emplace_back(Entry {
            .offset = byteOffsetOf(member),
            .isPrimitive = KAZE_FALSE,
            .hasMultiple = KAZE_TRUE,
            .count = fixedLength,
            .composite = {
                .map = map
            }
        });

        return *this;
    }

    /// Start the definition; clears any definition created before.
    /// \note Must be called before using `StructMap::add`
    /// \returns this object.
    auto begin() -> StructLayout &
    {
        if ( m_beginCalled )
        {
            KAZE_CORE_ERRCODE(Error::LogicErr, "StructMap::begin()/end() mismatch. Did you forget to call `end`?");
            return *this;
        }

        m_entries.clear();
        m_beginCalled = KAZE_TRUE;
        return *this;
    }

    /// End the definition; protects any additional entries from being added.
    auto end() -> StructLayout &
    {
        if ( !m_beginCalled )
        {
            KAZE_CORE_ERRCODE(Error::LogicErr, "StructMap::begin()/end() mismatch. Did you forget to call `begin`?");
            return *this;
        }

        m_beginCalled = KAZE_FALSE;
        return *this;
    }

    /// \returns whether there are any entries in the container
    [[nodiscard]]
    auto empty() const noexcept { return m_entries.empty(); }

    /// \returns the number of entries in the container
    [[nodiscard]]
    auto size() const noexcept { return m_entries.size(); }

    auto clear() noexcept { return m_entries.clear(); }

    [[nodiscard]]
    auto operator[] (Size index) const noexcept -> const Entry & { return m_entries[index]; }

    auto getEntries() const noexcept -> const List<Entry> & { return m_entries; }
    auto getDefaultArithmeticEndian() const noexcept -> Endian::Type { return m_arithmeticEndian; }
    auto getDefaultStringEndian() const noexcept -> Endian::Type { return m_stringEndian; }

private:
    List<Entry> m_entries           = {};
    Endian::Type m_arithmeticEndian = Endian::Little; ///< endianness for arithmetic types
    Endian::Type m_stringEndian     = Endian::Big;    ///< endianness for
    Bool m_beginCalled              = {KAZE_FALSE};
};

KAZE_NAMESPACE_END

#endif // kaze_core_io_structlayout_h_
