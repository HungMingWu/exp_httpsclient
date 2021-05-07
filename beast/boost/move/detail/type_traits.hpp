//////////////////////////////////////////////////////////////////////////////
// (C) Copyright John Maddock 2000.
// (C) Copyright Ion Gaztanaga 2005-2015.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/move for documentation.
//
// The alignment and Type traits implementation comes from
// John Maddock's TypeTraits library.
//
// Some other tricks come from Howard Hinnant's papers and StackOverflow replies
//////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_MOVE_DETAIL_TYPE_TRAITS_HPP
#define BOOST_MOVE_DETAIL_TYPE_TRAITS_HPP

#ifndef BOOST_CONFIG_HPP
#  include <boost/config.hpp>
#endif
#
#if defined(BOOST_HAS_PRAGMA_ONCE)
#  pragma once
#endif

#include <boost/move/detail/config_begin.hpp>

// move/detail
#include <boost/move/detail/meta_utils.hpp>
// other
#include <boost/assert.hpp>
#include <boost/static_assert.hpp>
// std
#include <cstddef>

//Use of Boost.TypeTraits leads to long preprocessed source code due to
//MPL dependencies. We'll use intrinsics directly and make or own
//simplified version of TypeTraits.
//If someday Boost.TypeTraits dependencies are minimized, we should
//revisit this file redirecting code to Boost.TypeTraits traits.

//These traits don't care about volatile, reference or other checks
//made by Boost.TypeTraits because no volatile or reference types
//can be hold in Boost.Containers. This helps to avoid any Boost.TypeTraits
//dependency.

// Helper macros for builtin compiler support.
// If your compiler has builtin support for any of the following
// traits concepts, then redefine the appropriate macros to pick
// up on the compiler support:
//
// (these should largely ignore cv-qualifiers)
// BOOST_MOVE_IS_POD(T) should evaluate to true if T is a POD type
// BOOST_MOVE_HAS_TRIVIAL_CONSTRUCTOR(T) should evaluate to true if "T x;" has no effect
// BOOST_MOVE_HAS_TRIVIAL_COPY(T) should evaluate to true if T(t) <==> memcpy
// (Note: this trait does not guarantee T is copy constructible, the copy constructor could be deleted but still be trivial)
// BOOST_MOVE_HAS_TRIVIAL_MOVE_CONSTRUCTOR(T) should evaluate to true if T(boost::move(t)) <==> memcpy
// BOOST_MOVE_HAS_TRIVIAL_ASSIGN(T) should evaluate to true if t = u <==> memcpy
// (Note: this trait does not guarantee T is assignable , the copy assignmen could be deleted but still be trivial)
// BOOST_MOVE_HAS_TRIVIAL_MOVE_ASSIGN(T) should evaluate to true if t = boost::move(u) <==> memcpy
// BOOST_MOVE_HAS_TRIVIAL_DESTRUCTOR(T) should evaluate to true if ~T() has no effect
// BOOST_MOVE_HAS_NOTHROW_CONSTRUCTOR(T) should evaluate to true if "T x;" can not throw
// BOOST_MOVE_HAS_NOTHROW_COPY(T) should evaluate to true if T(t) can not throw
// BOOST_MOVE_HAS_NOTHROW_ASSIGN(T) should evaluate to true if t = u can not throw
// BOOST_MOVE_HAS_NOTHROW_MOVE_CONSTRUCT(T) should evaluate to true if T has a non-throwing move constructor.
// BOOST_MOVE_HAS_NOTHROW_MOVE_ASSIGN(T) should evaluate to true if T has a non-throwing move assignment operator.
//
// The following can also be defined: when detected our implementation is greatly simplified.
//
// BOOST_ALIGNMENT_OF(T) should evaluate to the alignment requirements of type T.

#if defined(__MSL_CPP__) && (__MSL_CPP__ >= 0x8000)
    // Metrowerks compiler is acquiring intrinsic type traits support
    // post version 8.  We hook into the published interface to pick up
    // user defined specializations as well as compiler intrinsics as
    // and when they become available:
