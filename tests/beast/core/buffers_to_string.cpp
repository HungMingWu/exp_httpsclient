#include "catch.hpp"
#include <boost/beast/core/buffers_to_string.hpp>
#include <boost/beast/core/multi_buffer.hpp>
#include <boost/beast/core/ostream.hpp>

using namespace boost::beast;
TEST_CASE("buffers_to_string", "buffers_to_string") {
    multi_buffer b;
    ostream(b) << "Hello, ";
    REQUIRE(buffers_to_string(b.data()) == "Hello, ");
    ostream(b) << "world!";
    REQUIRE(buffers_to_string(b.data()) == "Hello, world!");
}