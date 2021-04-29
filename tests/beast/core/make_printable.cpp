#include "catch.hpp"
#include <string_view>
#include <boost/asio/buffer.hpp>
#include <boost/beast/core/make_printable.hpp>
#include "test_buffer.hpp"
namespace net = boost::asio;
using namespace boost::beast;

TEST_CASE("testMakePrintable", "make_printable") {
    char buf[13];
    buffers_triple b(buf, sizeof(buf));
    std::string_view src = "Hello, world!";
    REQUIRE(src.size() == sizeof(buf));
    net::buffer_copy(b,
        net::const_buffer(src.data(), src.size()));
    std::ostringstream ss;
    ss << make_printable(b);
    REQUIRE(ss.str() == src);
}