#   include <msl_utility>
#   define BOOST_MOVE_IS_UNION(T) BOOST_STD_EXTENSION_NAMESPACE::is_union<T>::value
#   define BOOST_MOVE_IS_POD(T) BOOST_STD_EXTENSION_NAMESPACE::is_POD<T>::value
#   define BOOST_MOVE_HAS_TRIVIAL_CONSTRUCTOR(T) BOOST_STD_EXTENSION_NAMESPACE::has_trivial_default_ctor<T>::value
#   define BOOST_MOVE_HAS_TRIVIAL_COPY(T) BOOST_STD_EXTENSION_NAMESPACE::has_trivial_copy_ctor<T>::value
#   define BOOST_MOVE_HAS_TRIVIAL_ASSIGN(T) BOOST_STD_EXTENSION_NAMESPACE::has_trivial_assignment<T>::value
#   define BOOST_MOVE_HAS_TRIVIAL_DESTRUCTOR(T) BOOST_STD_EXTENSION_NAMESPACE::has_trivial_dtor<T>::value
#endif

#if (defined(BOOST_MSVC) && defined(BOOST_MSVC_FULL_VER) && (BOOST_MSVC_FULL_VER >=140050215))\
         || (defined(BOOST_INTEL) && defined(_MSC_VER) && (_MSC_VER >= 1500))
#   define BOOST_MOVE_IS_UNION(T) __is_union(T)
#   define BOOST_MOVE_IS_POD(T)                    (__is_pod(T) && __has_trivial_constructor(T))
#   define BOOST_MOVE_IS_EMPTY(T)                  __is_empty(T)
#   define BOOST_MOVE_HAS_TRIVIAL_CONSTRUCTOR(T)   __has_trivial_constructor(T)
#   define BOOST_MOVE_HAS_TRIVIAL_COPY(T)          (__has_trivial_copy(T)|| ::boost::move_detail::is_pod<T>::value)
#   define BOOST_MOVE_HAS_TRIVIAL_ASSIGN(T)        (__has_trivial_assign(T) || ::boost::move_detail::is_pod<T>::value)
#   define BOOST_MOVE_HAS_TRIVIAL_DESTRUCTOR(T)    (__has_trivial_destructor(T) || ::boost::move_detail::is_pod<T>::value)
#   define BOOST_MOVE_HAS_NOTHROW_CONSTRUCTOR(T)   (__has_nothrow_constructor(T) || ::boost::move_detail::is_trivially_default_constructible<T>::value)
#   define BOOST_MOVE_HAS_NOTHROW_COPY(T)          (__has_nothrow_copy(T) || ::boost::move_detail::is_trivially_copy_constructible<T>::value)
#   define BOOST_MOVE_HAS_NOTHROW_ASSIGN(T)        (__has_nothrow_assign(T) || ::boost::move_detail::is_trivially_copy_assignable<T>::value)

#   if defined(_MSC_VER) && (_MSC_VER >= 1700)
#       define BOOST_MOVE_HAS_TRIVIAL_MOVE_CONSTRUCTOR(T)   (__has_trivial_move_constructor(T) || ::boost::move_detail::is_pod<T>::value)
#       define BOOST_MOVE_HAS_TRIVIAL_MOVE_ASSIGN(T)        (__has_trivial_move_assign(T) || ::boost::move_detail::is_pod<T>::value)
#   endif
#  if _MSC_FULL_VER >= 180020827
#     define BOOST_MOVE_HAS_NOTHROW_MOVE_ASSIGN(T) (__is_nothrow_assignable(T&, T&&))
#     define BOOST_MOVE_HAS_NOTHROW_MOVE_CONSTRUCT(T) (__is_nothrow_constructible(T, T&&))
#  endif
#endif

#if defined(BOOST_CLANG)
//    BOOST_MOVE_HAS_TRAIT
#   ifdef __has_extension
#     define BOOST_MOVE_HAS_TRAIT(T) __has_extension(T)
#   else
#     define BOOST_MOVE_HAS_TRAIT(T) 0
#   endif

//    BOOST_MOVE_IS_UNION
#   if BOOST_MOVE_HAS_TRAIT(is_union)
#     define BOOST_MOVE_IS_UNION(T) __is_union(T)
#   endif

