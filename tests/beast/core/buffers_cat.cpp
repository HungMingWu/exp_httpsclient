#include "catch.hpp"

#include <boost/beast/core/buffers_cat.hpp>
#include <boost/beast/core/buffer_traits.hpp>
#include <boost/beast/core/buffers_prefix.hpp>
#include <boost/beast/core/buffers_suffix.hpp>
#include <boost/asio/buffer.hpp>
#include <iterator>
#include <list>
#include <string_view>
#include <type_traits>
#include <vector>
#include "test_buffer.hpp"

#if 0
TEST_CASE("DefaultIterators", "buffer_cat") {
    // default ctor is one past the end
    char c[2] = {};
    auto bs = boost::beast::buffers_cat(
        boost::asio::const_buffer(&c[0], 1),
        boost::asio::const_buffer(&c[1], 1));
    decltype(bs)::const_iterator it;
    decltype(bs)::const_iterator it2;
    REQUIRE(it == it2);
    REQUIRE(it2 == it);
    it = bs.end();
    it2 = bs.end();
    REQUIRE(it == it2);
    REQUIRE(it2 == it);
    decltype(bs)::const_iterator it3(it2);
    REQUIRE(it3 == it2);
    it = bs.begin();
    REQUIRE(it != it3);
    it = it3;
    REQUIRE(it == it3);

    // dereferencing default iterator should throw
    bool checked = false;
    try
    {
        it = {};
        (void)*it;
    }
    catch (std::logic_error const&)
    {
        checked = true;
    }
    catch (...)
    {
    }
    REQUIRE(checked);
}

#endif

TEST_CASE("BufferSequence", "buffer_cat") {
    std::string_view s = "Hello, world!";
    boost::asio::const_buffer b1(s.data(), 6);
    boost::asio::const_buffer b2(
        s.data() + b1.size(), s.size() - b1.size());
    boost::beast::test_buffer_sequence(boost::beast::buffers_cat(b1, b2));
}