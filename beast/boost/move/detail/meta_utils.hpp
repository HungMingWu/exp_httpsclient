//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2012-2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/move for documentation.
//
//////////////////////////////////////////////////////////////////////////////

//! \file

#ifndef BOOST_MOVE_DETAIL_META_UTILS_HPP
#define BOOST_MOVE_DETAIL_META_UTILS_HPP

#if defined(BOOST_HAS_PRAGMA_ONCE)
#  pragma once
#endif
#include <boost/move/detail/config_begin.hpp>
#include <boost/move/detail/meta_utils_core.hpp>
#include <cstddef>   //for std::size_t
#include <type_traits>

//Small meta-typetraits to support move

namespace boost {

namespace move_detail {

//////////////////////////////////////
//             apply
//////////////////////////////////////
template<class F, class Param>
struct apply
{
   typedef typename F::template apply<Param>::type type;
};

//////////////////////////////////////
//              nat
//////////////////////////////////////
struct nat{};
struct nat2{};
struct nat3{};

//////////////////////////////////////
//          yes_type/no_type
//////////////////////////////////////
typedef char yes_type;

struct no_type
{
   char _[2];
};

//////////////////////////////////////
//            natify
//////////////////////////////////////
template <class T> struct natify{};

//////////////////////////////////////
//      add_lvalue_reference
//////////////////////////////////////
template<class T>
struct add_lvalue_reference
{  typedef T& type;  };

template<class T> struct add_lvalue_reference<T&>                 {  typedef T& type;  };
template<>        struct add_lvalue_reference<void>               {  typedef void type;   };
template<>        struct add_lvalue_reference<const void>         {  typedef const void type;  };
template<>        struct add_lvalue_reference<volatile void>      {  typedef volatile void type;   };
template<>        struct add_lvalue_reference<const volatile void>{  typedef const volatile void type;   };

template<class T>
struct add_const_lvalue_reference
{
   using t_unreferenced = std::remove_reference_t<T>;
   using t_unreferenced_const = std::add_const_t<t_unreferenced>;
   using type = typename add_lvalue_reference<t_unreferenced_const>::type;
};

template <typename T, typename U>
concept is_same_or_convertible = std::is_same_v<T, U> || std::is_convertible_v<T, U>;

#if defined(BOOST_GCC) && (BOOST_GCC <= 40000)
#define BOOST_MOVE_HELPERS_RETURN_SFINAE_BROKEN
#endif

}  //namespace move_detail {
}  //namespace boost {

#include <boost/move/detail/config_end.hpp>

#endif //#ifndef BOOST_MOVE_DETAIL_META_UTILS_HPP
