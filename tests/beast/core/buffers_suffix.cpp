#include "catch.hpp"
#include <boost/beast/core/buffers_suffix.hpp>
#include <boost/beast/core/buffers_cat.hpp>
#include "test_buffer.hpp"
using namespace boost::beast;

TEST_CASE("testBufferSequence mutable", "buffers_suffix") {
    char buf[13];
    auto b = buffers_triple(buf, sizeof(buf));
    buffers_suffix<decltype(b)> bs(b);
    test_buffer_sequence(bs);
}

TEST_CASE("testBufferSequence const", "buffers_suffix") {
    string_view src = "Hello, world!";
    std::array<net::const_buffer, 3> b{ {
        net::const_buffer(src.data(),     3),
        net::const_buffer(src.data() + 3, 4),
        net::const_buffer(src.data() + 7, 6) } };
    buffers_suffix<decltype(b)> bs(b);
    test_buffer_sequence(bs);
}

TEST_CASE("testSpecial default construction", "buffers_suffix") {
    class test_buffer
        : public net::const_buffer
    {
    public:
        test_buffer()
            : net::const_buffer("\r\n", 2)
        {
        }
    };

    buffers_suffix<test_buffer> cb;
    REQUIRE(buffers_to_string(cb) == "\r\n");
    cb.consume(1);
    REQUIRE(buffers_to_string(cb) == "\n");
}

TEST_CASE("testSpecial in-place init", "buffers_suffix") {
    buffers_suffix<buffers_cat_view<
        net::const_buffer,
        net::const_buffer>> cb(
            in_place_init,
            net::const_buffer("\r", 1),
            net::const_buffer("\n", 1));
    REQUIRE(buffers_to_string(cb) == "\r\n");
}

TEST_CASE("testSpecial empty sequence", "buffers_suffix") {
    buffers_suffix<net::mutable_buffer> cb(
        net::mutable_buffer{});
    REQUIRE(buffer_bytes(cb) == 0);
    buffers_suffix<net::mutable_buffer> cb2(
        net::mutable_buffer{});
    REQUIRE(net::buffer_copy(cb2, cb) == 0);
}

template<class BufferSequence>
static
buffers_suffix<BufferSequence>
consumed_buffers(BufferSequence const& bs, std::size_t n)
{
    buffers_suffix<BufferSequence> cb(bs);
    cb.consume(n);
    return cb;
}

template<class Buffers1, class Buffers2>
static
bool
eq(Buffers1 const& lhs, Buffers2 const& rhs)
{
    return
        buffers_to_string(lhs) ==
        buffers_to_string(rhs);
}

TEST_CASE("testMatrix", "buffers_suffix") {
    char buf[12];
    std::string const s = "Hello, world";
    REQUIRE(s.size() == sizeof(buf));
    net::buffer_copy(net::buffer(buf), net::buffer(s));
    REQUIRE(buffers_to_string(net::buffer(buf)) == s);
    for (std::size_t i = 1; i < 4; ++i) {
        for (std::size_t j = 1; j < 4; ++j) {
            for (std::size_t x = 1; x < 4; ++x) {
                for (std::size_t y = 1; y < 4; ++y) {
                    std::size_t k = sizeof(buf) - (i + j);
                    std::size_t z = sizeof(buf) - (x + y);
                    {
                        std::array<net::const_buffer, 3> bs{ {
                            net::const_buffer{&buf[0], i},
                            net::const_buffer{&buf[i], j},
                            net::const_buffer{&buf[i + j], k}} };
                        buffers_suffix<decltype(bs)> cb(bs);
                        REQUIRE(buffers_to_string(cb) == s);
                        REQUIRE(buffer_bytes(cb) == s.size());
                        cb.consume(0);
                        REQUIRE(eq(cb, consumed_buffers(bs, 0)));
                        REQUIRE(buffers_to_string(cb) == s);
                        REQUIRE(buffer_bytes(cb) == s.size());
                        cb.consume(x);
                        REQUIRE(buffers_to_string(cb) == s.substr(x));
                        REQUIRE(eq(cb, consumed_buffers(bs, x)));
                        cb.consume(y);
                        REQUIRE(buffers_to_string(cb) == s.substr(x + y));
                        REQUIRE(eq(cb, consumed_buffers(bs, x + y)));
                        cb.consume(z);
                        REQUIRE(buffers_to_string(cb) == "");
                        REQUIRE(eq(cb, consumed_buffers(bs, x + y + z)));
                        cb.consume(1);
                        REQUIRE(buffers_to_string(cb) == "");
                        REQUIRE(eq(cb, consumed_buffers(bs, x + y + z)));
                    }
                }
            }
        }
    }
}