#include "catch.hpp"
#include <boost/beast/core/buffers_prefix.hpp>
#include "test_buffer.hpp"
using namespace boost::beast;
TEST_CASE("testBufferSequence", "buffers_prefix") {
    char buf[13];
    auto const b =
        buffers_triple(buf, sizeof(buf));
    for (std::size_t i = 1; i <= sizeof(buf); ++i)
        test_buffer_sequence(
            buffers_prefix(i, b));
}

TEST_CASE("testInPlaceInit 1", "buffers_prefix") {
    class test_buffers
    {
        net::const_buffer cb_;

    public:
        using const_iterator =
            net::const_buffer const*;

        explicit
            test_buffers(std::true_type)
        {
        }

        const_iterator
            begin() const
        {
            return &cb_;
        }

        const_iterator
            end() const
        {
            return begin() + 1;
        }
    };
    buffers_prefix_view<test_buffers> v(
        2, in_place_init, std::true_type{});
    REQUIRE(buffer_bytes(v) == 0);
}

TEST_CASE("testInPlaceInit 2", "buffers_prefix") {
    char c[2];
    c[0] = 0;
    c[1] = 0;
    buffers_prefix_view<net::const_buffer> v(
        2, in_place_init, c, sizeof(c));
    REQUIRE(buffer_bytes(v) == 2);
}

TEST_CASE("testInPlaceInit 3", "buffers_prefix") {
    char c[2];
    buffers_prefix_view<net::mutable_buffer> v(
        2, in_place_init, c, sizeof(c));
    REQUIRE(buffer_bytes(v) == 2);
}

template<class BufferType>
void
testPrefixes()
{
    std::string s = "Hello, world";
    REQUIRE(s.size() == 12);
    for (std::size_t x = 1; x < 4; ++x) {
        for (std::size_t y = 1; y < 4; ++y) {
            {
                std::size_t z = s.size() - (x + y);
                std::array<BufferType, 3> bs{ {
                    BufferType{&s[0], x},
                    BufferType{&s[x], y},
                    BufferType{&s[x + y], z}} };
                for (std::size_t i = 0; i <= s.size() + 1; ++i)
                {
                    auto pb = buffers_prefix(i, bs);
                    REQUIRE(buffers_to_string(pb) == s.substr(0, i));
                    auto pb2 = pb;
                    REQUIRE(buffers_to_string(pb2) == buffers_to_string(pb));
                    pb = buffers_prefix(0, bs);
                    pb2 = pb;
                    REQUIRE(buffer_bytes(pb2) == 0);
                    pb2 = buffers_prefix(i, bs);
                    REQUIRE(buffers_to_string(pb2) == s.substr(0, i));
                }
            }
        }
    }
}

TEST_CASE("testPrefixes type is net::const_buffer", "buffers_prefix") {
    testPrefixes<net::const_buffer>();
}

TEST_CASE("testPrefixes type is net::mutable_buffer", "buffers_prefix") {
    testPrefixes<net::mutable_buffer>();
}

TEST_CASE("testEmpty", "buffers_prefix") {
    auto pb0 = buffers_prefix(0, net::mutable_buffer{});
    REQUIRE(buffer_bytes(pb0) == 0);
    auto pb1 = buffers_prefix(1, net::mutable_buffer{});
    REQUIRE(buffer_bytes(pb1) == 0);
    REQUIRE(net::buffer_copy(pb0, pb1) == 0);
}

TEST_CASE("testBuffersFront 1", "buffers_prefix") {
    std::array<net::const_buffer, 2> v;
    v[0] = { "", 0 };
    v[1] = net::const_buffer("Hello, world!", 13);
    REQUIRE(buffers_front(v).size() == 0);
    std::swap(v[0], v[1]);
    REQUIRE(buffers_front(v).size() == 13);
}

TEST_CASE("testBuffersFront 2", "buffers_prefix") {
    struct null_sequence
    {
        net::const_buffer b;
        using iterator = net::const_buffer const*;
        iterator begin() const noexcept
        {
            return &b;
        }
        iterator end() const noexcept
        {
            return begin();
        }
    };
    null_sequence z;
    REQUIRE(buffers_front(z).size() == 0);
}
