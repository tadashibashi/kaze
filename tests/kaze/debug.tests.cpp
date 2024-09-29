#include <doctest/doctest.h>
#include <kaze/kaze.h>
#include <kaze/debug.h>

TEST_SUITE("debug")
{
    TEST_CASE("Set and get debug message")
    {
        CHECK(kaze::getError() == "");
        CHECK( !kaze::hasError() );
        kaze::setError("Error123");

        CHECK(kaze::getError() == "Error123");
        CHECK(kaze::hasError());

        kaze::clearError();

        CHECK(kaze::getError() == "");
        CHECK( !kaze::hasError() );
    }
}
