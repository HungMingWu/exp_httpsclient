//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/beast
//

#ifndef BOOST_BEAST_IMPL_BUFFERS_CAT_HPP
#define BOOST_BEAST_IMPL_BUFFERS_CAT_HPP

#include <boost/asio/buffer.hpp>
#include <boost/mp11/detail/mp_with_index.hpp>
#include <boost/assert.hpp>
#include <cstdint>
#include <iterator>
#include <new>
#include <stdexcept>
#include <tuple>
#include <utility>
#include <variant>

namespace boost {
namespace beast {

template <typename ...Args, std::size_t ...Idx, typename Func>
void var_visit(std::variant<Args...>& var, Func&& func, std::index_sequence<Idx...>)
{
    ((var.index() == Idx && (func(std::integral_constant<std::size_t, Idx>{}, std::get<Idx>(var)), true)) || ...);
}

template <typename ...Args, typename Func>
void var_visit(std::variant<Args...>& var, Func&& func)
{
    var_visit(var, std::forward<Func>(func), std::make_index_sequence<sizeof...(Args)>());
}

template<class Buffer>
class buffers_cat_view<Buffer>
{
    Buffer buffer_;
public:
    using value_type = buffers_type<Buffer>;

    using const_iterator = buffers_iterator_type<Buffer>;

    explicit
    buffers_cat_view(Buffer const& buffer)
        : buffer_(buffer)
    {
    }

    const_iterator
    begin() const
    {
        return net::buffer_sequence_begin(buffer_);
    }

    const_iterator
    end() const
    {
        return net::buffer_sequence_end(buffer_);
    }
};

#if defined(_MSC_VER) && ! defined(__clang__)
# define BOOST_BEAST_UNREACHABLE() __assume(false)
# define BOOST_BEAST_UNREACHABLE_RETURN(v) return v
#else
# define BOOST_BEAST_UNREACHABLE() __builtin_unreachable()
# define BOOST_BEAST_UNREACHABLE_RETURN(v) \
    do { __builtin_unreachable(); return v; } while(false)
#endif

#ifdef BOOST_BEAST_TESTS

#define BOOST_BEAST_LOGIC_ERROR(s) \
    do { \
        BOOST_THROW_EXCEPTION(std::logic_error((s))); \
        BOOST_BEAST_UNREACHABLE(); \
    } while(false)

#define BOOST_BEAST_LOGIC_ERROR_RETURN(v, s) \
    do { \
        BOOST_THROW_EXCEPTION(std::logic_error(s)); \
        BOOST_BEAST_UNREACHABLE_RETURN(v); \
    } while(false)

#else

#define BOOST_BEAST_LOGIC_ERROR(s) \
    do { \
        BOOST_ASSERT_MSG(false, s); \
        BOOST_BEAST_UNREACHABLE(); \
    } while(false)

#define BOOST_BEAST_LOGIC_ERROR_RETURN(v, s) \
    do { \
        BOOST_ASSERT_MSG(false, (s)); \
        BOOST_BEAST_UNREACHABLE_RETURN(v); \
    } while(false)

#endif

namespace detail {

struct buffers_cat_view_iterator_base
{
    struct past_end
    {
        net::mutable_buffer
        operator*() const
        {
            BOOST_BEAST_LOGIC_ERROR_RETURN({},
                "Dereferencing a one-past-the-end iterator");
        }

