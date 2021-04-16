#pragma once

#include <utility>
#include <iostream>
#include <type_traits>

namespace boost {
namespace beast {
namespace detail {
    template <std::size_t Index, typename T>
    class TupleElement
    {
        T value;
    protected:
        TupleElement(const T& val) : value(val) {}

        template <std::size_t I>
        constexpr std::enable_if_t<I == Index, T&>
            get() { return value; }

        template <std::size_t I>
        constexpr std::enable_if_t<I == Index, const T&>
            get() const { return value; }
    };

    template <typename Ind, typename... Ts>
    class TupleImpl;

    template <std::size_t... Is, typename... Ts>
    class TupleImpl<std::index_sequence<Is...>, Ts...>
        : TupleElement<Is, Ts>...
    {
    public:
        constexpr TupleImpl(const Ts&... args)
            : TupleElement<Is, Ts>(args)... {}

        using TupleElement<Is, Ts>::get...;
    };

    template <typename... Ts>
    struct tuple : detail::TupleImpl<std::index_sequence_for<Ts...>, Ts...>
    {
        using detail::TupleImpl<std::index_sequence_for<Ts...>, Ts...>::TupleImpl;
    };

    template <typename... Ts>
    tuple(const Ts&...) -> tuple<Ts...>;
} // detail
} // beast
} // boost