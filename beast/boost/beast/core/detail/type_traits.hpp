//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/beast
//

#ifndef BOOST_BEAST_DETAIL_TYPE_TRAITS_HPP
#define BOOST_BEAST_DETAIL_TYPE_TRAITS_HPP

#include <type_traits>
#include <new>

namespace boost {
namespace beast {
namespace detail {

// for span
template<class T, class E, class = void>
struct is_contiguous_container: std::false_type {};

template<class T, class E>
struct is_contiguous_container<T, E, std::void_t<
    decltype(
        std::declval<std::size_t&>() = std::declval<T const&>().size(),
        std::declval<E*&>() = std::declval<T&>().data()),
    typename std::enable_if<
        std::is_same<
            typename std::remove_cv<E>::type,
            typename std::remove_cv<
                typename std::remove_pointer<
                    decltype(std::declval<T&>().data())
                >::type
            >::type
        >::value
    >::type>>: std::true_type
{};

template <class T, class U>
T launder_cast(U* u)
{
#if defined(__cpp_lib_launder) && __cpp_lib_launder >= 201606
    return std::launder(reinterpret_cast<T>(u));
#elif defined(BOOST_GCC) && BOOST_GCC_VERSION > 80000
    return __builtin_launder(reinterpret_cast<T>(u));
#else
    return reinterpret_cast<T>(u);
#endif
}

} // detail
} // beast
} // boost

#endif