//    BOOST_MOVE_IS_POD
#   if (!defined(__GLIBCXX__) || (__GLIBCXX__ >= 20080306 && __GLIBCXX__ != 20080519)) && BOOST_MOVE_HAS_TRAIT(is_pod)
#     define BOOST_MOVE_IS_POD(T) __is_pod(T)
#   endif

//    BOOST_MOVE_IS_EMPTY
#   if (!defined(__GLIBCXX__) || (__GLIBCXX__ >= 20080306 && __GLIBCXX__ != 20080519)) && BOOST_MOVE_HAS_TRAIT(is_empty)
#     define BOOST_MOVE_IS_EMPTY(T) __is_empty(T)
#   endif

//    BOOST_MOVE_HAS_TRIVIAL_CONSTRUCTOR
#   if BOOST_MOVE_HAS_TRAIT(is_constructible) && BOOST_MOVE_HAS_TRAIT(is_trivially_constructible)
#     define BOOST_MOVE_HAS_TRIVIAL_CONSTRUCTOR(T) __is_trivially_constructible(T)
#   elif BOOST_MOVE_HAS_TRAIT(has_trivial_constructor)
#     define BOOST_MOVE_HAS_TRIVIAL_CONSTRUCTOR(T) __has_trivial_constructor(T)
#   endif

//    BOOST_MOVE_HAS_TRIVIAL_COPY
#   if BOOST_MOVE_HAS_TRAIT(is_constructible) && BOOST_MOVE_HAS_TRAIT(is_trivially_constructible)
#     define BOOST_MOVE_HAS_TRIVIAL_COPY(T) (__is_constructible(T, const T &) && __is_trivially_constructible(T, const T &))
#   elif BOOST_MOVE_HAS_TRAIT(has_trivial_copy)
#     define BOOST_MOVE_HAS_TRIVIAL_COPY(T) __has_trivial_copy(T)
#   endif

//    BOOST_MOVE_HAS_TRIVIAL_ASSIGN
#   if BOOST_MOVE_HAS_TRAIT(is_assignable) && BOOST_MOVE_HAS_TRAIT(is_trivially_assignable)
#     define BOOST_MOVE_HAS_TRIVIAL_ASSIGN(T) (__is_assignable(T, const T &) && __is_trivially_assignable(T, const T &))
#   elif BOOST_MOVE_HAS_TRAIT(has_trivial_copy)
#     define BOOST_MOVE_HAS_TRIVIAL_ASSIGN(T) __has_trivial_assign(T)
#   endif

//    BOOST_MOVE_HAS_TRIVIAL_DESTRUCTOR
#   if BOOST_MOVE_HAS_TRAIT(is_trivially_destructible)
#     define BOOST_MOVE_HAS_TRIVIAL_DESTRUCTOR(T) __is_trivially_destructible(T)
#   elif BOOST_MOVE_HAS_TRAIT(has_trivial_destructor)
#     define BOOST_MOVE_HAS_TRIVIAL_DESTRUCTOR(T) __has_trivial_destructor(T)
#   endif

//    BOOST_MOVE_HAS_NOTHROW_CONSTRUCTOR
#   if BOOST_MOVE_HAS_TRAIT(is_nothrow_constructible)
#     define BOOST_MOVE_HAS_NOTHROW_CONSTRUCTOR(T) __is_nothrow_constructible(T)
#   elif BOOST_MOVE_HAS_TRAIT(has_nothrow_constructor)
#     define BOOST_MOVE_HAS_NOTHROW_CONSTRUCTOR(T) __has_nothrow_constructor(T)
#   endif

//    BOOST_MOVE_HAS_NOTHROW_COPY
#   if BOOST_MOVE_HAS_TRAIT(is_constructible) && BOOST_MOVE_HAS_TRAIT(is_nothrow_constructible)
#     define BOOST_MOVE_HAS_NOTHROW_COPY(T) (__is_constructible(T, const T &) && __is_nothrow_constructible(T, const T &))
#   elif BOOST_MOVE_HAS_TRAIT(has_nothrow_copy)
#     define BOOST_MOVE_HAS_NOTHROW_COPY(T) (__has_nothrow_copy(T))
#   endif

