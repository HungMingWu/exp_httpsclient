//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2014-2017. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/move for documentation.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef BOOST_MOVE_DETAIL_POINTER_ELEMENT_HPP
#define BOOST_MOVE_DETAIL_POINTER_ELEMENT_HPP

#ifndef BOOST_CONFIG_HPP
#  include <boost/config.hpp>
#endif

#if defined(BOOST_HAS_PRAGMA_ONCE)
#  pragma once
#endif

namespace boost {
namespace movelib {
namespace detail{

//////////////////////
//struct first_param
//////////////////////

template <typename T> 
struct first_param
{  
	using type = void;
};

template <template <typename, typename...> class TemplateClass, typename T, typename... Args>
struct first_param<TemplateClass<T, Args...>>
{
	using type = T;
};

template <typename T>
using first_param_t = typename first_param<T>::type;

template <typename T>
concept has_internal_pointer_element_v = requires {
	T::element_type;
};

template<class Ptr, bool = has_internal_pointer_element_v<Ptr>>
struct pointer_element_impl
{
   using type = typename Ptr::element_type;
};

template<class Ptr>
struct pointer_element_impl<Ptr, false>
{
	using type = boost::movelib::detail::first_param_t<Ptr>;
};

template <typename Ptr>
using pointer_element_impl_t = typename pointer_element_impl<Ptr>::type;

} //namespace detail

template <typename Ptr>
struct pointer_element
{
   using type = boost::movelib::detail::pointer_element_impl_t<Ptr>;
};

template <typename T>
struct pointer_element<T*> {
	using type = T;
};

template <typename Ptr>
using pointer_element_t = typename pointer_element<Ptr>::type;

}  //namespace movelib {
}  //namespace boost {

#endif // defined(BOOST_MOVE_DETAIL_POINTER_ELEMENT_HPP)
