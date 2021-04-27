#include "catch.hpp"
#include <boost/beast/core/error.hpp>
using namespace boost::beast;

void check(error e)
{
    auto const ec = make_error_code(e);
    ec.category().name();
    REQUIRE(!ec.message().empty());
}

void check(condition c, error e)
{
    {
        auto const ec = make_error_code(e);
        REQUIRE(ec.category().name() != nullptr);
        REQUIRE(!ec.message().empty());
        REQUIRE(ec == c);
    }
    {
        auto ec = make_error_condition(c);
        REQUIRE(ec.category().name() != nullptr);
        REQUIRE(!ec.message().empty());
        REQUIRE(ec == c);
    }
}

TEST_CASE("check 1", "error") {
    check(error::timeout);
}

TEST_CASE("check 2", "error") {
    check(condition::timeout, error::timeout);
}