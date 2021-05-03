#include "catch.hpp"
#include <asio/buffer.hpp>
#include <boost/beast/core/buffer_traits.hpp>
#include <boost/beast/core/detail/is_invocable.hpp>
namespace net = asio;
using namespace boost::beast;

// is_const_buffer_sequence

static_assert(is_const_buffer_sequence<
>::value);

static_assert(is_const_buffer_sequence<
    net::const_buffer
>::value);

static_assert(is_const_buffer_sequence<
    net::const_buffer, net::const_buffer
>::value);

static_assert(is_const_buffer_sequence<
    net::const_buffer, net::mutable_buffer
>::value);

static_assert(is_const_buffer_sequence<
    net::mutable_buffer, net::mutable_buffer
>::value);

static_assert(is_const_buffer_sequence<
    net::const_buffer const&
>::value);

static_assert(is_const_buffer_sequence<
    net::const_buffer const&, net::const_buffer const&
>::value);

static_assert(is_const_buffer_sequence<
    net::const_buffer const&, net::mutable_buffer const&
>::value);

// is_mutable_buffer_sequence

static_assert(is_mutable_buffer_sequence<
>::value);

static_assert(is_mutable_buffer_sequence<
    net::mutable_buffer
>::value);

static_assert(is_mutable_buffer_sequence<
    net::mutable_buffer, net::mutable_buffer
>::value);

static_assert(!is_mutable_buffer_sequence<
    net::const_buffer, net::const_buffer
>::value);

static_assert(!is_mutable_buffer_sequence<
    net::const_buffer, net::mutable_buffer
>::value);

static_assert(is_mutable_buffer_sequence<
    net::mutable_buffer const&
>::value);

static_assert(is_mutable_buffer_sequence<
    net::mutable_buffer const&, net::mutable_buffer const&
>::value);

// buffers_type

static_assert(
    std::is_same<net::const_buffer, buffers_type<
    net::const_buffer
    >>::value);

static_assert(
    std::is_same<net::const_buffer, buffers_type<
    net::const_buffer, net::const_buffer
    >>::value);

static_assert(
    std::is_same<net::const_buffer, buffers_type<
    net::const_buffer, net::mutable_buffer
    >>::value);

static_assert(
    std::is_same<net::mutable_buffer, buffers_type<
    >>::value);

static_assert(
    std::is_same<net::mutable_buffer, buffers_type<
    net::mutable_buffer
    >>::value);

static_assert(
    std::is_same<net::mutable_buffer, buffers_type<
    net::mutable_buffer, net::mutable_buffer
    >>::value);

static_assert(
    std::is_same<net::const_buffer, buffers_type<
    std::array<net::const_buffer, 3>
    >>::value);

static_assert(
    std::is_same<net::mutable_buffer, buffers_type<
    std::array<net::mutable_buffer, 3>
    >>::value);

static_assert(
    std::is_same<net::const_buffer, buffers_type<
    std::array<int, 3>
    >>::value);

// buffers_iterator_type

static_assert(
    std::is_same<net::const_buffer const*, buffers_iterator_type<
    net::const_buffer
    >>::value);

static_assert(
    std::is_same<net::mutable_buffer const*, buffers_iterator_type<
    net::mutable_buffer
    >>::value);

struct sequence
{
    struct value_type
    {
        operator net::const_buffer() const noexcept
        {
            return { "Hello, world!", 13 };
        }
    };

    using const_iterator = value_type const*;

    const_iterator begin() const noexcept
    {
        return &v_;
    }

    const_iterator end() const noexcept
    {
        return begin() + 1;
    }

private:
    value_type v_;
};

struct not_sequence
{
};

TEST_CASE("testFunction", "buffer_traits") {
	REQUIRE(buffer_bytes(
		net::const_buffer("Hello, world!", 13)) == 13);

    REQUIRE(buffer_bytes(
        net::mutable_buffer{}) == 0);

    {
        sequence s;
        REQUIRE(buffer_bytes(s) == 13);
    }

    {
        std::array<net::const_buffer, 2> s({ {
            net::const_buffer("Hello, world!", 13),
            net::const_buffer("Hello, world!", 13)} });
        REQUIRE(buffer_bytes(s) == 26);
    }

    static_assert(!detail::is_invocable<
        detail::buffer_bytes_impl,
        std::size_t(not_sequence const&)>::value);
}