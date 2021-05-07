//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2014-2014. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/intrusive for documentation.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef BOOST_INTRUSIVE_POINTER_REBIND_HPP
#define BOOST_INTRUSIVE_POINTER_REBIND_HPP

#ifndef BOOST_CONFIG_HPP
#  include <boost/config.hpp>
#endif

#if defined(BOOST_HAS_PRAGMA_ONCE)
#  pragma once
#endif

namespace boost {
namespace intrusive {

///////////////////////////
//struct pointer_rebind_mode
///////////////////////////
template <typename Ptr, typename U>
struct pointer_has_rebind
{
   template <typename V> struct any
   {  any(const V&) { } };

   template <typename X>
   static char test(int, typename X::template rebind<U>*);

   template <typename X>
   static int test(any<int>, void*);

   static const bool value = (1 == sizeof(test<Ptr>(0, 0)));
};

template <typename Ptr, typename U>
struct pointer_has_rebind_other
{
   template <typename V> struct any
   {  any(const V&) { } };

   template <typename X>
   static char test(int, typename X::template rebind<U>::other*);

   template <typename X>
   static int test(any<int>, void*);

   static const bool value = (1 == sizeof(test<Ptr>(0, 0)));
};

template <typename Ptr, typename U>
struct pointer_rebind_mode
{
   static const unsigned int rebind =       (unsigned int)pointer_has_rebind<Ptr, U>::value;
   static const unsigned int rebind_other = (unsigned int)pointer_has_rebind_other<Ptr, U>::value;
   static const unsigned int mode =         rebind + rebind*rebind_other;
};

////////////////////////
//struct pointer_rebinder
////////////////////////
template <typename Ptr, typename U, unsigned int RebindMode>
struct pointer_rebinder;

// Implementation of pointer_rebinder<U>::type if Ptr has
// its own rebind<U>::other type (C++03)
template <typename Ptr, typename U>
struct pointer_rebinder< Ptr, U, 2u >
{
   typedef typename Ptr::template rebind<U>::other type;
};

// Implementation of pointer_rebinder<U>::type if Ptr has
// its own rebind template.
template <typename Ptr, typename U>
struct pointer_rebinder< Ptr, U, 1u >
{
   typedef typename Ptr::template rebind<U> type;
};

template <template <class, class...> class Ptr, typename A, class... An, class U>
struct pointer_rebinder<Ptr<A, An...>, U, 0u >
{
   typedef Ptr<U, An...> type;
};

//Needed for non-conforming compilers like GCC 4.3
template <template <class> class Ptr, typename A, class U>
struct pointer_rebinder<Ptr<A>, U, 0u >
{
   typedef Ptr<U> type;
};

template <typename Ptr, typename U>
struct pointer_rebind
   : public pointer_rebinder<Ptr, U, pointer_rebind_mode<Ptr, U>::mode>
{};

template <typename T, typename U>
struct pointer_rebind<T*, U>
{  typedef U* type; };

template <typename T, typename U>
using pointer_rebind_t = typename pointer_rebind<T, U>::type;

}  //namespace container {
}  //namespace boost {

#endif // defined(BOOST_INTRUSIVE_POINTER_REBIND_HPP)
