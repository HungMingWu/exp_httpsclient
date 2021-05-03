#include "catch.hpp"

#include <boost/beast/core/buffers_cat.hpp>
#include <boost/beast/core/buffer_traits.hpp>
#include <boost/beast/core/buffers_prefix.hpp>
#include <boost/beast/core/buffers_suffix.hpp>
#include <asio/buffer.hpp>
#include <iterator>
#include <list>
#include <string_view>
#include <type_traits>
#include <vector>
#include "test_buffer.hpp"

namespace net = asio;

TEST_CASE("DefaultIterators", "buffer_cat") {
    // default ctor is one past the end
    char c[2] = {};
    auto bs = boost::beast::buffers_cat(
        net::const_buffer(&c[0], 1),
        net::const_buffer(&c[1], 1));
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

TEST_CASE("BufferSequence", "buffer_cat") {
    std::string_view s = "Hello, world!";
    net::const_buffer b1(s.data(), 6);
    net::const_buffer b2(
        s.data() + b1.size(), s.size() - b1.size());
    boost::beast::test_buffer_sequence(boost::beast::buffers_cat(b1, b2));
}

template <class F>
void checkException(F&& f)
{
    bool check = false;
    try
    {
        f();
    }
    catch (std::logic_error const&)
    {
        check = true;
    }
    catch (...)
    {
    }
    REQUIRE(check);
}

TEST_CASE("testExceptions", "buffer_cat") {
    net::const_buffer b1{ "He", 2 };
    net::const_buffer b2{ "llo,", 4 };
    net::const_buffer b3{ " world!", 7 };

    auto const b = boost::beast::buffers_cat(b1, b2, b3);
    using type = decltype(b);

    // Dereferencing a default-constructed iterator
    checkException(
        []
        {
            (void)*(type::const_iterator{});
        });

    // Incrementing a default-constructed iterator
    checkException(
        []
        {
            ++(type::const_iterator{});
        });

    // Decrementing a default-constructed iterator
    checkException(
        []
        {
            --(type::const_iterator{});
        });

    // Decrementing an iterator to the beginning
    checkException(
        [&b]
        {
            --b.begin();
        });

    // Dereferencing an iterator to the end
    checkException(
        [&b]
        {
            *b.end();
        });

    // Incrementing an iterator to the end
    checkException(
        [&b]
        {
            ++b.end();
        });
}

TEST_CASE("testEmpty 1", "buffer_cat") {
    net::const_buffer b0{};
    net::const_buffer b1{ "He", 2 };
    net::const_buffer b2{ "llo,", 4 };
    net::const_buffer b3{ " world!", 7 };
    using namespace boost::beast;
    {
        auto const b = buffers_cat(b0, b0);
        REQUIRE(buffer_bytes(b) == 0);
        REQUIRE(buffers_length(b) == 0);
    }
    {
        auto const b = buffers_cat(b0, b0, b0, b0);
        REQUIRE(buffer_bytes(b) == 0);
        REQUIRE(buffers_length(b) == 0);
    }
    {
        auto const b = buffers_cat(b1, b2, b3);
        REQUIRE(buffers_to_string(b) == "Hello, world!");
        REQUIRE(buffers_length(b) == 3);
        test_buffer_sequence(b);
    }
    {
        auto const b = buffers_cat(b0, b1, b2, b3);
        REQUIRE(buffers_to_string(b) == "Hello, world!");
        REQUIRE(buffers_length(b) == 3);
        test_buffer_sequence(b);
    }
    {
        auto const b = buffers_cat(b1, b0, b2, b3);
        REQUIRE(buffers_to_string(b) == "Hello, world!");
        REQUIRE(buffers_length(b) == 3);
        test_buffer_sequence(b);
    }
    {
        auto const b = buffers_cat(b1, b2, b0, b3);
        REQUIRE(buffers_to_string(b) == "Hello, world!");
        REQUIRE(buffers_length(b) == 3);
        test_buffer_sequence(b);
    }
    {
        auto const b = buffers_cat(b1, b2, b3, b0);
        REQUIRE(buffers_to_string(b) == "Hello, world!");
        REQUIRE(buffers_length(b) == 3);
        test_buffer_sequence(b);
    }
}

TEST_CASE("testEmpty 2", "buffer_cat") {
    using namespace boost::beast;
    auto e1 = net::const_buffer{};
    auto b1 = std::array<net::const_buffer, 3>{ {
            e1,
                net::const_buffer{ "He", 2 },
                net::const_buffer{ "l", 1 } }};
    auto b2 = std::array<net::const_buffer, 3>{ {
            net::const_buffer{ "lo", 2 },
                e1,
                net::const_buffer{ ", ", 2 } }};
    auto b3 = std::array<net::const_buffer, 3>{ {
            net::const_buffer{ "w", 1 },
                net::const_buffer{ "orld!", 5 },
                e1 }};
    {
        auto const b = buffers_cat(
            e1, b1, e1, b2, e1, b3, e1);
        REQUIRE(buffers_to_string(b) == "Hello, world!");
        REQUIRE(buffers_length(b) == 6);
    }
}

TEST_CASE("testSingleBuffer", "buffer_cat") {
    using namespace boost::beast;
    struct empty_sequence
    {
        using value_type = net::const_buffer;
        using const_iterator = value_type const*;

        const_iterator
            begin() const noexcept
        {
            return &v_;
        }

        const_iterator
            end() const noexcept
        {
            return begin();
        }

    private:
        value_type v_;
    };
    auto e1 = net::const_buffer{};
    auto e2 = empty_sequence{};
    auto b1 = std::array<net::const_buffer, 3>{ {
            e1,
                net::const_buffer{ "He", 2 },
                net::const_buffer{ "l", 1 } }};
    auto b2 = std::array<net::const_buffer, 3>{ {
            net::const_buffer{ "lo", 2 },
                e1,
                net::const_buffer{ ", ", 2 } }};
    auto b3 = std::array<net::const_buffer, 3>{ {
            net::const_buffer{ "w", 1 },
                net::const_buffer{ "orld!", 5 },
                e1 }};
    {
        auto const b = buffers_cat(
            e2, b1, e2, b2, e2, b3, e2);
        REQUIRE(buffers_to_string(b) == "Hello, world!");
        REQUIRE(buffers_length(b) == 6);
    }
}

TEST_CASE("testEmpty 3", "buffer_cat") {
    using namespace boost::beast;
    char c[1] = {};
    auto b = net::const_buffer(c, 1);
    auto bs = buffers_cat(net::const_buffer(c, 1));
    auto first = net::buffer_sequence_begin(bs);
    auto last = net::buffer_sequence_end(bs);
    BOOST_ASSERT(first != last);
    REQUIRE(std::distance(first, last) == 1);
    net::const_buffer b2(*first);
    REQUIRE(b.data() == b2.data());
    REQUIRE(b.size() == b2.size());
}