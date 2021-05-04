#include "catch.hpp"
#include <boost/beast/core/buffers_adaptor.hpp>
#include <boost/beast/core/read_size.hpp>
#include <asio/streambuf.hpp>
#include "test_buffer.hpp"
using namespace boost::beast;
namespace net = asio;
static_assert(MutableDynamicBuffer<buffers_adaptor<buffers_triple>>);

TEST_CASE("buffers_adaptor testDynamicBuffer", "buffers_adaptor") {
    char s[13];
    buffers_triple tb(s, sizeof(s));
    buffers_adaptor<buffers_triple> b(tb);
    test_dynamic_buffer(b);
}

TEST_CASE("testSpecial", "buffers_adaptor") {
    char s1[13];
    buffers_triple tb1(s1, sizeof(s1));
    REQUIRE(buffer_bytes(tb1) == sizeof(s1));

    char s2[15];
    buffers_triple tb2(s2, sizeof(s2));
    REQUIRE(buffer_bytes(tb2) == sizeof(s2));

    {
        // construction

        buffers_adaptor<buffers_triple> b1(tb1);
        REQUIRE(b1.value() == tb1);

        buffers_adaptor<buffers_triple> b2(tb2);
        REQUIRE(b2.value() == tb2);

        buffers_adaptor<buffers_triple> b3(b2);
        REQUIRE(b3.value() == tb2);

        char s3[15];
        buffers_adaptor<buffers_triple> b4(
            boost::beast::in_place_init, s3, sizeof(s3));
        REQUIRE(b4.value() == buffers_triple(s3, sizeof(s3)));

        // assignment

        b3 = b1;
        REQUIRE(b3.value() == tb1);
    }
}

TEST_CASE("testIssue386", "buffers_adaptor") {
    using type = net::streambuf;
    type buffer;
    buffers_adaptor<
        type::mutable_buffers_type> ba{ buffer.prepare(512) };
    read_size(ba, 1024);
}

namespace boost::beast {
    struct buffers_adaptor_test_hook
    {
        template<class MutableBufferSequence>
        static
            auto
            make_subrange(
                buffers_adaptor <MutableBufferSequence>& adaptor,
                std::size_t pos = 0,
                std::size_t n = (std::numeric_limits<std::size_t>::max)())
            -> typename buffers_adaptor<MutableBufferSequence>::mutable_buffers_type
        {
            return adaptor.make_subrange(pos, n);
        }

        template<class MutableBufferSequence>
        static
            auto
            make_subrange(
                buffers_adaptor<MutableBufferSequence> const& adaptor,
                std::size_t pos = 0,
                std::size_t n = (std::numeric_limits<std::size_t>::max)())
            -> typename buffers_adaptor<MutableBufferSequence>::const_buffers_type
        {
            return adaptor.make_subrange(pos, n);
        }
    };
}

template <bool isMutable>
void testSubrange()
{
    std::string s =
        "the quick brown fox jumps over the lazy dog";

    auto iterate_test = [&](
        std::size_t a,
        std::size_t b,
        std::size_t c)
    {
        auto buffers = std::vector<net::mutable_buffer>();
        if (a)
            buffers.push_back(net::buffer(&s[0], a));
        if (b - a)
            buffers.push_back(net::buffer(&s[a], (b - a)));
        if (c - b)
            buffers.push_back(net::buffer(&s[b], (c - b)));
        auto adapter = buffers_adaptor<std::vector<net::mutable_buffer>>(buffers);

        using maybe_mutable =
            typename std::conditional<
            isMutable,
            buffers_adaptor<std::vector<net::mutable_buffer>>&,
            buffers_adaptor<std::vector<net::mutable_buffer>> const&>::type;

        auto sub = buffers_adaptor_test_hook::make_subrange(static_cast<maybe_mutable>(adapter));
        /*
        using value_type = typename std::conditional<
            isMutable, net::mutable_buffer, net::const_buffer>::type;
        BEAST_EXPECTS(typeid(typename decltype(sub)::value_type) == typeid(value_type), "iterate_test");
        */
        REQUIRE(buffers_to_string(sub) == s.substr(0, c));
    };

    iterate_test(0, 0, 1);

    for (std::size_t a = 0; a <= s.size(); ++a)
        for (std::size_t b = a; b <= s.size(); ++b)
            for (std::size_t c = b; c <= s.size(); ++c)
                iterate_test(a, b, c);
}

TEST_CASE("testSubrange<true> test", "buffers_adaptor") {
    testSubrange<true>();
}

TEST_CASE("testSubrange<false> test", "buffers_adaptor") {
    testSubrange<false>();
}