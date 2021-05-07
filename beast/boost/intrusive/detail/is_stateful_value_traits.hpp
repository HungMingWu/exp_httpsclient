/////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga  2009-2013.
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/intrusive for documentation.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef BOOST_INTRUSIVE_DETAIL_IS_STATEFUL_VALUE_TRAITS_HPP
#define BOOST_INTRUSIVE_DETAIL_IS_STATEFUL_VALUE_TRAITS_HPP

#ifndef BOOST_CONFIG_HPP
#  include <boost/config.hpp>
#endif

#if defined(BOOST_HAS_PRAGMA_ONCE)
#  pragma once
#endif

namespace boost::intrusive::detail {

template <typename T, typename value_type, typename node_ptr>
concept have_to_node_ptr_v = std::is_same_v<
    decltype(std::declval<T>().to_node_ptr(std::declval<value_type&>())), node_ptr>;

template <typename T, typename value_type, typename node_ptr>
concept have_stateless_to_node_ptr_v =
    std::is_same_v<decltype(T::to_node_ptr(std::declval<value_type&>())), node_ptr>;

template <typename T, typename value_type, typename node_ptr>
concept have_stateful_to_node_ptr_v = have_to_node_ptr_v<T, value_type, node_ptr>
    && !have_stateless_to_node_ptr_v<T, value_type, node_ptr>;

template <typename T, typename value_type, typename const_node_ptr>
concept have_const_to_node_ptr_v = std::is_same_v<
    decltype(std::declval<T>().to_node_ptr(std::declval<const value_type&>())), const_node_ptr>;

template <typename T, typename value_type, typename const_node_ptr>
concept have_stateless_const_to_const_node_ptr_v =
std::is_same_v<decltype(T::to_node_ptr(std::declval<const value_type&>())), const_node_ptr>;

template <typename T, typename value_type, typename const_node_ptr>
concept have_stateful_const_to_node_ptr_v = have_const_to_node_ptr_v<T, value_type, const_node_ptr>
    && !have_stateless_const_to_const_node_ptr_v<T, value_type, const_node_ptr>;

template <typename T, typename node_ptr, typename pointer>
concept have_to_value_ptr_v = std::is_same_v<
    decltype(std::declval<T>().to_value_ptr(std::declval<node_ptr>())), pointer>;

template <typename T, typename node_ptr, typename pointer>
concept have_stateless_to_value_ptr_v =
    std::is_same_v<decltype(T::to_value_ptr(std::declval<node_ptr>())), pointer>;

template <typename T, typename node_ptr, typename pointer>
concept have_stateful_to_value_ptr_v = have_to_value_ptr_v<T, node_ptr, pointer>
    && !have_stateless_to_value_ptr_v<T, node_ptr, pointer>;

template <typename T, typename const_node_ptr, typename const_pointer>
concept have_const_to_value_ptr_v = std::is_same_v<
    decltype(std::declval<T>().to_value_ptr(std::declval<const_node_ptr>())), const_pointer>;

template <typename T, typename const_node_ptr, typename const_pointer>
concept have_stateless_const_to_value_ptr_v =
    std::is_same_v<decltype(T::to_value_ptr(std::declval<const_node_ptr>())), const_pointer>;

template <typename T, typename const_node_ptr, typename const_pointer>
concept have_stateful_const_to_value_ptr_v = have_const_to_value_ptr_v<T, const_node_ptr, const_pointer>
    && !have_stateless_const_to_value_ptr_v<T, const_node_ptr, const_pointer>;

template <typename value_traits>
struct is_stateful_value_traits
{
   using node_ptr = typename value_traits::node_ptr;
   using pointer = typename value_traits::pointer;
   using value_type = typename value_traits::value_type;
   using const_node_ptr = typename value_traits::const_node_ptr;
   using const_pointer = typename value_traits::const_pointer;

   static constexpr bool value =
       have_stateful_to_node_ptr_v<value_traits, value_type, node_ptr> ||
       have_stateful_to_value_ptr_v<value_traits, node_ptr, pointer> ||
       have_stateful_const_to_node_ptr_v<value_traits, value_type, const_node_ptr> ||
       have_stateful_const_to_value_ptr_v<value_traits, const_node_ptr, const_pointer>;
};

template <typename value_traits>
concept is_stateful_value_traits_v = is_stateful_value_traits<value_traits>::value;

} // boost::intrusive::detail

#endif   //@ifndef BOOST_INTRUSIVE_DETAIL_IS_STATEFUL_VALUE_TRAITS_HPP
