#include "catch.hpp"
#include <boost/beast/core/flat_buffer.hpp>
#include <boost/beast/core/ostream.hpp>
#include <boost/beast/core/read_size.hpp>
#include "test_buffer.hpp"
#include "test_allocator.hpp"
using namespace boost::beast;

TEST_CASE("flat_buffer testDynamicBuffer", "flat_buffer") {
    flat_buffer b(30);
    REQUIRE(b.max_size() == 30);
    test_dynamic_buffer(b);
}

TEST_CASE("testSpecialMembers construction 1", "flat_buffer") {
    flat_buffer b;
    REQUIRE(b.capacity() == 0);
}

TEST_CASE("testSpecialMembers construction 2", "flat_buffer") {
    flat_buffer b{ 500 };
    REQUIRE(b.capacity() == 0);
    REQUIRE(b.max_size() == 500);
}

using a_t = test::test_allocator<char,
    true, true, true, true, true>;

// Equal == false
using a_neq_t = test::test_allocator<char,
    false, true, true, true, true>;

TEST_CASE("testSpecialMembers construction 3", "flat_buffer") {
    a_neq_t a1;
    basic_flat_buffer<a_neq_t> b{ a1 };
    REQUIRE(b.get_allocator() == a1);
    a_neq_t a2;
    REQUIRE(b.get_allocator() != a2);
}

TEST_CASE("testSpecialMembers construction 4", "flat_buffer") {
    a_neq_t a;
    basic_flat_buffer<a_neq_t> b{ 500, a };
    REQUIRE(b.capacity() == 0);
    REQUIRE(b.max_size() == 500);
}

TEST_CASE("testSpecialMembers move construction 1", "flat_buffer") {
    flat_buffer b1;
    ostream(b1) << "Hello";
    flat_buffer b2;
    b2 = std::move(b1);
    REQUIRE(b1.size() == 0);
    REQUIRE(b1.capacity() == 0);
    REQUIRE(buffers_to_string(b2.data()) == "Hello");
}

TEST_CASE("testSpecialMembers move construction 2", "flat_buffer") {
    basic_flat_buffer<a_t> b1{ 30 };
    ostream(b1) << "Hello";
    a_t a;
    basic_flat_buffer<a_t> b2{ std::move(b1), a };
    REQUIRE(b1.size() == 0);
    REQUIRE(b1.capacity() == 0);
    REQUIRE(buffers_to_string(b2.data()) == "Hello");
    REQUIRE(b1.max_size() == b2.max_size());
}

TEST_CASE("testSpecialMembers move construction 3", "flat_buffer") {
    basic_flat_buffer<a_neq_t> b1{ 30 };
    ostream(b1) << "Hello";
    a_neq_t a;
    basic_flat_buffer<a_neq_t> b2{ std::move(b1), a };
    REQUIRE(b1.size() != 0);
    REQUIRE(b1.capacity() != 0);
    REQUIRE(buffers_to_string(b2.data()) == "Hello");
    REQUIRE(b1.max_size() == b2.max_size());
}

TEST_CASE("testSpecialMembers copy construction 1", "flat_buffer") {
    basic_flat_buffer<a_t> b1;
    ostream(b1) << "Hello";
    basic_flat_buffer<a_t> b2(b1);
    REQUIRE(b1.get_allocator() == b2.get_allocator());
    REQUIRE(buffers_to_string(b1.data()) == "Hello");
    REQUIRE(buffers_to_string(b2.data()) == "Hello");
}

TEST_CASE("testSpecialMembers copy construction 2", "flat_buffer") {
    basic_flat_buffer<a_neq_t> b1;
    ostream(b1) << "Hello";
    a_neq_t a;
    basic_flat_buffer<a_neq_t> b2(b1, a);
    REQUIRE(b1.get_allocator() != b2.get_allocator());
    REQUIRE(buffers_to_string(b1.data()) == "Hello");
    REQUIRE(buffers_to_string(b2.data()) == "Hello");
}

TEST_CASE("testSpecialMembers copy construction 3", "flat_buffer") {
    basic_flat_buffer<a_t> b1;
    ostream(b1) << "Hello";
    basic_flat_buffer<a_neq_t> b2(b1);
    REQUIRE(buffers_to_string(b1.data()) == "Hello");
    REQUIRE(buffers_to_string(b2.data()) == "Hello");
}

TEST_CASE("testSpecialMembers copy construction 4", "flat_buffer") {
    basic_flat_buffer<a_neq_t> b1;
    ostream(b1) << "Hello";
    a_t a;
    basic_flat_buffer<a_t> b2(b1, a);
    REQUIRE(b2.get_allocator() == a);
    REQUIRE(buffers_to_string(b1.data()) == "Hello");
    REQUIRE(buffers_to_string(b2.data()) == "Hello");
}

