#include "catch.hpp"
#include <boost/beast/core/detail/varint.hpp>
#include <boost/assert.hpp>

TEST_CASE("detail varint test", "detail_varint") {
    std::size_t n0 = 0;
    std::size_t n1 = 1;
    for (;;)
    {
        char buf[16];
        BOOST_ASSERT(sizeof(buf) >= boost::beast::detail::varint_size(n0));
        auto it = &buf[0];
        boost::beast::detail::varint_write(it, n0);
        it = &buf[0];
        auto n = boost::beast::detail::varint_read(it);
        REQUIRE(n == n0);
        n = n0 + n1;
        if (n < n1)
            break;
        n0 = n1;
        n1 = n;
    }
}