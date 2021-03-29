#include "catch.hpp"
#include <boost/beast/core/detail/clamp.hpp>
TEST_CASE("test clamp function", "detail_clamp") {
    REQUIRE(boost::beast::detail::clamp(
        (std::numeric_limits<std::uint64_t>::max)()) ==
        (std::numeric_limits<std::size_t>::max)());
}