TEST_CASE("testSpecialMembers copy construction 5", "flat_buffer") {
    flat_buffer b1;
    ostream(b1) << "Hello";
    basic_flat_buffer<a_t> b2;
    b2.reserve(1);
    REQUIRE(b2.capacity() == 1);
    b2 = b1;
    REQUIRE(buffers_to_string(b2.data()) == "Hello");
    REQUIRE(b2.capacity() == b2.size());
}

TEST_CASE("testSpecialMembers move assignment 1", "flat_buffer") {
    flat_buffer b1;
    ostream(b1) << "Hello";
    flat_buffer b2;
    b2 = std::move(b1);
    REQUIRE(b1.size() == 0);
    REQUIRE(b1.capacity() == 0);
    REQUIRE(buffers_to_string(b2.data()) == "Hello");
}

TEST_CASE("testSpecialMembers move assignment 2", "flat_buffer") {
    using na_t = test::test_allocator<char,
        true, true, false, true, true>;
    basic_flat_buffer<na_t> b1;
    ostream(b1) << "Hello";
    basic_flat_buffer<na_t> b2;
    b2 = std::move(b1);
    REQUIRE(b1.get_allocator() == b2.get_allocator());
    REQUIRE(b1.size() == 0);
    REQUIRE(b1.capacity() == 0);
    REQUIRE(buffers_to_string(b2.data()) == "Hello");
}

TEST_CASE("testSpecialMembers move assignment 3", "flat_buffer") {
    using na_t = test::test_allocator<char,
        false, true, false, true, true>;
    basic_flat_buffer<na_t> b1;
    ostream(b1) << "Hello";
    basic_flat_buffer<na_t> b2;
    b2 = std::move(b1);
    REQUIRE(b1.get_allocator() != b2.get_allocator());
    REQUIRE(b1.size() != 0);
    REQUIRE(b1.capacity() != 0);
    REQUIRE(buffers_to_string(b2.data()) == "Hello");
}

TEST_CASE("testSpecialMembers move assignment 4", "flat_buffer") {
    // propagate_on_container_move_assignment : true
    using pocma_t = test::test_allocator<char,
        true, true, true, true, true>;
    basic_flat_buffer<pocma_t> b1;
    ostream(b1) << "Hello";
    basic_flat_buffer<pocma_t> b2;
    b2 = std::move(b1);
    REQUIRE(b1.size() == 0);
    REQUIRE(buffers_to_string(b2.data()) == "Hello");
}

TEST_CASE("testSpecialMembers move assignment 5", "flat_buffer") {
    // propagate_on_container_move_assignment : false
    using pocma_t = test::test_allocator<char,
        true, true, false, true, true>;
    basic_flat_buffer<pocma_t> b1;
    ostream(b1) << "Hello";
    basic_flat_buffer<pocma_t> b2;
    b2 = std::move(b1);
    REQUIRE(b1.size() == 0);
    REQUIRE(buffers_to_string(b2.data()) == "Hello");
}

TEST_CASE("testSpecialMembers copy assignment 1", "flat_buffer") {
    flat_buffer b1;
    ostream(b1) << "Hello";
    flat_buffer b2;
    b2 = b1;
    REQUIRE(buffers_to_string(b1.data()) == "Hello");
    REQUIRE(buffers_to_string(b2.data()) == "Hello");
    basic_flat_buffer<a_t> b3;
    b3 = b2;
    REQUIRE(buffers_to_string(b3.data()) == "Hello");
}

TEST_CASE("testSpecialMembers copy assignment 2", "flat_buffer") {
    // propagate_on_container_copy_assignment : true
    using pocca_t = test::test_allocator<char,
        true, true, true, true, true>;
    basic_flat_buffer<pocca_t> b1;
    ostream(b1) << "Hello";
    basic_flat_buffer<pocca_t> b2;
    b2 = b1;
    REQUIRE(buffers_to_string(b2.data()) == "Hello");
}

TEST_CASE("testSpecialMembers copy assignment 3", "flat_buffer") {
    // propagate_on_container_copy_assignment : false
    using pocca_t = test::test_allocator<char,
        true, false, true, true, true>;
    basic_flat_buffer<pocca_t> b1;
    ostream(b1) << "Hello";
    basic_flat_buffer<pocca_t> b2;
    b2 = b1;
    REQUIRE(buffers_to_string(b2.data()) == "Hello");
}

TEST_CASE("testSpecialMembers operations", "flat_buffer") {
    string_view const s = "Hello, world!";
    flat_buffer b1{ 64 };
    REQUIRE(b1.size() == 0);
    REQUIRE(b1.max_size() == 64);
    REQUIRE(b1.capacity() == 0);
    ostream(b1) << s;
    REQUIRE(buffers_to_string(b1.data()) == s);
    {
        flat_buffer b2{ b1 };
        REQUIRE(buffers_to_string(b2.data()) == s);
        b2.consume(7);
        REQUIRE(buffers_to_string(b2.data()) == s.substr(7));
    }
    {
        flat_buffer b2{ 32 };
        REQUIRE(b2.max_size() == 32);
        b2 = b1;
        REQUIRE(b2.max_size() == b1.max_size());
        REQUIRE(buffers_to_string(b2.data()) == s);
        b2.consume(7);
        REQUIRE(buffers_to_string(b2.data()) == s.substr(7));
    }
}

