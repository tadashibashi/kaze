#include <doctest/doctest.h>
#include <kaze/core/io/BufferIO.h>
#include <kaze/core/io/BufferWriter.h>
#include <kaze/core/io/StructLayout.h>

USING_KAZE_NAMESPACE;

TEST_SUITE("BufferIO")
{
    TEST_CASE("Read flat struct layout")
    {
        enum class Year {
            Freshman,
            Sophomore,
            Junior,
            Senior,
        };

        struct Student {
            String name;
            Year year;
            Float gpa;
        };

        auto studentLayout = StructLayout()
            .begin()
                .add(&Student::name)
                .add(&Student::year)
                .add(&Student::gpa)
            .end();

        SUBCASE("Read")
        {
            BufferWriter writer{};
            writer << "Joe" << Year::Freshman << 3.4f;

            BufferView view(writer.data(), writer.size());
            auto student = BufferIO::read<Student>(view, studentLayout);

            CHECK(student.name == "Joe");
            CHECK(student.year == Year::Freshman);
            CHECK(student.gpa == 3.4f);
        }

        SUBCASE("Write")
        {
            Student joe;
            joe.name = "Robert";
            joe.year = Year::Sophomore;
            joe.gpa = 3.2f;

            BufferWriter writer{};
            BufferIO::write(&joe, writer, studentLayout);

            BufferView view(writer.data(), writer.size());
            auto student = BufferIO::read<Student>(view, studentLayout);

            CHECK(student.name == "Robert");
            CHECK(student.year == Year::Sophomore);
            CHECK(student.gpa == 3.2f);
        }
    }

    TEST_CASE("Nested struct layout with List of composite type")
    {
        struct Record {
            String name;
            Int value;
        };

        struct Collection {
            Int count;
            List<Record> records;
        };

        auto recordLayout = StructLayout()
            .begin()
                .add(&Record::name)
                .add(&Record::value)
            .end();

        auto collectionLayout = StructLayout()
            .begin()
                .add(&Collection::count, Endian::opposite(Endian::Native)) // test individual reverse endian
                .add(&Collection::records, &Collection::count, &recordLayout)
            .end();

        SUBCASE("Read")
        {
            BufferWriter writer{};
            writer << Endian::swap(4) << // reverse endian of count
                "Mozart Violin Concerto in A Major" <<
                10 <<
                "Bach Brandenburg Concerto No. 4" <<
                12 <<
                "Beethoven 7th Symphony" <<
                14 <<
                "Shostakovich 5th Symphony" <<
                15;

            BufferView view(writer.data(), writer.size());

            Collection collection;
            CHECK(BufferIO::read(&collection, view, collectionLayout));

            CHECK(collection.count == 4);
            CHECK(collection.records.size() == 4);
            CHECK(collection.records[0].name == "Mozart Violin Concerto in A Major");
            CHECK(collection.records[0].value == 10);
            CHECK(collection.records[1].name == "Bach Brandenburg Concerto No. 4");
            CHECK(collection.records[1].value == 12);
            CHECK(collection.records[2].name == "Beethoven 7th Symphony");
            CHECK(collection.records[2].value == 14);
            CHECK(collection.records[3].name == "Shostakovich 5th Symphony");
            CHECK(collection.records[3].value == 15);
        }

        SUBCASE("Write")
        {
            Collection collection;

            collection.records.emplace_back(Record {
                .name = "Mozart Violin Concerto in A Major",
                .value = 10,
            });
            collection.records.emplace_back(Record {
                .name = "Beethoven 7th Symphony",
                .value = 123,
            });
            collection.records.emplace_back(Record {
                .name = "Shostakovich 5th Symphony",
                .value = 44,
            });
            collection.count = collection.records.size();

            BufferWriter writer{};
            BufferIO::write(&collection, writer, collectionLayout);

            BufferView view(writer.data(), writer.size());
            Collection check;
            BufferIO::read(&check, view, collectionLayout);
            CHECK(check.count == 3);
            CHECK(check.records.size() == 3);
            CHECK(check.records[0].name == "Mozart Violin Concerto in A Major");
            CHECK(check.records[0].value == 10);
            CHECK(check.records[1].name == "Beethoven 7th Symphony");
            CHECK(check.records[1].value == 123);
            CHECK(check.records[2].name == "Shostakovich 5th Symphony");
            CHECK(check.records[2].value == 44);
        }
    }

    TEST_CASE("Nested struct layout with Array of composite type")
    {
        struct Record {
            String name;
            Int value;
        };

        struct Collection {
            Array<Record, 4> records;
        };

        auto recordLayout = StructLayout()
            .begin()
                .add(&Record::name)
                .add(&Record::value)
            .end();

        auto collectionLayout = StructLayout()
            .begin()
                .add(&Collection::records, &recordLayout)
            .end();

        SUBCASE("Read")
        {
            BufferWriter writer{};
            writer <<
                "Mozart Violin Concerto in A Major" <<
                10 <<
                "Bach Brandenburg Concerto No. 4" <<
                12 <<
                "Beethoven 7th Symphony" <<
                14 <<
                "Shostakovich 5th Symphony" <<
                15;

            BufferView view(writer.data(), writer.size());

            Collection collection;
            CHECK(BufferIO::read(&collection, view, collectionLayout));

            CHECK(collection.records[0].name == "Mozart Violin Concerto in A Major");
            CHECK(collection.records[0].value == 10);
            CHECK(collection.records[1].name == "Bach Brandenburg Concerto No. 4");
            CHECK(collection.records[1].value == 12);
            CHECK(collection.records[2].name == "Beethoven 7th Symphony");
            CHECK(collection.records[2].value == 14);
            CHECK(collection.records[3].name == "Shostakovich 5th Symphony");
            CHECK(collection.records[3].value == 15);
        }

        SUBCASE("Write")
        {
            Collection collection;

            collection.records[0] = Record {
                .name = "Mozart Violin Concerto in A Major",
                .value = 10,
            };
            collection.records[1] = Record {
                .name = "Beethoven 7th Symphony",
                .value = 123,
            };
            collection.records[2] = Record {
                .name = "Shostakovich 5th Symphony",
                .value = 44,
            };
            collection.records[3] = Record {
                .name = "Shostakovich 4th Symphony",
                .value = 253,
            };

            BufferWriter writer{};
            BufferIO::write(&collection, writer, collectionLayout);

            BufferView view(writer.data(), writer.size());
            Collection check;
            BufferIO::read(&check, view, collectionLayout);
            CHECK(check.records[0].name == "Mozart Violin Concerto in A Major");
            CHECK(check.records[0].value == 10);
            CHECK(check.records[1].name == "Beethoven 7th Symphony");
            CHECK(check.records[1].value == 123);
            CHECK(check.records[2].name == "Shostakovich 5th Symphony");
            CHECK(check.records[2].value == 44);
            CHECK(check.records[3].name == "Shostakovich 4th Symphony");
            CHECK(check.records[3].value == 253);
        }
    }
}
