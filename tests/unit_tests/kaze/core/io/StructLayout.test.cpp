#include <doctest/doctest.h>

#include <kaze/core/errors.h>
#include <kaze/core/io/StructLayout.h>
#include <kaze/core/lib.h>

USING_KAZE_NAMESPACE;

TEST_SUITE("StructMap")
{
    TEST_CASE("Default constructor")
    {
        StructLayout layout{};
        CHECK(layout.empty());
        CHECK(layout.size() == 0);
        CHECK(layout.getDefaultArithmeticEndian() == Endian::Little);
        CHECK(layout.getDefaultStringEndian() == Endian::Big);
    }

    TEST_CASE("Map a simple, one-layer layout with numeric types")
    {
        struct Struct {
            Int i;
            Double d;
            Ubyte ub;
        };

        auto layout = StructLayout()
            .begin()
                .add(&Struct::i)
                .add(&Struct::d)
                .add(&Struct::ub)
            .end();
        CHECK(layout.size() == 3);
        const auto &i = layout.getEntries().at(0);
        const auto &d = layout.getEntries().at(1);
        const auto &ub = layout.getEntries().at(2);

        CHECK(i.offset == byteOffsetOf(&Struct::i));
        CHECK(i.isPrimitive);
        CHECK(i.hasMultiple == KAZE_FALSE);
        CHECK(i.type == typeid(Int));
        CHECK(i.primitive.isString == KAZE_FALSE);
        CHECK(i.primitive.srcEndian == layout.getDefaultArithmeticEndian());
        CHECK(i.primitive.size == sizeof(Int));

        CHECK(d.offset == byteOffsetOf(&Struct::d));
        CHECK(d.isPrimitive);
        CHECK(d.hasMultiple == KAZE_FALSE);
        CHECK(d.type == typeid(Double));
        CHECK(d.primitive.isString == KAZE_FALSE);
        CHECK(d.primitive.srcEndian == layout.getDefaultArithmeticEndian());
        CHECK(d.primitive.size == sizeof(Double));

        CHECK(ub.offset == byteOffsetOf(&Struct::ub));
        CHECK(ub.isPrimitive);
        CHECK(ub.hasMultiple == KAZE_FALSE);
        CHECK(ub.type == typeid(Ubyte));
        CHECK(ub.primitive.isString == KAZE_FALSE);
        CHECK(ub.primitive.srcEndian == layout.getDefaultArithmeticEndian());
        CHECK(ub.primitive.size == sizeof(Ubyte));
    }

    TEST_CASE("Map a layout with single Strings")
    {
        struct Struct {
            String name;
            String address;
            String bio;
        };

        StructLayout layout{};
        layout
            .begin()
                .add(&Struct::name)
                .add(&Struct::address)
                .add(&Struct::bio)
            .end();

        CHECK(layout.size() == 3);
        const auto &name    = layout.getEntries().at(0);
        const auto &address = layout.getEntries().at(1);
        const auto &bio     = layout.getEntries().at(2);

        CHECK(name.offset == byteOffsetOf(&Struct::name));
        CHECK(name.isPrimitive);
        CHECK(name.hasMultiple == KAZE_FALSE);
        CHECK(name.type == typeid(String));
        CHECK(name.primitive.isString);
        CHECK(name.primitive.srcEndian == layout.getDefaultStringEndian());
        CHECK(name.primitive.size == StructLayout::VaryingLength);

        CHECK(address.offset == byteOffsetOf(&Struct::address));
        CHECK(address.isPrimitive);
        CHECK(address.hasMultiple == KAZE_FALSE);
        CHECK(address.type == typeid(String));
        CHECK(address.primitive.isString);
        CHECK(address.primitive.srcEndian == layout.getDefaultStringEndian());
        CHECK(address.primitive.size == StructLayout::VaryingLength);

        CHECK(bio.offset == byteOffsetOf(&Struct::bio));
        CHECK(bio.isPrimitive);
        CHECK(bio.hasMultiple == KAZE_FALSE);
        CHECK(bio.type == typeid(String));
        CHECK(bio.primitive.isString);
        CHECK(bio.primitive.srcEndian == layout.getDefaultStringEndian());
        CHECK(bio.primitive.size == StructLayout::VaryingLength);
    }

    TEST_CASE("Map a layout with nested layouts")
    {
        
        struct StructA {
            Int i;
            String s;
        };

        struct StructB {
            StructA a;
        };

        struct StructC {
            StructB b;
        };

        auto layoutA = StructLayout()
            .begin()
                .add(&StructA::i)
                .add(&StructA::s)
            .end();

        auto layoutB = StructLayout()
            .begin()
                .add(&StructB::a, &layoutA)
            .end();

        auto layoutC = StructLayout()
            .begin()
                .add(&StructC::b, &layoutB)
            .end();

        CHECK( !layoutB.empty() );
        const auto &structA = layoutB.getEntries()[0];

        CHECK(structA.offset == byteOffsetOf(&StructB::a));
        CHECK(structA.isPrimitive == KAZE_FALSE);
        CHECK(structA.hasMultiple == KAZE_FALSE);
        CHECK(structA.type == typeid(StructA));
        CHECK(structA.composite.size == sizeof(StructA));
        CHECK(structA.composite.layout == &layoutA);

        CHECK( !layoutC.empty() );
        const auto &structB = layoutC.getEntries().at(0);
        CHECK(structB.offset == byteOffsetOf(&StructC::b));
        CHECK(structB.isPrimitive == KAZE_FALSE);
        CHECK(structB.hasMultiple == KAZE_FALSE);
        CHECK(structB.type == typeid(StructB));
        CHECK(structB.composite.size == sizeof(StructB));
        CHECK(structB.composite.layout == &layoutB);
    }

    TEST_CASE("Map a numeric type in a varying list")
    {
        struct Struct {
            const Int intSize; // hypothetical scenario with const type
            List<Int> ints;
        };

        auto layout = StructLayout()
            .begin()
                .add(&Struct::ints, &Struct::intSize)
            .end();

        CHECK( !layout.empty() );

        const auto &ints = layout.getEntries().at(0);
        CHECK(ints.offset == byteOffsetOf(&Struct::ints));
        CHECK(ints.hasMultiple);
        CHECK(ints.count == StructLayout::VaryingLength);
        CHECK(ints.countMemberOffset == byteOffsetOf(&Struct::intSize));
        CHECK(ints.countMemberType.hash_code() == typeid(Int).hash_code());

        CHECK(ints.type.hash_code() == typeid(Int).hash_code());
        CHECK(ints.isPrimitive);
        CHECK(ints.primitive.size == sizeof(Int));
        CHECK(ints.primitive.isString == KAZE_FALSE);
        CHECK(ints.primitive.srcEndian == layout.getDefaultArithmeticEndian());
    }

    TEST_CASE("Map a string type in a varying list")
    {
        struct Struct {
            Size stringCount;
            List<String> strings;
        };

        auto layout = StructLayout()
            .begin()
                .add(&Struct::stringCount)
                .add(&Struct::strings, &Struct::stringCount)
            .end();
        CHECK(layout.size() == 2);

        const auto &strings = layout.getEntries().at(1);
        CHECK(strings.offset == byteOffsetOf(&Struct::strings));
        CHECK(strings.hasMultiple);
        CHECK(strings.count == StructLayout::VaryingLength);
        CHECK(strings.countMemberOffset == byteOffsetOf(&Struct::stringCount));
        CHECK(strings.countMemberType == typeid(Size));
        CHECK(strings.type == typeid(String));
        CHECK(strings.isPrimitive);
        CHECK(strings.primitive.size == StructLayout::VaryingLength);
        CHECK(strings.primitive.isString);
        CHECK(strings.primitive.srcEndian == layout.getDefaultStringEndian());
    }

    TEST_CASE("Map a composite type in a varying list")
    {
        struct StructA {
            Int id;
            Double value;
        };

        struct StructB {
            Int id;
            Int count;
            List<StructA> value;
        };

        auto layoutA = StructLayout()
            .begin()
                .add(&StructA::id)
                .add(&StructA::value)
            .end();
        auto layoutB = StructLayout()
            .begin()
                .add(&StructB::id)
                .add(&StructB::count)
                .add(&StructB::value, &StructB::count, &layoutA)
            .end();

        auto const &structAList = layoutB.getEntries().at(2);
        CHECK(structAList.offset == byteOffsetOf(&StructB::value));
        CHECK(structAList.hasMultiple);
        CHECK(structAList.count == StructLayout::VaryingLength);
        CHECK(structAList.countMemberOffset == byteOffsetOf(&StructB::count));
        CHECK(structAList.countMemberType == typeid(Int));
        CHECK(structAList.isPrimitive == KAZE_FALSE);
        CHECK(structAList.composite.layout == &layoutA);
        CHECK(structAList.composite.size == sizeof(StructA));
    }

    TEST_CASE("Map a numeric type in a fixed array")
    {
        struct Struct {
            Array<Int, 5> ints;
        };

        auto layout = StructLayout()
            .begin()
                .add(&Struct::ints)
            .end();

        CHECK( !layout.empty() );

        const auto &ints = layout.getEntries().at(0);
        CHECK(ints.offset == byteOffsetOf(&Struct::ints));
        CHECK(ints.hasMultiple);
        CHECK(ints.count == 5);

        CHECK(ints.type.hash_code() == typeid(Int).hash_code());
        CHECK(ints.isPrimitive);
        CHECK(ints.primitive.size == sizeof(Int));
        CHECK(ints.primitive.isString == KAZE_FALSE);
        CHECK(ints.primitive.srcEndian == layout.getDefaultArithmeticEndian());
    }

    TEST_CASE("Map a string type in a fixed array")
    {
        struct Struct {
            Array<String, 10> strings;
        };

        auto layout = StructLayout()
            .begin()
                .add(&Struct::strings)
            .end();
        CHECK(layout.size() == 1);

        const auto &strings = layout.getEntries().at(0);
        CHECK(strings.offset == byteOffsetOf(&Struct::strings));
        CHECK(strings.hasMultiple);
        CHECK(strings.count == 10);
        CHECK(strings.type == typeid(String));
        CHECK(strings.isPrimitive);
        CHECK(strings.primitive.size == StructLayout::VaryingLength);
        CHECK(strings.primitive.isString);
        CHECK(strings.primitive.srcEndian == layout.getDefaultStringEndian());
    }

    TEST_CASE("Map a composite type in a fixed array")
    {
        struct StructA {
            Int id;
            Double value;
        };

        struct StructB {
            Int id;
            Array<StructA, 20> value;
        };

        auto layoutA = StructLayout()
            .begin()
                .add(&StructA::id)
                .add(&StructA::value)
            .end();
        auto layoutB = StructLayout()
            .begin()
                .add(&StructB::id)
                .add(&StructB::value, &layoutA)
            .end();

        auto const &structAList = layoutB.getEntries().at(1);
        CHECK(structAList.offset == byteOffsetOf(&StructB::value));
        CHECK(structAList.hasMultiple);
        CHECK(structAList.count == 20);
        CHECK(structAList.isPrimitive == KAZE_FALSE);
        CHECK(structAList.composite.layout == &layoutA);
        CHECK(structAList.composite.size == sizeof(StructA));
    }

    // ----- Some edge cases -----

    TEST_CASE("Mismatched class")
    {
        struct StructA {
            Int i;
        };

        struct StructB {
            Float f;
        };

        auto layout = StructLayout()
            .begin()
                .add(&StructA::i)
                .add(&StructB::f)
            .end();
        CHECK(layout.size() == 1); // failed to add StructB::i
        CHECK(layout.getEntries()[0].type == typeid(Int)); // type is from StructA::i not StructB::f
    }

    TEST_CASE("Mismatched begin/end")
    {
        struct Struct {
            Int i;
        };

        StructLayout layout{};

        SUBCASE("Forgot to add begin")
        {
            layout.add(&Struct::i);

            CHECK(layout.empty());
        }

        SUBCASE("Forgot to close with end")
        {
            layout
                .begin()
                    .add(&Struct::i);

            clearError();
            auto &entries = layout.getEntries(); // Attempting to getEntries sets an error
            CHECK(getError().code != Error::Ok);

            clearError();
            layout.begin();
            CHECK(getError().code != Error::Ok);
        }
    }
}
