#include "catch.hpp"
#include <string_view>
#include <boost/asio/buffer.hpp>
#include <boost/beast/core/buffers_range.hpp>
#include "test_buffer.hpp"
namespace net = boost::asio;
using namespace boost::beast;
static_assert(
    is_const_buffer_sequence<
    decltype(buffers_range(
        std::declval<net::const_buffer>()))>::value);

static_assert(
    is_const_buffer_sequence<
    decltype(buffers_range(
        std::declval<net::mutable_buffer>()))>::value);

static_assert(
    !net::is_mutable_buffer_sequence<
    decltype(buffers_range(
        std::declval<net::const_buffer>()))>::value);

static_assert(
    net::is_mutable_buffer_sequence<
    decltype(buffers_range(
        std::declval<net::mutable_buffer>()))>::value);

TEST_CASE("testBufferSequence 1", "buffers_range") {
    std::string_view s = "Hello, world!";
    test_buffer_sequence(buffers_range(
        net::const_buffer{ s.data(), s.size() }));
}

TEST_CASE("testBufferSequence 2", "buffers_range") {
    char buf[13];
    test_buffer_sequence(
        buffers_range(net::mutable_buffer{
            buf, sizeof(buf) }));
}