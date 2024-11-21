#include <doctest/doctest.h>
#include <kaze/core/debug.h>

USING_KAZE_NAMESPACE;

TEST_SUITE("debug")
{
    TEST_CASE("Set and get debug message")
    {
        CHECK(getError().message == "");
        CHECK( !hasError() );
        setError("Error123", Error::RuntimeErr);

        CHECK(hasError());
        CHECK(getError().message == "Error123");

        clearError();

        CHECK(getError().message == "");
        CHECK( !hasError() );
    }
}
