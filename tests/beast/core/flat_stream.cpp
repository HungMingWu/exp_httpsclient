#include "catch.hpp"
#include <boost/beast/core/flat_stream.hpp>
#include <boost/asio/io_context.hpp>
#include "stream_tests.hpp"
#include "stream.hpp"
namespace net = boost::asio;
using namespace boost::beast;

TEST_CASE("flat_stream testMembers basic", "flat_stream") {
	test_sync_stream<flat_stream<test::stream>>();

	test_async_stream<flat_stream<test::stream>>();
}

TEST_CASE("flat_stream testMembers read/write", "flat_stream") {
    net::io_context ioc;
    error_code ec;
    flat_stream<test::stream> s(ioc);
    {
        // VFALCO Hack to make test stream code = eof
        test::stream ts(ioc);
        s.next_layer().connect(ts);
    }
    char buf[1];
    net::mutable_buffer m1 = net::buffer(buf);

    REQUIRE(s.read_some(net::mutable_buffer{}) == 0);
    REQUIRE(s.read_some(net::mutable_buffer{}, ec) == 0);
    REQUIRE(!ec);

    bool checked = false;
    try
    {
        s.read_some(m1);
    }
    catch (std::exception const&)
    {
        checked = true;
    }
    catch (...)
    {
    }
    REQUIRE(checked);

    REQUIRE(s.write_some(net::const_buffer{}) == 0);
    REQUIRE(s.write_some(net::const_buffer{}, ec) == 0);
    REQUIRE(!ec);

    checked = false;
    try
    {
        s.write_some(m1);
    }
    catch (std::exception const&)
    {
        checked = true;
    }
    catch (...)
    {
    }
    REQUIRE(checked);

    bool invoked;

    invoked = false;
    s.async_read_some(net::mutable_buffer{},
        [&](error_code ec, std::size_t)
        {
            invoked = true;
            REQUIRE(!ec);
        });
    ioc.run();
    ioc.restart();
    REQUIRE(invoked);

    invoked = false;
    s.async_write_some(net::const_buffer{},
        [&](error_code ec, std::size_t)
        {
            invoked = true;
            REQUIRE(!ec);
        });
    ioc.run();
    ioc.restart();
    REQUIRE(invoked);
}

TEST_CASE("flat_stream testMembers stack_write_some", "flat_stream") {
    net::io_context ioc;
    char b[detail::flat_stream_base::max_size];
    std::array<net::const_buffer, 3> bs;
    bs[0] = net::const_buffer(b, 100);
    bs[1] = net::const_buffer(b + 100, 200);
    bs[2] = net::const_buffer(b + 100 + 200, 300);
    REQUIRE(buffer_bytes(bs) <=
        detail::flat_stream_base::max_stack);
    flat_stream<test::stream> s(ioc);
    error_code ec;
    s.write_some(bs, ec);
}

TEST_CASE("flat_stream testMembers write_some", "flat_stream") {
    net::io_context ioc;
    char b[detail::flat_stream_base::max_size];
    std::array<net::const_buffer, 2> bs;
    bs[0] = net::const_buffer(b,
        detail::flat_stream_base::max_stack);
    bs[1] = net::const_buffer(b + bs[0].size(), 1024);
    REQUIRE(buffer_bytes(bs) <=
        detail::flat_stream_base::max_size);
    flat_stream<test::stream> s(ioc);
    error_code ec;
    s.write_some(bs, ec);
}

TEST_CASE("flat_stream testMembers async_write_some", "flat_stream") {
    net::io_context ioc;
    char b[detail::flat_stream_base::max_size];
    std::array<net::const_buffer, 2> bs;
    bs[0] = net::const_buffer(b,
        detail::flat_stream_base::max_stack);
    bs[1] = net::const_buffer(b + bs[0].size(), 1024);
    REQUIRE(buffer_bytes(bs) <=
        detail::flat_stream_base::max_size);
    flat_stream<test::stream> s(ioc);
    s.async_write_some(bs,
        [](error_code, std::size_t)
        {
        });
}

TEST_CASE("flat_stream testMembers teardown 1", "flat_stream") {
    net::io_context ioc;
    test::stream ts(ioc);
    flat_stream<test::stream> s(ioc);
    ts.connect(s.next_layer());
    error_code ec;
    teardown(role_type::client, s, ec);
}

TEST_CASE("flat_stream testMembers teardown 2", "flat_stream") {
    net::io_context ioc;
    test::stream ts(ioc);
    flat_stream<test::stream> s(ioc);
    ts.connect(s.next_layer());
    async_teardown(role_type::client, s,
        [](error_code)
        {
        });
}

TEST_CASE("flat_stream testSplit", "flat_stream") {
    auto const check =
        [&](
            std::initializer_list<int> v0,
            std::size_t limit,
            unsigned long count,
            bool copy)
    {
        std::vector<net::const_buffer> v;
        v.reserve(v0.size());
        for (auto const n : v0)
            v.emplace_back("", n);
        auto const result =
            boost::beast::detail::flat_stream_base::flatten(v, limit);
        REQUIRE(result.size == count);
        REQUIRE(result.flatten == copy);
        return result;
    };
    check({}, 1, 0, false);
    check({ 1,2 }, 1, 1, false);
    check({ 1,2 }, 2, 1, false);
    check({ 1,2 }, 3, 3, true);
    check({ 1,2 }, 4, 3, true);
    check({ 1,2,3 }, 1, 1, false);
    check({ 1,2,3 }, 2, 1, false);
    check({ 1,2,3 }, 3, 3, true);
    check({ 1,2,3 }, 4, 3, true);
    check({ 1,2,3 }, 7, 6, true);
    check({ 1,2,3,4 }, 3, 3, true);
}

#if BOOST_ASIO_HAS_CO_AWAIT
static_assert(std::is_same_v<
        net::awaitable<std::size_t>, decltype(
            stream.async_read_some(rxbuf, net::use_awaitable))>);

static_assert(std::is_same_v<
        net::awaitable<std::size_t>, decltype(
            stream.async_write_some(txbuf, net::use_awaitable))>);
#endif