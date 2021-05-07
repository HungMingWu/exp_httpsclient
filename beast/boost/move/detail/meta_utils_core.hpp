//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2015-2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/move for documentation.
//
//////////////////////////////////////////////////////////////////////////////

//! \file

#ifndef BOOST_MOVE_DETAIL_META_UTILS_CORE_HPP
#define BOOST_MOVE_DETAIL_META_UTILS_CORE_HPP

#ifndef BOOST_CONFIG_HPP
#  include <boost/config.hpp>
#endif
#
#if defined(BOOST_HAS_PRAGMA_ONCE)
#  pragma once
#endif

//Small meta-typetraits to support move

namespace boost {
namespace move_detail {

//////////////////////////////////////
//             if_c
//////////////////////////////////////
template<bool C, typename T1, typename T2>
struct if_c
{
   typedef T1 type;
};

template<typename T1, typename T2>
struct if_c<false,T1,T2>
{
   typedef T2 type;
};

//////////////////////////////////////
//             if_
//////////////////////////////////////
template<typename T1, typename T2, typename T3>
struct if_ : if_c<0 != T1::value, T2, T3>
{};

//////////////////////////////////////
//          enable_if_c
//////////////////////////////////////
struct enable_if_nat{};

template <bool B, class T = enable_if_nat>
struct enable_if_c
{
   typedef T type;
};

template <class T>
struct enable_if_c<false, T> {};

//////////////////////////////////////
//          disable_if_c
//////////////////////////////////////
template <bool B, class T = enable_if_nat>
struct disable_if_c
   : enable_if_c<!B, T>
{};

}  //namespace move_detail {
}  //namespace boost {

#endif //#ifndef BOOST_MOVE_DETAIL_META_UTILS_CORE_HPP
