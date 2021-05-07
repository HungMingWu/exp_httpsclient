//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Pablo Halpern 2009. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2011-2014. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/intrusive for documentation.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef BOOST_INTRUSIVE_POINTER_TRAITS_HPP
#define BOOST_INTRUSIVE_POINTER_TRAITS_HPP

#include <boost/intrusive/pointer_rebind.hpp>
#include <boost/move/detail/pointer_element.hpp>
#include <boost/intrusive/detail/mpl.hpp>
#include <cstddef>
#include <memory>

#if defined(BOOST_HAS_PRAGMA_ONCE)
#  pragma once
#endif

namespace boost {
namespace intrusive {
namespace detail {

template <typename T, typename reference>
concept has_member_function_callable_with_pointer_to = requires {
    { T::pointer_to(std::declval<reference>()) };
};

template <typename T, typename reference>
concept has_member_function_callable_with_dynamic_cast_from = requires {
    { T::dynamic_cast_from(std::declval<reference>()) };
};

template <typename T, typename reference>
concept has_member_function_callable_with_static_cast_from = requires {
    { T::static_cast_from(std::declval<reference>()) };
};

template <typename T, typename reference>
concept has_member_function_callable_with_const_cast_from = requires {
    { T::const_cast_from(std::declval<reference>()) };
};

template <typename T>
using type_reference_t = typename T::reference;

template <typename T>
using type_difference_type_t = typename T::difference_type;

template <typename T>
using type_value_traits_ptr_t = typename T::value_traits_ptr;

template <typename T>
using type_element_type_t = typename T::element_type;

}  //namespace detail {


//! pointer_traits is the implementation of C++11 std::pointer_traits class with some
//! extensions like castings.
//!
//! pointer_traits supplies a uniform interface to certain attributes of pointer-like types.
//!
//! <b>Note</b>: When defining a custom family of pointers or references to be used with BI
//! library, make sure the public static conversion functions accessed through
//! the `pointer_traits` interface (`*_cast_from` and `pointer_to`) can
//! properly convert between const and nonconst referred member types
//! <b>without the use of implicit constructor calls</b>. It is suggested these
//! conversions be implemented as function templates, where the template
//! argument is the type of the object being converted from.
template <typename Ptr>
struct pointer_traits
{
   using pointer = Ptr;

   using element_type = detail::default_action_t<Ptr, detail::type_element_type_t,
                                                 typename boost::movelib::detail::first_param<Ptr>::type>;

   using difference_type = detail::default_action_t<Ptr, detail::type_difference_type_t, std::ptrdiff_t>;
   using reference = detail::default_action_t<Ptr, detail::type_reference_t, typename boost::intrusive::detail::unvoid_ref<element_type>::type>;

   template <class U>
   struct rebind_pointer
   {
      using type = boost::intrusive::pointer_rebind_t<Ptr, U>;
   };

   template <class U>
   using rebind = boost::intrusive::pointer_rebind_t<Ptr, U>;

   //! <b>Remark</b>: If element_type is (possibly cv-qualified) void, r type is unspecified; otherwise,
   //!   it is element_type &.
   //!
   //! <b>Returns</b>: A dereferenceable pointer to r obtained by calling Ptr::pointer_to(reference).
   //!   Non-standard extension: If such function does not exist, returns pointer(addressof(r));
   //!
   //! <b>Note</b>: For non-conforming compilers only the existence of a member function called
   //!   <code>pointer_to</code> is checked.
   inline static pointer pointer_to(reference r)
   {
      //Non-standard extension, it does not require Ptr::pointer_to. If not present
      //tries to converts &r to pointer.
      if constexpr (boost::intrusive::detail::has_member_function_callable_with_pointer_to<Ptr, reference>) {
          return Ptr::pointer_to(r);
      }
      else {
          return pointer(std::addressof(r));
      }
   }

   //! <b>Remark</b>: Non-standard extension.
   //!
   //! <b>Returns</b>: A dereferenceable pointer to r obtained by calling the static template function
   //!   Ptr::static_cast_from(UPpr/const UPpr &).
   //!   If such function does not exist, returns pointer_to(static_cast<element_type&>(*uptr))
   //!
   //! <b>Note</b>: For non-conforming compilers only the existence of a member function called
   //!   <code>static_cast_from</code> is checked.
   template<class UPtr>
   inline static pointer static_cast_from(const UPtr &uptr)
   {
      typedef const UPtr &RefArg;
      constexpr bool value =
          boost::intrusive::detail::has_member_function_callable_with_static_cast_from<pointer, RefArg> ||
          boost::intrusive::detail::has_member_function_callable_with_static_cast_from<pointer, UPtr>;
      if constexpr (value) {
        return Ptr::static_cast_from(uptr);
      }
      else {
        return uptr ? pointer_to(*static_cast<element_type*>(to_raw_pointer(uptr))) : pointer();
      }
   }

