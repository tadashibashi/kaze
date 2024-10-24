#include <doctest/doctest.h>


#include <kaze/core/ServiceProvider.h>

USING_KAZE_NAMESPACE;

TEST_SUITE("ServiceProvider")
{
    TEST_CASE("Test size, empty, set, get")
    {
        struct Person
        {
            String name;
        };

        ServiceProvider sp;
        Person p;
        p.name = "Bob";

        CHECK(sp.size() == 0);
        CHECK(sp.empty());

        CHECK(sp.getService<Person>() == nullptr);

        sp.provide(&p);

        CHECK(sp.getService<Person>() == &p);
        CHECK(sp.getService<Person>()->name == "Bob");
    }

    TEST_CASE("Can distinguish between inherited types when getting service")
    {
        struct RootService
        {
            int x;
        };

        struct ChildService : RootService
        {
            int y;
        };

        RootService r {
            .x = 10
        };
        ChildService c {
            .y = 20
        };

        ServiceProvider sp;

        sp.provide(&r);
        sp.provide(&c);

        CHECK(sp.size() == 2); // ensure they didn't overwrite each other

        CHECK(sp.getService<RootService>() == &r);
        CHECK(sp.getService<ChildService>() == &c);
    }

    TEST_CASE("Can erase entries")
    {
        ServiceProvider sp;

        int i = 10;
        sp.provide(&i);

        CHECK(sp.size() == 1);

        CHECK(sp.erase<int>()); // returns true when object is deleted

        CHECK(sp.empty());
    }

    TEST_CASE("Can iterate entries")
    {
        ServiceProvider sp;
        int i = 10;
        float f = 20.f;
        double d = 30.0;

        sp.provide(&i);
        sp.provide(&f);
        sp.provide(&d);

        for (auto &[type, ptr] : sp)
        {
            CHECK(ptr != nullptr);

            if (type == typeid(int))
            {
                const auto value = *static_cast<int *>(ptr);
                CHECK(value == 10);
            }
            else if (type == typeid(float))
            {
                const auto value = *static_cast<float *>(ptr);
                CHECK(value == 20.f);
            }
            else if (type == typeid(double))
            {
                const auto value = *static_cast<double *>(ptr);
                CHECK(value == 30.0);
            }
        }
    }

    TEST_CASE("Clear works as expected")
    {
        ServiceProvider sp;
        int i = 10;
        float f = 20.f;
        double d = 30.0;

        sp.provide(&i);
        sp.provide(&f);
        sp.provide(&d);

        CHECK(sp.size() == 3);

        sp.clear();

        CHECK(sp.size() == 0);
        CHECK(sp.empty());
        CHECK(sp.getService<int>() == nullptr);
        CHECK(sp.getService<float>() == nullptr);
        CHECK(sp.getService<double>() == nullptr);
    }

    TEST_CASE("tryProvide works as expected")
    {
        ServiceProvider sp;

        int i = 10;
        int i2 = 20;

        CHECK(sp.tryProvide(&i));
        CHECK(sp.size() == 1);
        CHECK(!sp.tryProvide(&i2));
        CHECK(sp.size() == 1);
        CHECK(*sp.getService<int>() == 10);
    }

    TEST_CASE("tryGetService works as expected")
    {
        ServiceProvider sp;
        int service = 123;
        int *outService;
        CHECK(!sp.tryGetService<int>(&outService));

        sp.provide(&service);

        CHECK(sp.size() == 1);

        CHECK(sp.tryGetService(&outService));
        CHECK(*outService == 123);
    }
}
