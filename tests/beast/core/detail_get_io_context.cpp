#include "catch.hpp"
#include <boost/beast/core/detail/get_io_context.hpp>
#include <boost/asio/strand.hpp>

TEST_CASE("test get_io_contex", "get_io_context") {
    using namespace boost::beast::detail;
    struct none
    {
    };

    boost::asio::io_context ioc;
    REQUIRE(get_io_context(5) == nullptr);
    REQUIRE(get_io_context(none{}) == nullptr);
    REQUIRE(get_io_context(ioc) == &ioc);
    REQUIRE(get_io_context(ioc.get_executor()) == &ioc);
    REQUIRE(get_io_context(boost::asio::make_strand(ioc)) == &ioc);
    REQUIRE(get_io_context(boost::asio::any_io_executor(ioc.get_executor())) == &ioc);
}