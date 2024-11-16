#include "StructIO.h"

KAZE_NS_BEGIN

/// Only for use in readNumeric
#define CHECK_NTYPE(t) do { \
    if (type == typeid(t)) { \
        *outValue = reverse ? Endian::swap(*(t *)source) : *(t *)source;\
        return sizeof(t); \
    } \
} while(0)

/// \param[in]  source   data source pointer to read type from
/// \param[in]  type     the arithmetic type to read
/// \param[in]  reverse  whether to swap endianness of data
/// \param[out] outValue retrieves the numeric value from the source data
/// \returns the number of bytes read, the size of the numeric type, or 0 on error.
static auto readSize(void *source, std::type_index type, Bool reverse,
                              Size *outValue) -> Size {
    CHECK_NTYPE(Int8);
    CHECK_NTYPE(Int16);
    CHECK_NTYPE(Int);
    CHECK_NTYPE(Int64);
    CHECK_NTYPE(Uint8);
    CHECK_NTYPE(Uint16);
    CHECK_NTYPE(Uint);
    CHECK_NTYPE(Uint64);
    CHECK_NTYPE(Float);
    CHECK_NTYPE(Double);

    KAZE_PUSH_ERR(Error::InvalidArgErr,
        "Unsupported arithmetic type: {}",
        type.name());
    return 0;
}

static auto readImpl(void *dest,
                     BufferView &view,
                     const StructLayout &thisLayout) -> Size
{
    auto startSrcByte = view.tell();

    for (const auto &entry : thisLayout.getEntries())
    {
        if (entry.isPrimitive)
        {
            // Primitive type

            if (entry.primitive.isString)
            {
                auto str = (String *)((Ubyte *)dest + entry.offset);
                if (entry.primitive.size == StructLayout::VaryingLength)
                {
                    view.read(str, {.endian=entry.primitive.srcEndian});
                }
                else
                {
                    view.read(
                        str,
                        static_cast<Int64>(entry.primitive.size),
                        {.endian=entry.primitive.srcEndian});
                }
            }
            else
            {
                auto num = (Ubyte *)dest + entry.offset;
                KAZE_ASSERT(entry.primitive.size <= 128 && entry.primitive.size > 0,
                    "ensure regular arithemtic type size");
                view.readNumber(num, static_cast<Int64>(entry.primitive.size), entry.primitive.srcEndian);
            }
        }
        else
        {
            // Composite type

            // Get layout
            const auto layout = entry.composite.layout;
            if ( !layout )
            {
                KAZE_PUSH_ERR(Error::RuntimeErr, "Missing composite layout");
                view.seek(startSrcByte, SeekBase::Start);
                return 0;
            }

            if (entry.hasMultiple)
            {
                // Array or List

                if ( !entry.getDataPtr )
                {
                    KAZE_PUSH_ERR(Error::RuntimeErr, "Missing expected `getDataPtr` field in entry");
                    view.seek(startSrcByte, SeekBase::Start);
                    return 0;
                }

                Size count;
                if (entry.count == StructLayout::VaryingLength)
                {
                    // List

                    if ( !entry.setDataElemSize )
                    {
                        KAZE_PUSH_ERR(Error::RuntimeErr, "Missing expected `setDataElemSize` field in varying list entry");
                        view.seek(startSrcByte, SeekBase::Start);
                        return 0;
                    }

                    // get the count from another member
                    if ( !readSize((Ubyte *)dest + entry.countMemberOffset, entry.countMemberType, KAZE_FALSE, &count) )
                    {
                        KAZE_PUSH_ERR(Error::RuntimeErr, "Failed to read varying list element count");
                        view.seek(startSrcByte, SeekBase::Start);
                        return 0;
                    }

                    entry.setDataElemSize((Ubyte *)dest + entry.offset, count);
                }
                else
                {
                    // Array

                    count = entry.count;
                }

                // Read multiple
                const auto dataPtr = entry.getDataPtr((Ubyte *)dest + entry.offset);
                for (Size i = 0; i < count; ++i)
                {
                    readImpl(
                        (Ubyte *)dataPtr + entry.composite.size * i,
                        view,
                        *entry.composite.layout);
                }
            }
            else
            {
                // Single composite object
                readImpl((Ubyte *)dest + entry.offset, view, *layout);
            }
        }
    }

    return view.tell() - startSrcByte;
}

static auto writeImpl(const void *src, BufferWriter &writer, const StructLayout &thisLayout) -> Size
{
    auto startByte = writer.size();

    for (const auto &entry : thisLayout.getEntries())
    {
        if (entry.isPrimitive)
        {
            // primitive type

            if (entry.primitive.isString)
            {
                auto str = (String *)(Ubyte *)src + entry.offset;
                if (entry.primitive.size == StructLayout::VaryingLength)
                {
                    if ( !str->empty() )
                        writer.writeString(str->data(), str->size(), {.endian = entry.primitive.srcEndian});
                }
                else
                {
                    writer.writeString(str->data(), str->size(), {
                        .fixedBufSize = entry.primitive.size,
                        .endian = entry.primitive.srcEndian,
                    });
                }
            }
            else
            {
                auto num = (Ubyte *)src + entry.offset;
                KAZE_ASSERT(entry.primitive.size <= 128 && entry.primitive.size > 0,
                    "ensure regular arithmetic type size");
                writer.writeNumber(num, entry.primitive.size, entry.primitive.srcEndian);
            }
        }
        else
        {
            // composite type

            // Get layout
            const auto layout = entry.composite.layout;
            if ( !layout )
            {
                KAZE_PUSH_ERR(Error::RuntimeErr, "Missing composite layout");
                writer.skipBackTo(startByte);
                return 0;
            }

            if (entry.hasMultiple)
            {
                // Array or List of composite type

                if ( !entry.getDataPtr )
                {
                    KAZE_PUSH_ERR(Error::RuntimeErr, "Missing expected `getDataPtr` field in entry");
                    writer.skipBackTo(startByte);
                    return 0;
                }

                Size count;
                if (entry.count == StructLayout::VaryingLength)
                {
                    // List

                    if ( !entry.setDataElemSize )
                    {
                        KAZE_PUSH_ERR(Error::RuntimeErr, "Missing expected `setDataElemSize` field in entry");
                        writer.skipBackTo(startByte);
                        return 0;
                    }

                    // get the count from another member
                    if ( !readSize((Ubyte *)src + entry.countMemberOffset, entry.countMemberType, KAZE_FALSE, &count) )
                    {
                        KAZE_PUSH_ERR(Error::RuntimeErr, "Failed to read varying list element count");
                        writer.skipBackTo(startByte);
                        return 0;
                    }
                }
                else
                {
                    count = entry.count;
                }

                // Write multiple
                const auto dataPtr = entry.getDataPtr((Ubyte *)src + entry.offset);
                for (Size i = 0; i < count; ++i)
                {
                    writeImpl(
                        (Ubyte *)dataPtr + entry.composite.size * i,
                        writer,
                        *layout);
                }
            }
            else
            {
                // Single composite type
                writeImpl((Ubyte *)src + entry.offset, writer, *layout);
            }
        }
    }

    return writer.size() - startByte;
}

auto StructIO::readLayout(void *dest, BufferView &view,
                                const StructLayout &layout) -> Size
{
    if (layout.empty())
    {
        return 0;
    }

    return readImpl(dest, view, layout);
}

auto StructIO::writeLayout(const void *dest, BufferWriter &writer, const StructLayout &layout) -> Size
{
    if (layout.empty())
    {
        return 0;
    }

    return writeImpl(dest, writer, layout);
}

KAZE_NS_END

