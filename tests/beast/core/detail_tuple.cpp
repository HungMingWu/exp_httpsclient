#include "catch.hpp"
#include <optional>
#include <boost/beast/core/detail/tuple.hpp>
#include <utility>
#include <string>
TEST_CASE("detail tuple test", "detail_tuple") {
    struct explicit_constructible
    {
        explicit_constructible(std::nullptr_t)
            : i_(0)
        {
        }

        explicit explicit_constructible(int i)
            : i_(i)
        {
        }

        int i_;
    };
    using namespace boost::beast::detail;
    tuple<explicit_constructible, int> t{ nullptr, 42 };
    REQUIRE(t.get<1>() == 42);
    REQUIRE(t.get<0>().i_ == 0);

    t = tuple<explicit_constructible, int>{ explicit_constructible(42), 43 };
    REQUIRE(t.get<1>() == 43);
    REQUIRE(t.get<0>().i_ == 42);
}