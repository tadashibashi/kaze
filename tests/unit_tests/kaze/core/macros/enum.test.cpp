#include <cstring>
#include <doctest/doctest.h>
#include <kaze/core.h>
#include <sstream>

USING_KAZE_NAMESPACE;

KAZE_FLAGS_LEAN(Abilities, CanWalk, CanFly, CanSwim);

// Values should exist and have correct constant values
static_assert(Abilities::CanWalk == 1u);
static_assert(Abilities::CanFly == 1u << 1);
static_assert(Abilities::CanSwim == 1u << 2);

TEST_SUITE("Enum Flags")
{
    TEST_CASE("Enum flag assignment and constructor tests")
    {
        SUBCASE("Assignment from Flag_ sets the flag")
        {
            constexpr Abilities fishAbilities(Abilities::CanSwim);

            CHECK((fishAbilities.value & Abilities::CanSwim) > 0);
            CHECK((fishAbilities.value & Abilities::CanWalk) == 0);
            CHECK((fishAbilities.value & Abilities::CanFly) == 0);
        }

        SUBCASE("Multiple flag assignment from Flag_ sets the flags")
        {
            constexpr Abilities manAbilities = Abilities::CanWalk | Abilities::CanFly | Abilities::CanSwim;

            CHECK((manAbilities.value & Abilities::CanSwim) > 0);
            CHECK((manAbilities.value & Abilities::CanFly) > 0);
            CHECK((manAbilities.value & Abilities::CanWalk) > 0);
        }
    }

    TEST_CASE("Enum flag empty() and reset()")
    {
        SUBCASE("Check an enum set during constructor")
        {
            Abilities fishAbilities(Abilities::CanSwim);
            CHECK(!fishAbilities.empty());
            CHECK(fishAbilities.value == Abilities::CanSwim);

            fishAbilities.reset();
            CHECK(fishAbilities.value == 0);
            CHECK(fishAbilities.empty());
        }

        SUBCASE("Default enum constructor results in empty value")
        {
            Abilities fishAbilities{};
            CHECK(fishAbilities.empty());
            CHECK(fishAbilities.value == 0);
        }
    }

    TEST_CASE("Enum flag toggle()")
    {
        SUBCASE("Toggle one value")
        {
            Abilities fishAbilities{Abilities::CanSwim};

            fishAbilities.toggle(Abilities::CanSwim);
            CHECK((fishAbilities.value & Abilities::CanSwim) == 0);

            fishAbilities.toggle(Abilities::CanSwim);
            CHECK((fishAbilities.value & Abilities::CanSwim) > 0);
        }

        SUBCASE("Toggle various values with expected state")
        {
            Abilities fishAbilities{Abilities::CanSwim};

            fishAbilities.toggle(Abilities::CanFly);
            CHECK((fishAbilities.value & Abilities::CanSwim) > 0);
            CHECK((fishAbilities.value & Abilities::CanFly) > 0);
            CHECK((fishAbilities.value & Abilities::CanWalk) == 0);

            fishAbilities.toggle(Abilities::CanSwim);
            CHECK((fishAbilities.value & Abilities::CanSwim) == 0);
            CHECK((fishAbilities.value & Abilities::CanFly) > 0);
            CHECK((fishAbilities.value & Abilities::CanWalk) == 0);

            fishAbilities.toggle(Abilities::CanSwim);
            fishAbilities.toggle(Abilities::CanFly);
            CHECK((fishAbilities.value & Abilities::CanSwim) > 0);
            CHECK((fishAbilities.value & Abilities::CanFly) == 0);
            CHECK((fishAbilities.value & Abilities::CanWalk) == 0);

            fishAbilities.toggle(Abilities::CanSwim);
            fishAbilities.toggle(Abilities::CanFly);
            fishAbilities.toggle(Abilities::CanWalk);
            CHECK((fishAbilities.value & Abilities::CanSwim) == 0);
            CHECK((fishAbilities.value & Abilities::CanFly) > 0);
            CHECK((fishAbilities.value & Abilities::CanWalk) > 0);

            // Toggle all with the ~operator
            fishAbilities = ~fishAbilities;
            CHECK((fishAbilities.value & Abilities::CanSwim) > 0);
            CHECK((fishAbilities.value & Abilities::CanFly) == 0);
            CHECK((fishAbilities.value & Abilities::CanWalk) == 0);
        }
    }

    TEST_CASE("Enum flag isSet function works")
    {
        Abilities waterStriderAbilities;
        waterStriderAbilities |= Abilities::CanSwim | Abilities::CanWalk;

        CHECK(waterStriderAbilities.isSet(Abilities::CanSwim));
        CHECK(waterStriderAbilities.isSet(Abilities::CanWalk));
        CHECK(waterStriderAbilities.isSet(Abilities::CanWalk | Abilities::CanSwim));
        CHECK(!waterStriderAbilities.isSet(Abilities::CanWalk | Abilities::CanSwim | Abilities::CanFly));
        CHECK(waterStriderAbilities.isOneSet(Abilities::all()));
    }


    TEST_CASE("Enum flag set function works")
    {
        SUBCASE("Set one flag")
        {
            Abilities chickenAbilities;
            chickenAbilities.set(Abilities::CanFly, true); // wait, no I didn't mean to do that!
            CHECK((chickenAbilities.value & Abilities::CanFly) > 0);

            chickenAbilities.set(Abilities::CanFly, false); // ok, that's better
            CHECK((chickenAbilities.value & Abilities::CanFly) == 0);
        }

        SUBCASE("Set and unset multiple flag combination")
        {
            Abilities chickenAbilities;
            chickenAbilities.set(Abilities::CanSwim, true); // sort of, but not great at this task :/
            chickenAbilities.set(Abilities::CanWalk, true);

            CHECK((chickenAbilities.value & Abilities::CanWalk) > 0);
            CHECK((chickenAbilities.value & Abilities::CanSwim) > 0);
            CHECK((chickenAbilities.value & Abilities::CanFly) == 0);

            chickenAbilities.set(Abilities::CanSwim, false); // let's not stress the chicken out...
            chickenAbilities.set(Abilities::CanWalk, false); // oops, he stubbed his toe!
            chickenAbilities.set(Abilities::CanFly, true);   // so we gave him a jetpack

            CHECK((chickenAbilities.value & Abilities::CanSwim) == 0);
            CHECK((chickenAbilities.value & Abilities::CanWalk) == 0);
            CHECK((chickenAbilities.value & Abilities::CanFly) > 0);
        }
    }

    KAZE_FLAGS(FoodType, Pizza, Burger, Hotdog, Teriyaki);

    TEST_CASE("Enum flag name string tests")
    {
        SUBCASE("getName static function test")
        {
            CHECK(std::strcmp(FoodType::getName(FoodType::Pizza), "Pizza") == 0);
            CHECK(std::strcmp(FoodType::getName(FoodType::Burger), "Burger") == 0);
            CHECK(std::strcmp(FoodType::getName(FoodType::Hotdog), "Hotdog") == 0);
            CHECK(std::strcmp(FoodType::getName(FoodType::Teriyaki), "Teriyaki") == 0);
        }

        SUBCASE("Enum flag's ostream & operator<< works")
        {
            std::ostringstream os;

            os << FoodType::Pizza;
            CHECK(os.str() == "Pizza");
            os.str("");

            os << FoodType::Burger;
            CHECK(os.str() == "Burger");
            os.str("");

            os << FoodType::Hotdog;
            CHECK(os.str() == "Hotdog");
            os.str("");

            os << FoodType::Teriyaki;
            CHECK(os.str() == "Teriyaki");
            os.str("");
        }
    }

}