//    BOOST_MOVE_HAS_NOTHROW_ASSIGN
#   if BOOST_MOVE_HAS_TRAIT(is_assignable) && BOOST_MOVE_HAS_TRAIT(is_nothrow_assignable)
#     define BOOST_MOVE_HAS_NOTHROW_ASSIGN(T) (__is_assignable(T, const T &) && __is_nothrow_assignable(T, const T &))
#   elif BOOST_MOVE_HAS_TRAIT(has_nothrow_assign)
#     define BOOST_MOVE_HAS_NOTHROW_ASSIGN(T) (__has_nothrow_assign(T))
#   endif

//    BOOST_MOVE_HAS_TRIVIAL_MOVE_CONSTRUCTOR
#   if !defined(BOOST_NO_CXX11_RVALUE_REFERENCES) && BOOST_MOVE_HAS_TRAIT(is_constructible) && BOOST_MOVE_HAS_TRAIT(is_trivially_constructible)
#     define BOOST_MOVE_HAS_TRIVIAL_MOVE_CONSTRUCTOR(T) (__is_constructible(T, T&&) && __is_trivially_constructible(T, T&&))
#   elif BOOST_MOVE_HAS_TRAIT(has_trivial_move_constructor)
#     define BOOST_MOVE_HAS_TRIVIAL_MOVE_CONSTRUCTOR(T) __has_trivial_move_constructor(T)
#   endif

//    BOOST_MOVE_HAS_TRIVIAL_MOVE_ASSIGN
#   if BOOST_MOVE_HAS_TRAIT(is_assignable) && BOOST_MOVE_HAS_TRAIT(is_trivially_assignable)
#     define BOOST_MOVE_HAS_TRIVIAL_MOVE_ASSIGN(T) (__is_assignable(T, T&&) && __is_trivially_assignable(T, T&&))
#   elif BOOST_MOVE_HAS_TRAIT(has_trivial_move_assign)
#     define BOOST_MOVE_HAS_TRIVIAL_MOVE_ASSIGN(T) __has_trivial_move_assign(T)
#   endif
#   define BOOST_MOVE_ALIGNMENT_OF(T) __alignof(T)

#endif   //#if defined(BOOST_CLANG)

#if defined(__GNUC__) && ((__GNUC__ > 4) || ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 3) && !defined(__GCCXML__))) && !defined(BOOST_CLANG)

#ifdef BOOST_INTEL
#  define BOOST_MOVE_INTEL_TT_OPTS || ::boost::move_detail::is_pod<T>::value
#else
#  define BOOST_MOVE_INTEL_TT_OPTS
#endif

#   define BOOST_MOVE_IS_UNION(T) __is_union(T)
#   define BOOST_MOVE_IS_POD(T) __is_pod(T)
#   define BOOST_MOVE_IS_EMPTY(T) __is_empty(T)
#   define BOOST_MOVE_HAS_TRIVIAL_CONSTRUCTOR(T) ((__has_trivial_constructor(T) BOOST_MOVE_INTEL_TT_OPTS))

#   if defined(BOOST_GCC) && (BOOST_GCC > 50000)
#     define BOOST_MOVE_HAS_TRIVIAL_COPY(T) (__is_trivially_constructible(T, const T &))
#     define BOOST_MOVE_HAS_TRIVIAL_ASSIGN(T) (__is_trivially_assignable(T, const T &))
#   else
#     define BOOST_MOVE_HAS_TRIVIAL_COPY(T) ((__has_trivial_copy(T) BOOST_MOVE_INTEL_TT_OPTS))
#     define BOOST_MOVE_HAS_TRIVIAL_ASSIGN(T) ((__has_trivial_assign(T) BOOST_MOVE_INTEL_TT_OPTS) )
#   endif

#   define BOOST_MOVE_HAS_TRIVIAL_DESTRUCTOR(T) (__has_trivial_destructor(T) BOOST_MOVE_INTEL_TT_OPTS)
#   define BOOST_MOVE_HAS_NOTHROW_CONSTRUCTOR(T) (__has_nothrow_constructor(T) BOOST_MOVE_INTEL_TT_OPTS)
#   define BOOST_MOVE_HAS_NOTHROW_COPY(T) ((__has_nothrow_copy(T) BOOST_MOVE_INTEL_TT_OPTS))
#   define BOOST_MOVE_HAS_NOTHROW_ASSIGN(T) ((__has_nothrow_assign(T) BOOST_MOVE_INTEL_TT_OPTS))

