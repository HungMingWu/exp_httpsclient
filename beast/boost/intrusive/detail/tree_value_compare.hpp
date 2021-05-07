//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2015-2015. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/container for documentation.
//
//////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_INTRUSIVE_DETAIL_TREE_VALUE_COMPARE_HPP
#define BOOST_INTRUSIVE_DETAIL_TREE_VALUE_COMPARE_HPP

#ifndef BOOST_CONFIG_HPP
#  include <boost/config.hpp>
#endif

#if defined(BOOST_HAS_PRAGMA_ONCE)
#  pragma once
#endif

#include <boost/intrusive/detail/mpl.hpp>
#include <boost/intrusive/detail/ebo_functor_holder.hpp>
#include <boost/intrusive/pointer_traits.hpp>

namespace boost{
namespace intrusive{

//Needed to support smart references to value types
template <class From, class ValuePtr>
struct disable_if_smartref_to
   : detail::disable_if_c 
      <  std::is_same_v
            <From, typename pointer_traits
               <ValuePtr>
                  ::reference>
      || std::is_same_v
            <From, typename pointer_traits
                     < 
                        pointer_rebind_t<ValuePtr, const boost::movelib::pointer_element_t<ValuePtr>>>
                  ::reference>
      >
{};
template <typename From, typename ValuePtr>
concept disable_if_smartref_to_v = std::is_same_v<From, typename pointer_traits<ValuePtr>::reference> ||
                                   std::is_same_v<From, typename pointer_traits<pointer_rebind_t<ValuePtr, const boost::movelib::pointer_element_t<ValuePtr>>>::reference>;
//This function object takes a KeyCompare function object
//and compares values that contains keys using KeyOfValue
template< class ValuePtr, class KeyCompare, class KeyOfValue, class Ret = bool
        , bool = std::is_same_v
   <boost::movelib::pointer_element_t<ValuePtr>, typename KeyOfValue::type>>
struct tree_value_compare
   :  public boost::intrusive::detail::ebo_functor_holder<KeyCompare>
{
   using value_type = boost::movelib::pointer_element_t<ValuePtr>;
   using key_compare = KeyCompare;
   using key_of_value = KeyOfValue;
   using key_type = typename KeyOfValue::type;

   [[no_unique_address]] key_compare functor;

   inline explicit tree_value_compare(const key_compare &kcomp)
      :  functor(kcomp)
   {}

   inline const key_compare &key_comp() const
   {  return functor;  }

   inline Ret operator()(const key_type &key) const
   {  return functor(key);   }

   inline Ret operator()(const value_type &value) const
   {  return functor(KeyOfValue()(value));  }

   template<class U>
   inline Ret operator()( const U &nonkey
                                             , typename disable_if_smartref_to<U, ValuePtr>::type* = 0) const
   {  return functor(nonkey);  }

   inline Ret operator()(const key_type &key1, const key_type &key2) const
   {  return functor(key1, key2);  }

   inline Ret operator()(const value_type &value1, const value_type &value2) const
   {  return functor(KeyOfValue()(value1), KeyOfValue()(value2));  }

   inline Ret operator()(const key_type &key1, const value_type &value2) const
   {  return functor(key1, KeyOfValue()(value2));  }

   inline Ret operator()(const value_type &value1, const key_type &key2) const
   {  return functor(KeyOfValue()(value1), key2);  }

   template<class U>
   inline Ret operator()( const key_type &key1, const U &nonkey2
                                              , typename disable_if_smartref_to<U, ValuePtr>::type* = 0) const
   {  return functor(key1, nonkey2);  }

   template<class U>
   inline Ret operator()( const U &nonkey1, const key_type &key2
                                              , typename disable_if_smartref_to<U, ValuePtr>::type* = 0) const
   {  return functor(nonkey1, key2);  }

   template<class U>
   inline Ret operator()( const value_type &value1, const U &nonvalue2
                                              , typename disable_if_smartref_to<U, ValuePtr>::type* = 0) const
   {  return functor(KeyOfValue()(value1), nonvalue2);  }

   template<class U>
   inline Ret operator()( const U &nonvalue1, const value_type &value2
                                              , typename disable_if_smartref_to<U, ValuePtr>::type* = 0) const
   {  return functor(nonvalue1, KeyOfValue()(value2));  }
};

template<class ValuePtr, class KeyCompare, class KeyOfValue, class Ret>
struct tree_value_compare<ValuePtr, KeyCompare, KeyOfValue, Ret, true>
   :  public boost::intrusive::detail::ebo_functor_holder<KeyCompare>
{
   using value_type = boost::movelib::pointer_element_t<ValuePtr>;
   using key_compare = KeyCompare;
   using key_of_value = KeyOfValue;
   using key_type = typename KeyOfValue::type;

   [[no_unique_address]] key_compare functor;

   inline explicit tree_value_compare(const key_compare &kcomp)
      :  functor(kcomp)
   {}

   inline const key_compare &key_comp() const
   {  return functor;  }

   inline Ret operator()(const key_type &key) const
   {  return functor(key);   }

   template<class U>
   inline Ret operator()( const U &nonkey
                                             , typename disable_if_smartref_to<U, ValuePtr>::type* = 0) const
   {  return functor(nonkey);  }

   inline Ret operator()(const key_type &key1, const key_type &key2) const
   {  return functor(key1, key2);  }

   template<class U>
   inline Ret operator()( const key_type &key1, const U &nonkey2
                                              , typename disable_if_smartref_to<U, ValuePtr>::type* = 0) const
   {  return functor(key1, nonkey2);  }

   template<class U>
   inline Ret operator()(const U &nonkey1, const key_type &key2
                                              , typename disable_if_smartref_to<U, ValuePtr>::type* = 0) const
   {  return functor(nonkey1, key2);  }
};

}  //namespace intrusive{
}  //namespace boost{

#endif   //#ifdef BOOST_INTRUSIVE_DETAIL_TREE_VALUE_COMPARE_HPP