   //! <b>Remark</b>: Non-standard extension.
   //!
   //! <b>Returns</b>: A dereferenceable pointer to r obtained by calling the static template function
   //!   Ptr::const_cast_from<UPtr>(UPpr/const UPpr &).
   //!   If such function does not exist, returns pointer_to(const_cast<element_type&>(*uptr))
   //!
   //! <b>Note</b>: For non-conforming compilers only the existence of a member function called
   //!   <code>const_cast_from</code> is checked.
   template<class UPtr>
   inline static pointer const_cast_from(const UPtr &uptr)
   {
      typedef const UPtr &RefArg;
      constexpr bool value = 
         boost::intrusive::detail::has_member_function_callable_with_const_cast_from<pointer, RefArg> ||
         boost::intrusive::detail::has_member_function_callable_with_const_cast_from<pointer, UPtr>;
      if constexpr (value) {
        return Ptr::const_cast_from(uptr);
      }
      else {
        return uptr ? pointer_to(const_cast<element_type&>(*uptr)) : pointer();
      }
   }

   //! <b>Remark</b>: Non-standard extension.
   //!
   //! <b>Returns</b>: A dereferenceable pointer to r obtained by calling the static template function
   //!   Ptr::dynamic_cast_from<UPtr>(UPpr/const UPpr &).
   //!   If such function does not exist, returns pointer_to(*dynamic_cast<element_type*>(&*uptr))
   //!
   //! <b>Note</b>: For non-conforming compilers only the existence of a member function called
   //!   <code>dynamic_cast_from</code> is checked.
   template<class UPtr>
   inline static pointer dynamic_cast_from(const UPtr &uptr)
   {
      typedef const UPtr &RefArg;
      constexpr bool value = 
         boost::intrusive::detail::has_member_function_callable_with_dynamic_cast_from<pointer, RefArg> ||
         boost::intrusive::detail::has_member_function_callable_with_dynamic_cast_from<pointer, UPtr>;
      if constexpr (value) {
        return Ptr::dynamic_cast_from(uptr);
      }
      else {
        return uptr ? pointer_to(dynamic_cast<element_type&>(*uptr)) : pointer();
      }
   }

   ///@cond
   private:
   //priv_to_raw_pointer
   template <class T>
   inline static T* to_raw_pointer(T* p)
   {  return p; }

   template <class Pointer>
   inline static typename pointer_traits<Pointer>::element_type*
      to_raw_pointer(const Pointer &p)
   {  return pointer_traits::to_raw_pointer(p.operator->());  }

   ///@endcond
};

///@cond

// Remove cv qualification from Ptr parameter to pointer_traits:
template <typename Ptr>
struct pointer_traits<const Ptr> : pointer_traits<Ptr> {};
template <typename Ptr>
struct pointer_traits<volatile Ptr> : pointer_traits<Ptr> { };
template <typename Ptr>
struct pointer_traits<const volatile Ptr> : pointer_traits<Ptr> { };
// Remove reference from Ptr parameter to pointer_traits:
template <typename Ptr>
struct pointer_traits<Ptr&> : pointer_traits<Ptr> { };

///@endcond

//! Specialization of pointer_traits for raw pointers
//!
template <typename T>
struct pointer_traits<T*>
{
   using element_type = T;
   using pointer = T*;
   using difference_type = std::ptrdiff_t;

   using reference = typename boost::intrusive::detail::unvoid_ref<element_type>::type;

   template <class U>
   using rebind = U*;

   template <class U>
   struct rebind_pointer
   {
       using type = U*;
   };

   //! <b>Returns</b>: addressof(r)
   //!
   inline static pointer pointer_to(reference r)
   { return std::addressof(r); }

   //! <b>Returns</b>: static_cast<pointer>(uptr)
   //!
   template<class U>
   inline static pointer static_cast_from(U *uptr)
   {  return static_cast<pointer>(uptr);  }

   //! <b>Returns</b>: const_cast<pointer>(uptr)
   //!
   template<class U>
   inline static pointer const_cast_from(U *uptr)
   {  return const_cast<pointer>(uptr);  }

   //! <b>Returns</b>: dynamic_cast<pointer>(uptr)
   //!
   template<class U>
   inline static pointer dynamic_cast_from(U *uptr)
   {  return dynamic_cast<pointer>(uptr);  }
};

}  //namespace container {
}  //namespace boost {

#endif // ! defined(BOOST_INTRUSIVE_POINTER_TRAITS_HPP)