        operator bool() const noexcept
        {
            return true;
        }
    };
};

} // detail

template<class... Bn>
class buffers_cat_view<Bn...>::const_iterator
    : private detail::buffers_cat_view_iterator_base
{
    // VFALCO The logic to skip empty sequences fails
    //        if there is just one buffer in the list.
    static_assert(sizeof...(Bn) >= 2,
        "A minimum of two sequences are required");

    detail::tuple<Bn...> const* bn_ = nullptr;
    std::variant<
        buffers_iterator_type<Bn>..., past_end, std::monostate> it_ = std::monostate{};

    friend class buffers_cat_view<Bn...>;

    template<std::size_t I>
    using C = std::integral_constant<std::size_t, I>;

public:
    using value_type = typename
        buffers_cat_view<Bn...>::value_type;
    using pointer = value_type const*;
    using reference = value_type;
    using difference_type = std::ptrdiff_t;
    using iterator_category =
        std::bidirectional_iterator_tag;

    const_iterator() = default;
    const_iterator(const_iterator const& other) = default;
    const_iterator& operator=(
        const_iterator const& other) = default;

    bool
    operator==(const_iterator const& other) const;

    bool
    operator!=(const_iterator const& other) const
    {
        return ! (*this == other);
    }

    reference
    operator*() const;

    pointer
    operator->() const = delete;

    const_iterator&
    operator++();

    const_iterator
    operator++(int);

    const_iterator&
    operator--();

    const_iterator
    operator--(int);

private:
    const_iterator(
        detail::tuple<Bn...> const& bn,
        std::true_type);

    const_iterator(
        detail::tuple<Bn...> const& bn,
        std::false_type);

    struct increment
    {
        const_iterator& self;

        template <std::size_t I, typename T>
        void
        operator()(std::integral_constant<std::size_t, I>, T& arg)
        {
            if constexpr (std::is_same_v<T, std::monostate>) {
                BOOST_BEAST_LOGIC_ERROR(
                    "Incrementing a default-constructed iterator");
            }
            else if constexpr (std::is_same_v<T, past_end>) {
                BOOST_BEAST_LOGIC_ERROR(
                    "Incrementing a one-past-the-end iterator");
            }
            else {
                ++arg;
                next<I>();
            }
        }

        template <std::size_t I>
        void next()
        {
            auto& it = std::get<I>(self.it_);
            for (;;)
            {
                if (it == net::buffer_sequence_end(
                    self.bn_->get<I>()))
                    break;
                if (net::const_buffer(*it).size() > 0)
                    return;
                ++it;
            }

            if constexpr (I < sizeof...(Bn) - 1) {
                self.it_.template emplace<I + 1>(
                    net::buffer_sequence_begin(
                        self.bn_->get<I + 1>()));
                next<I + 1>();
            }
            else {
                self.it_.template emplace<past_end>();
            }
        }
    };

    struct decrement
    {
        const_iterator& self;

        template <std::size_t I, typename T>
        void
        operator()(std::integral_constant<std::size_t, I>, T& arg)
        {
            if constexpr (std::is_same_v<T, std::monostate>) {
                BOOST_BEAST_LOGIC_ERROR(
                    "Decrementing a default-constructed iterator");
            }
            else if constexpr (std::is_same_v<T, past_end>) {
                self.it_.template emplace<I - 1>(
                    net::buffer_sequence_end(
                        self.bn_->get<I - 1>()));
                prev<I - 1>();
            }
            else {
                prev<I>();
            }
        }

        template <std::size_t I>
        void prev()
        {
            auto& it = std::get<I>(self.it_);
            for (;;)
            {
                if (it == net::buffer_sequence_begin(
                    self.bn_->get<I>()))
                    break;
                --it;
                if (net::const_buffer(*it).size() > 0)
                    return;
            }

            if constexpr (I > 0) {
                self.it_.template emplace<I - 1>(
                    net::buffer_sequence_end(
                        self.bn_->get<I - 1>()));
                prev<I - 1>();
            } else if constexpr (I == 0) {
                BOOST_BEAST_LOGIC_ERROR(
                    "Decrementing an iterator to the beginning");
            }
        }
    };
};

//------------------------------------------------------------------------------

template<class... Bn>
buffers_cat_view<Bn...>::
const_iterator::
const_iterator(
    detail::tuple<Bn...> const& bn,
    std::true_type)
    : bn_(&bn)
{
    // one past the end
    it_.template emplace<past_end>();
}

template<class... Bn>
buffers_cat_view<Bn...>::
const_iterator::
const_iterator(
    detail::tuple<Bn...> const& bn,
    std::false_type)
    : bn_(&bn)
{
    it_.template emplace<0>(
        net::buffer_sequence_begin(
            bn_->get<0>()));
    increment{*this}.template next<0>();
}

template<class... Bn>
bool
buffers_cat_view<Bn...>::
const_iterator::
operator==(const_iterator const& other) const
{
    return bn_ == other.bn_ && it_ == other.it_;
}

template<class... Bn>
auto
buffers_cat_view<Bn...>::
const_iterator::
operator*() const ->
    reference
{
    return std::visit([](auto&& arg) -> reference {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::monostate>) {
            BOOST_BEAST_LOGIC_ERROR_RETURN({},
                "Dereferencing a default-constructed iterator");
        }
        else {
            return *arg;
        }
    }, it_);
}

template<class... Bn>
auto
buffers_cat_view<Bn...>::
const_iterator::
operator++() ->
    const_iterator&
{
    var_visit(it_, increment{ *this });
    return *this;
}

template<class... Bn>
auto
buffers_cat_view<Bn...>::
const_iterator::
operator++(int) ->
    const_iterator
{
    auto temp = *this;
    ++(*this);
    return temp;
}

template<class... Bn>
auto
buffers_cat_view<Bn...>::
const_iterator::
operator--() ->
    const_iterator&
{
    var_visit(it_, decrement{ *this });
    return *this;
}

template<class... Bn>
auto
buffers_cat_view<Bn...>::
const_iterator::
operator--(int) ->
    const_iterator
{
    auto temp = *this;
    --(*this);
    return temp;
}

//------------------------------------------------------------------------------

template<class... Bn>
buffers_cat_view<Bn...>::
buffers_cat_view(Bn const&... bn)
    : bn_(bn...)
{
}


template<class... Bn>
auto
buffers_cat_view<Bn...>::begin() const ->
    const_iterator
{
    return const_iterator{bn_, std::false_type{}};
}

template<class... Bn>
auto
buffers_cat_view<Bn...>::end() const->
    const_iterator
{
    return const_iterator{bn_, std::true_type{}};
}

} // beast
} // boost

#endif
