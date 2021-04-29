#include "catch.hpp"
#include <boost/beast/core/flat_static_buffer.hpp>
#include <boost/beast/core/ostream.hpp>
#include <boost/beast/core/read_size.hpp>
#include "test_buffer.hpp"
using namespace boost::beast;

static_assert(is_mutable_dynamic_buffer<flat_static_buffer<13>>::value);

TEST_CASE("flat_static_buffer testDynamicBuffer", "flat_static_buffer") {
	test_dynamic_buffer(flat_static_buffer<13>{});
}

TEST_CASE("testMembers flat_static_buffer_base", "flat_static_buffer") {
    string_view const s = "Hello, world!";
    char buf[64];
    flat_static_buffer_base b{ buf, sizeof(buf) };
    ostream(b) << s;
    REQUIRE(buffers_to_string(b.data()) == s);
    b.clear();
    REQUIRE(b.size() == 0);
    REQUIRE(buffer_bytes(b.data()) == 0);
}

TEST_CASE("testMembers flat_static_buffer", "flat_static_buffer") {
    string_view const s = "Hello, world!";
    flat_static_buffer<64> b1;
    REQUIRE(b1.size() == 0);
    REQUIRE(b1.max_size() == 64);
    REQUIRE(b1.capacity() == 64);
    ostream(b1) << s;
    REQUIRE(buffers_to_string(b1.data()) == s);
    {
        flat_static_buffer<64> b2{ b1 };
        REQUIRE(buffers_to_string(b2.data()) == s);
        b2.consume(7);
        REQUIRE(buffers_to_string(b2.data()) == s.substr(7));
    }
    {
        flat_static_buffer<64> b2;
        b2 = b1;
        REQUIRE(buffers_to_string(b2.data()) == s);
        b2.consume(7);
        REQUIRE(buffers_to_string(b2.data()) == s.substr(7));
    }
}

TEST_CASE("testMembers cause memmove", "flat_static_buffer") {
    flat_static_buffer<10> b;
    ostream(b) << "12345";
    b.consume(3);
    ostream(b) << "67890123";
    REQUIRE(buffers_to_string(b.data()) == "4567890123");
    bool checked = false;
    try
    {
        b.prepare(1);
    }
    catch (std::length_error const&)
    {
        checked = true;
    }
    REQUIRE(checked);
}

TEST_CASE("testMembers read_size", "flat_static_buffer") {
    flat_static_buffer<10> b;
    REQUIRE(read_size(b, 512) == 10);
    b.prepare(4);
    b.commit(4);
    REQUIRE(read_size(b, 512) == 6);
    b.consume(2);
    REQUIRE(read_size(b, 512) == 8);
    b.prepare(8);
    b.commit(8);
    REQUIRE(read_size(b, 512) == 0);
}

TEST_CASE("testMembers base", "flat_static_buffer") {
    flat_static_buffer<10> b;
    [&](flat_static_buffer_base& base)
    {
        REQUIRE(base.max_size() == b.capacity());
    }
    (b.base());

    [&](flat_static_buffer_base const& base)
    {
        REQUIRE(base.max_size() == b.capacity());
    }
    (b.base());
}