#   if (!defined(unix) && !defined(__unix__)) || defined(__LP64__)
      // GCC sometimes lies about alignment requirements
      // of type double on 32-bit unix platforms, use the
      // old implementation instead in that case:
#     define BOOST_MOVE_ALIGNMENT_OF(T) __alignof__(T)
#   endif
#endif

#if defined(__ghs__) && (__GHS_VERSION_NUMBER >= 600)

#   define BOOST_MOVE_IS_UNION(T) __is_union(T)
#   define BOOST_MOVE_IS_POD(T) __is_pod(T)
#   define BOOST_MOVE_IS_EMPTY(T) __is_empty(T)
#   define BOOST_MOVE_HAS_TRIVIAL_CONSTRUCTOR(T) __has_trivial_constructor(T)
#   define BOOST_MOVE_HAS_TRIVIAL_COPY(T) (__has_trivial_copy(T))
#   define BOOST_MOVE_HAS_TRIVIAL_ASSIGN(T) (__has_trivial_assign(T))
#   define BOOST_MOVE_HAS_TRIVIAL_DESTRUCTOR(T) __has_trivial_destructor(T)
#   define BOOST_MOVE_HAS_NOTHROW_CONSTRUCTOR(T) __has_nothrow_constructor(T)
#   define BOOST_MOVE_HAS_NOTHROW_COPY(T) (__has_nothrow_copy(T))
#   define BOOST_MOVE_HAS_NOTHROW_ASSIGN(T) (__has_nothrow_assign(T))

#   define BOOST_MOVE_ALIGNMENT_OF(T) __alignof__(T)
#endif

# if defined(BOOST_CODEGEARC)
#   define BOOST_MOVE_IS_UNION(T) __is_union(T)
#   define BOOST_MOVE_IS_POD(T) __is_pod(T)
#   define BOOST_MOVE_IS_EMPTY(T) __is_empty(T)
#   define BOOST_MOVE_HAS_TRIVIAL_CONSTRUCTOR(T) (__has_trivial_default_constructor(T))
#   define BOOST_MOVE_HAS_TRIVIAL_COPY(T) (__has_trivial_copy_constructor(T))
#   define BOOST_MOVE_HAS_TRIVIAL_ASSIGN(T) (__has_trivial_assign(T))
#   define BOOST_MOVE_HAS_TRIVIAL_DESTRUCTOR(T) (__has_trivial_destructor(T))
#   define BOOST_MOVE_HAS_NOTHROW_CONSTRUCTOR(T) (__has_nothrow_default_constructor(T))
#   define BOOST_MOVE_HAS_NOTHROW_COPY(T) (__has_nothrow_copy_constructor(T))
#   define BOOST_MOVE_HAS_NOTHROW_ASSIGN(T) (__has_nothrow_assign(T))

#   define BOOST_MOVE_ALIGNMENT_OF(T) alignof(T)

#endif

namespace boost {
namespace move_detail {

//////////////////////////
//       unvoid_ref
//////////////////////////
template <typename T> struct unvoid_ref : add_lvalue_reference<T>{};
template <> struct unvoid_ref<void>                { typedef unvoid_ref & type; };
template <> struct unvoid_ref<const void>          { typedef unvoid_ref & type; };
template <> struct unvoid_ref<volatile void>       { typedef unvoid_ref & type; };
template <> struct unvoid_ref<const volatile void> { typedef unvoid_ref & type; };

//////////////////////////
//    add_const_reference
//////////////////////////
template <class T>
struct add_const_reference
{  typedef const T &type;   };

template <class T>
struct add_const_reference<T&>
{  typedef T& type;   };

}  //namespace move_detail {
}  //namespace boost {

#include <boost/move/detail/config_end.hpp>

#endif   //#ifndef BOOST_MOVE_DETAIL_TYPE_TRAITS_HPP
