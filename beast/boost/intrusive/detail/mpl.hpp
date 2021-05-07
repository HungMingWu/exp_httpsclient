/////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga        2006-2014
// (C) Copyright Microsoft Corporation  2014
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/intrusive for documentation.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef BOOST_INTRUSIVE_DETAIL_MPL_HPP
#define BOOST_INTRUSIVE_DETAIL_MPL_HPP

#ifndef BOOST_CONFIG_HPP
#  include <boost/config.hpp>
#endif

#if defined(BOOST_HAS_PRAGMA_ONCE)
#  pragma once
#endif

#include <boost/move/detail/type_traits.hpp>
#include <cstddef>

namespace boost {
namespace intrusive {
namespace detail {

        template <class Default, class AlwaysVoid,
            template<class...> class Op, class... Args>
        struct detector
        {
            using value_t = std::false_type;
            using type = Default;
        };

        template <class Default, template<class...> class Op, class... Args>
        struct detector<Default, std::void_t<Op<Args...>>, Op, Args...>
        {
            using value_t = std::true_type;
            using type = Op<Args...>;
        };



    // special type to indicate detection failure
    struct nonesuch {
        nonesuch() = delete;
        ~nonesuch() = delete;
        nonesuch(nonesuch const&) = delete;
        void operator=(nonesuch const&) = delete;
    };

    template <template<class...> class Op, class... Args>
    using is_detected =
        typename detail::detector<nonesuch, void, Op, Args...>::value_t;

    template <template<class...> class Op, class... Args>
    using detected_t = typename detail::detector<nonesuch, void, Op, Args...>::type;

    template <class Default, template<class...> class Op, class... Args>
    using detected_or = detail::detector<Default, void, Op, Args...>;

    template <template <typename> class type_action, typename T>
    constexpr bool check_action = is_detected<type_action, T>::value;

    template <typename T, template <typename> class type_action, typename defaultType, bool = check_action<type_action, T>>
    struct default_action {
        using type = type_action<T>;
    };

    template <typename T, template <typename> class type_action, typename defaultType>
    struct default_action<T, type_action, defaultType, false> {
        using type = defaultType;
    };

    template <typename T, template <typename> class type_action, typename defaultType>
    using default_action_t = typename default_action<T, type_action, defaultType>::type;

    template <typename T>
    struct identity {
        using type = T;
        typedef typename boost::move_detail::add_const_lvalue_reference<T>::type reference;
        reference operator()(reference t)
        {
            return t;
        }
    };

using boost::move_detail::disable_if_c;
using boost::move_detail::apply;
using boost::move_detail::unvoid_ref;

constexpr std::size_t ls_zeros(std::size_t value)
{
    return (value < 2) ? 0 : ((value & 1) ? 0 : (1 + ls_zeros(value / 2)));
}

} //namespace detail
} //namespace intrusive
} //namespace boost

#endif //BOOST_INTRUSIVE_DETAIL_MPL_HPP
