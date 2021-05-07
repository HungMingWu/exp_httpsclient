/////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga  2014-2014
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/intrusive for documentation.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef BOOST_INTRUSIVE_DETAIL_KEY_NODEPTR_COMP_HPP
#define BOOST_INTRUSIVE_DETAIL_KEY_NODEPTR_COMP_HPP

#ifndef BOOST_CONFIG_HPP
#  include <boost/config.hpp>
#endif

#if defined(BOOST_HAS_PRAGMA_ONCE)
#  pragma once
#endif

#include <boost/intrusive/detail/mpl.hpp>
#include <boost/intrusive/detail/ebo_functor_holder.hpp>
#include <boost/intrusive/detail/tree_value_compare.hpp>


namespace boost {
namespace intrusive {
namespace detail {

template <typename P1, typename const_node_ptr, typename node_ptr>
concept is_same_or_nodeptr_convertible =
    (std::is_same_v<P1, const_node_ptr> || std::is_same_v<P1, node_ptr>) ||
    std::is_convertible_v<P1, const_node_ptr>;

template <class KeyTypeKeyCompare, class ValueTraits, class KeyOfValue>
struct key_nodeptr_comp_types
{
   using value_traits = ValueTraits;
   using value_type = typename value_traits::value_type;
   using node_ptr = typename value_traits::node_ptr;
   using const_node_ptr = typename value_traits::const_node_ptr;
   using key_of_value = std::conditional_t<std::is_same_v<KeyOfValue, void>,
                                           detail::identity<value_type>,
                                           KeyOfValue>;
   using base_t = tree_value_compare<typename ValueTraits::pointer, KeyTypeKeyCompare, key_of_value>;
};

//This function object transforms a key comparison type to
//a function that can compare nodes or nodes with nodes or keys.
template < class KeyTypeKeyCompare
         , class ValueTraits
         , class KeyOfValue = void
         >
struct key_nodeptr_comp {
public:
   using types_t = key_nodeptr_comp_types<KeyTypeKeyCompare, ValueTraits, KeyOfValue>;
   using value_traits = typename types_t::value_traits;
   using value_type = typename types_t::value_type;
   using node_ptr = typename types_t::node_ptr;
   using const_node_ptr = typename types_t::const_node_ptr;
   using base_t = typename types_t::base_t;
   using key_of_value = typename types_t::key_of_value;
   [[no_unique_address]] base_t functor;

   inline const base_t& base() const
   {  return functor; }

   inline key_nodeptr_comp(KeyTypeKeyCompare kcomp, const ValueTraits *traits)
      :  functor(kcomp), traits_(traits)
   {}

   //pred(pnode)
   template<class T1>
   inline bool operator()(const T1 &t1) const
   requires (is_same_or_nodeptr_convertible<T1, const_node_ptr, node_ptr>)
   {  return functor(key_of_value()(*traits_->to_value_ptr(t1)));  }

   //operator() 2 arg
   template<class T1, class T2>
   inline bool operator()(const T1 &t1, const T2 &t2) const
   {
       if constexpr (is_same_or_nodeptr_convertible<T1, const_node_ptr, node_ptr>) {
         if constexpr (is_same_or_nodeptr_convertible<T2, const_node_ptr, node_ptr>)
           return functor(*traits_->to_value_ptr(t1), *traits_->to_value_ptr(t2)); // pred(pnode, pnode)
         else
           return functor(*traits_->to_value_ptr(t1), t2); // pred(pnode, key)
       }
       else {
         if constexpr (is_same_or_nodeptr_convertible<T2, const_node_ptr, node_ptr>)
           return functor(t1, *traits_->to_value_ptr(t2)); // pred(key, pnode)
         else
           return functor(t1, t2); // pred(key, key)
       }
   }

   const ValueTraits *const traits_;
};

}  //namespace detail{
}  //namespace intrusive{
}  //namespace boost{

#endif //BOOST_INTRUSIVE_DETAIL_KEY_NODEPTR_COMP_HPP
