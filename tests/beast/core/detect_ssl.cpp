#include "catch.hpp"
#include <string_view>
#include <boost/beast/core/detect_ssl.hpp>
#include <boost/beast/core/flat_buffer.hpp>
#include "handler.hpp"
#include "stream.hpp"

using namespace std;
using namespace boost::beast;
TEST_CASE("testDetect", "detect_ssl") {
    auto const yes =
        [](int n, string_view s)
    {
        REQUIRE(detail::is_tls_client_hello(
            net::const_buffer(s.data(), n)));
    };

    auto const no =
        [](int n, string_view s)
    {
        REQUIRE(!*detail::is_tls_client_hello(
            net::const_buffer(s.data(), n)));
    };

    auto const maybe =
        [](int n, string_view s)
    {
        REQUIRE(!detail::is_tls_client_hello(
                net::const_buffer(s.data(), n)));
    };

    maybe(0, "\x00\x00\x00\x00\x00\x00\x00\x00\x00");
    no(1, "\x01\x00\x00\x00\x00\x00\x00\x00\x00");
    maybe(1, "\x16\x00\x00\x00\x00\x00\x00\x00\x00");
    maybe(4, "\x16\x00\x00\x00\x00\x00\x00\x00\x00");
    no(5, "\x16\x00\x00\x00\x00\x00\x00\x00\x00");
    maybe(5, "\x16\x00\x00\x01\x00\x00\x00\x00\x00");
    no(8, "\x16\x00\x00\x01\x00\x00\x00\x00\x00");
    maybe(8, "\x16\x00\x00\x01\x00\x01\x00\x00\x00");
    no(9, "\x16\x00\x00\x01\x00\x01\x01\x00\x00");
    yes(9, "\x16\x00\x00\x01\x00\x01\x00\x00\x00");
}

TEST_CASE("testRead true 1", "detect_ssl") {
    net::io_context ioc;

    boost::system::error_code ec;
    flat_buffer b;
    test::stream s1(ioc);
    s1.append({ "\x16\x00\x00\x01\x00\x01\x00\x00\x00", 9 });
    auto result = detect_ssl(s1, b, ec);
    REQUIRE(result == true);
    REQUIRE(!ec);
}

TEST_CASE("testRead true 2", "detect_ssl") {
    net::io_context ioc;

    boost::system::error_code ec;
    flat_buffer b;
    test::stream s1(ioc);
    auto s2 = test::connect(s1);
    s1.append({ "\x16\x00\x00\x01\x00\x01\x00\x00\x00", 9 });
    s2.close();
    auto result = detect_ssl(s1, b, ec);
    REQUIRE(result == true);
    REQUIRE(!ec);
}

TEST_CASE("testRead false", "detect_ssl") {
    net::io_context ioc;

    boost::system::error_code ec;
    flat_buffer b;
    test::stream s1(ioc);
    s1.append({ "\x16\x00\x00\x01\x00\x01\x01\x00\x00", 9 });
    auto result = detect_ssl(s1, b, ec);
    REQUIRE(result == false);
    REQUIRE(!ec);
}

TEST_CASE("testRead eof", "detect_ssl") {
    net::io_context ioc;

    boost::system::error_code ec;
    flat_buffer b;
    test::stream s1(ioc);
    auto s2 = test::connect(s1);
    s1.append({ "\x16\x00\x00\x01\x00", 5 });
    s2.close();
    auto result = detect_ssl(s1, b, ec);
    REQUIRE(result == false);
    REQUIRE(ec);
}

TEST_CASE("testAsyncRead true 1", "detect_ssl") {
    net::io_context ioc;

    flat_buffer b;
    test::stream s1(ioc);
    s1.append({ "\x16\x00\x00\x01\x00\x01\x00\x00\x00", 9 });
    async_detect_ssl(s1, b, test::success_handler());
    test::run(ioc);
}

TEST_CASE("testAsyncRead true 2", "detect_ssl") {
    net::io_context ioc;

    flat_buffer b;
    test::stream s1(ioc);
    auto s2 = test::connect(s1);
    s1.append({ "\x16\x00\x00\x01\x00\x01\x00\x00\x00", 9 });
    s2.close();
    async_detect_ssl(s1, b, test::success_handler());
    test::run(ioc);
}

TEST_CASE("testAsyncRead false", "detect_ssl") {
    net::io_context ioc;

    flat_buffer b;
    test::stream s1(ioc);
    s1.append({ "\x16\x00\x00\x01\x00\x01\x01\x00\x00", 9 });
    async_detect_ssl(s1, b, test::success_handler());
    test::run(ioc);
}

TEST_CASE("testAsyncRead eof", "detect_ssl") {
    net::io_context ioc;

    flat_buffer b;
    test::stream s1(ioc);
    auto s2 = test::connect(s1);
    s1.append({ "\x16\x00\x00\x01\x00", 5 });
    s2.close();
    async_detect_ssl(s1, b,
        test::fail_handler(net::error::eof));
    test::run(ioc);
}

#if BOOST_ASIO_HAS_CO_AWAIT
static_assert(
    std::is_same_v<
        net::awaitable<bool>, decltype(
            async_detect_ssl(stream, b, net::use_awaitable))>);
#endif