TEST_CASE("testSpecialMembers cause memmove", "flat_buffer") {
    flat_buffer b{ 20 };
    ostream(b) << "12345";
    b.consume(3);
    ostream(b) << "67890123";
    REQUIRE(buffers_to_string(b.data()) == "4567890123");
}

TEST_CASE("testSpecialMembers max_size", "flat_buffer") {
    flat_buffer b{ 10 };
    REQUIRE(b.max_size() == 10);
    b.max_size(32);
    REQUIRE(b.max_size() == 32);
}

TEST_CASE("testSpecialMembers allocator max_size", "flat_buffer") {
    basic_flat_buffer<a_t> b;
    auto a = b.get_allocator();
    static_assert(!std::is_const<decltype(a)>::value);
    a->max_size = 30;
    bool checked = false;
    try
    {
        b.prepare(1000);
    }
    catch (std::length_error const&)
    {
        checked = true;
    }
    REQUIRE(checked);
}

TEST_CASE("testSpecialMembers read_size", "flat_buffer") {
    flat_buffer b{ 10 };
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

TEST_CASE("testSpecialMembers swap 1", "flat_buffer") {
    basic_flat_buffer<a_neq_t> b1;
    ostream(b1) << "Hello";
    basic_flat_buffer<a_neq_t> b2;
    REQUIRE(b1.get_allocator() != b2.get_allocator());
    swap(b1, b2);
    REQUIRE(b1.get_allocator() != b2.get_allocator());
    REQUIRE(b1.size() == 0);
    REQUIRE(b1.capacity() == 0);
    REQUIRE(buffers_to_string(b2.data()) == "Hello");
}

TEST_CASE("testSpecialMembers swap 2", "flat_buffer") {
    using na_t = test::test_allocator<char,
        true, true, true, false, true>;
    na_t a1;
    basic_flat_buffer<na_t> b1{ a1 };
    na_t a2;
    ostream(b1) << "Hello";
    basic_flat_buffer<na_t> b2{ a2 };
    REQUIRE(b1.get_allocator() == a1);
    REQUIRE(b2.get_allocator() == a2);
    swap(b1, b2);
    REQUIRE(b1.get_allocator() == b2.get_allocator());
    REQUIRE(b1.size() == 0);
    REQUIRE(b1.capacity() == 0);
    REQUIRE(buffers_to_string(b2.data()) == "Hello");
}

TEST_CASE("testSpecialMembers prepare", "flat_buffer") {
    flat_buffer b{ 100 };
    b.prepare(10);
    b.commit(10);
    b.prepare(5);
    REQUIRE(b.capacity() >= 5);
    bool checked = false;
    try
    {
        b.prepare(1000);
    }
    catch (std::length_error const&)
    {
        checked = true;
    }
    REQUIRE(checked);
}

TEST_CASE("testSpecialMembers reserve", "flat_buffer") {
    flat_buffer b;
    REQUIRE(b.capacity() == 0);
    b.reserve(50);
    REQUIRE(b.capacity() == 50);
    b.prepare(20);
    b.commit(20);
    b.reserve(50);
    REQUIRE(b.capacity() == 50);

    b.max_size(b.capacity());
    b.reserve(b.max_size() + 20);
    REQUIRE(b.capacity() == 70);
    REQUIRE(b.max_size() == 70);
}

TEST_CASE("testSpecialMembers shrink to fit", "flat_buffer") {
    flat_buffer b;
    REQUIRE(b.capacity() == 0);
    b.prepare(50);
    REQUIRE(b.capacity() == 50);
    b.commit(50);
    REQUIRE(b.capacity() == 50);
    b.prepare(75);
    REQUIRE(b.capacity() >= 125);
    b.shrink_to_fit();
    REQUIRE(b.capacity() == b.size());
    b.shrink_to_fit();
    REQUIRE(b.capacity() == b.size());
    b.consume(b.size());
    REQUIRE(b.size() == 0);
    b.shrink_to_fit();
    REQUIRE(b.capacity() == 0);
}

TEST_CASE("testSpecialMembers clear", "flat_buffer") {
    flat_buffer b;
    REQUIRE(b.capacity() == 0);
    b.prepare(50);
    b.commit(50);
    REQUIRE(b.size() == 50);
    REQUIRE(b.capacity() == 50);
    b.clear();
    REQUIRE(b.size() == 0);
    REQUIRE(b.capacity() == 50);
}