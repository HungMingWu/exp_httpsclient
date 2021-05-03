#include "catch.hpp"
#include <asio/error.hpp>
#include <boost/beast/core/detail/buffer.hpp>
#include <boost/beast/core/flat_buffer.hpp>

namespace net = asio;

TEST_CASE("test flat_buffer prepare", "detail_buffer") {
#ifndef BOOST_NO_EXCEPTIONS
    std::error_code ec;
    boost::beast::flat_buffer b(32);
    boost::beast::detail::dynamic_buffer_prepare(b, 20, ec,
        net::error::eof);
    REQUIRE(!ec);
    b.commit(20);
    auto const result =
        boost::beast::detail::dynamic_buffer_prepare(b, 20, ec,
            net::error::eof);
    REQUIRE(result == std::nullopt);
    REQUIRE(ec == net::error::eof);
#endif
}

TEST_CASE("test flat_buffer prepare noexcept", "detail_buffer") {
    std::error_code ec;
    boost::beast::flat_buffer b(32);
    boost::beast::detail::dynamic_buffer_prepare(b, 20, ec,
        net::error::eof);

    boost::beast::detail::dynamic_buffer_prepare_noexcept(b, 20, ec,
        net::error::eof);
    REQUIRE(!ec);
    b.commit(20);
    auto const result =
        boost::beast::detail::dynamic_buffer_prepare_noexcept(b, 20, ec,
            net::error::eof);
    REQUIRE(result == std::nullopt);
    REQUIRE(ec == net::error::eof);
}