#include <doctest/doctest.h>
#include <kaze/core.h>

USING_KAZE_NS;

TEST_SUITE("Action")
{
    TEST_CASE("General Test")
    {
        Action<int> action;
        CHECK(action.empty());
        CHECK(action.size() == 0);

        int testData = 0;
        auto callback = [](int n, void *userdata) {
            auto testData = static_cast<int *>(userdata);
            *testData = n;
        };

        action.add(callback, &testData);
        CHECK(!action.empty());
        CHECK(action.size() == 1);

        action(123);

        CHECK(testData == 123);

        action.remove(callback, &testData);
        CHECK(action.empty());
        CHECK(action.size() == 0);

        action(456); // does not affect the result because it's removed
        CHECK(testData == 123);
    }

    TEST_CASE("contains")
    {
        Action<int> action;
        auto func1 = [](int i, void *userptr) { };
        auto func2 = [](int i, void *userptr) { };
        auto func3 = [](int i, void *userptr) { };

        CHECK( !action.contains(func1) );
        CHECK( !action.contains(func2) );
        CHECK( !action.contains(func3) );

        action.add(func1);
        CHECK(action.contains(func1));
        CHECK( !action.contains(func2) );
        CHECK( !action.contains(func3) );

        action.remove(func1);
        CHECK( !action.contains(func1) );
        CHECK( !action.contains(func2) );
        CHECK( !action.contains(func3) );

        action.add(func1);
        action.add(func2);
        action.add(func3);

        CHECK(action.contains(func1));
        CHECK(action.contains(func2));
        CHECK(action.contains(func3));

        action.remove(func1);
        CHECK( !action.contains(func1) );
        CHECK(action.contains(func2));
        CHECK(action.contains(func3));

        action.remove(func2);
        CHECK( !action.contains(func1) );
        CHECK( !action.contains(func2) );
        CHECK(action.contains(func3));

        action.remove(func3);
        CHECK( !action.contains(func1) );
        CHECK( !action.contains(func2) );
        CHECK( !action.contains(func3) );
    }

    TEST_CASE("Mismatched context or pointer will not remove callback")
    {
        Action<int> action;

        struct UserData {
            int value{};
        } dataA, dataB;

        auto timesTwo = [](int n, void *userptr) {
            auto data = static_cast<UserData *>(userptr);
            data->value = n * 2;
        };

        auto plusTwo = [](int n, void *userptr) {
            auto data = static_cast<UserData *>(userptr);
            data->value = n + 2;
        };

        action.add(timesTwo, &dataA);
        action.add(plusTwo, &dataB);

        action(10);
        CHECK(dataA.value == 20);
        CHECK(dataB.value == 12);

        CHECK(action.size() == 2);

        // Failed removals
        action.remove(timesTwo, &dataB);
        action.remove(plusTwo, &dataA);

        CHECK(action.size() == 2);

        // Run callbacks for extra proof
        action(20);
        CHECK(dataA.value == 40);
        CHECK(dataB.value == 22);

        // Now remove them
        action.remove(timesTwo, &dataA);
        action.remove(plusTwo, &dataB);
        CHECK(action.empty());

        // Call the action just to ensure callbacks not called and thus the values not mutated
        action(40);
        CHECK(dataA.value == 40);
        CHECK(dataB.value == 22);
    }

    TEST_CASE("Removal during invoke")
    {
        Action<Int, const String &> action;
        struct UserData {
            Action<Int, const String &> *action;
            funcptr_t< void(Int, const String &, void *)> func;
            Int intValue;
            String stringValue;
        };

        auto callback1 = [](Int i, const String &s, void *userptr)
        {
            auto data = static_cast<UserData *>(userptr);
            data->intValue = i;
            data->stringValue = s;
            data->action->remove(data->func, userptr);
        };
        auto data1 = UserData {
            .action = &action,
            .func = callback1,
            .intValue = 0,
            .stringValue = "",
        };

        auto callback2 = [](Int i, const String &s, void *userptr)
        {
            auto data = static_cast<UserData *>(userptr);
            data->intValue = i * 2;
            data->stringValue = s + s;
            data->action->remove(data->func, userptr);
        };
        auto data2 = UserData {
            .action = &action,
            .func = callback2,
            .intValue = 0,
            .stringValue = "",
        };

        auto callback3 = [](Int i, const String &s, void *userptr)
        {
            auto data = static_cast<UserData *>(userptr);
            data->intValue = i * 3;
            data->stringValue = s + s + s;
            data->action->remove(data->func, userptr);
        };
        auto data3 = UserData {
            .action = &action,
            .func = callback3,
            .intValue = 0,
            .stringValue = "",
        };
        auto callback4 = [](Int i, const String &s, void *userptr)
        {
            auto data = static_cast<UserData *>(userptr);
            data->intValue = i * 4;
            data->stringValue = s + s + s + s;
        };
        auto data4 = UserData {
            .action = &action,
            .func = callback4,
            .intValue = 0,
            .stringValue = "",
        };

        action.add(callback1, &data1);
        action.add(callback2, &data2);
        action.add(callback3, &data3);
        action.add(callback4, &data4);
        CHECK(action.size() == 4);

        action(10, "hello");
        CHECK(data1.intValue == 10);
        CHECK(data2.intValue == 20);
        CHECK(data3.intValue == 30);
        CHECK(data4.intValue == 40);
        CHECK(data1.stringValue == "hello");
        CHECK(data2.stringValue == "hellohello");
        CHECK(data3.stringValue == "hellohellohello");
        CHECK(data4.stringValue == "hellohellohellohello");
        CHECK(action.size() == 1); // each call back except callback4 has unsubscribed itself
        CHECK(action.contains(callback4, &data4));

        // Calling the Action will not affect the unsubscribed callbacks
        action(20, "goodbye");
        CHECK(data1.intValue == 10); // maintains value
        CHECK(data2.intValue == 20);
        CHECK(data3.intValue == 30);
        CHECK(data4.intValue == 80); // changed
        CHECK(data1.stringValue == "hello"); // maintains value
        CHECK(data2.stringValue == "hellohello");
        CHECK(data3.stringValue == "hellohellohello");
        CHECK(data4.stringValue == "goodbyegoodbyegoodbyegoodbye"); // changed
    }

    TEST_CASE("clear")
    {
        struct UserData {
            int value;
        };

        Action<const UserData &> action;
        action.add([](const UserData &ud, void *data) {});
        action.add([](const UserData &ud, void *data) {});
        action.add([](const UserData &ud, void *data) {});
        action.add([](const UserData &ud, void *data) {});
        CHECK(action.size() == 4);

        action.clear();

        CHECK(action.empty());
    }

    TEST_CASE("priority order")
    {
        List<Int> priorityTester{};
        Action<Int> action;

        SUBCASE("Regular order")
        {
            action.add([](Int n, void *data) {
                auto list = static_cast<List<Int> *>(data);
                list->emplace_back(n);
            }, &priorityTester, 10);

            action.add([](Int n, void *data) {
                auto list = static_cast<List<Int> *>(data);
                list->emplace_back(2 * n);
            }, &priorityTester, 15);

            action.add([](Int n, void *data) {
                auto list = static_cast<List<Int> *>(data);
                list->emplace_back(3 * n);
            }, &priorityTester, 25);

            action(5);
            CHECK(priorityTester.size() == 3);
            CHECK(priorityTester.at(0) == 5);
            CHECK(priorityTester.at(1) == 10);
            CHECK(priorityTester.at(2) == 15);
        }

        SUBCASE("Reverse order")
        {
            action.add([](Int n, void *data) {
                auto list = static_cast<List<Int> *>(data);
                list->emplace_back(n);
            }, &priorityTester, 25);

            action.add([](Int n, void *data) {
                auto list = static_cast<List<Int> *>(data);
                list->emplace_back(2 * n);
            }, &priorityTester, 15);

            action.add([](Int n, void *data) {
                auto list = static_cast<List<Int> *>(data);
                list->emplace_back(3 * n);
            }, &priorityTester, -15);

            action(5);
            CHECK(priorityTester.size() == 3);
            CHECK(priorityTester.at(0) == 15);
            CHECK(priorityTester.at(1) == 10);
            CHECK(priorityTester.at(2) == 5);
        }

        SUBCASE("Random")
        {
            action.add([](Int n, void *data) {
                auto list = static_cast<List<Int> *>(data);
                list->emplace_back(n);
            }, &priorityTester, 15);

            action.add([](Int n, void *data) {
                auto list = static_cast<List<Int> *>(data);
                list->emplace_back(2 * n);
            }, &priorityTester, 25);

            action.add([](Int n, void *data) {
                auto list = static_cast<List<Int> *>(data);
                list->emplace_back(3 * n);
            }, &priorityTester, -15);

            action(5);
            CHECK(priorityTester.size() == 3);
            CHECK(priorityTester.at(0) == 15);
            CHECK(priorityTester.at(1) == 5);
            CHECK(priorityTester.at(2) == 10);
        }
    }
}
