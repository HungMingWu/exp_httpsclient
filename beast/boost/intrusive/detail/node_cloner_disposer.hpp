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

#ifndef BOOST_INTRUSIVE_DETAIL_NODE_CLONER_DISPOSER_HPP
#define BOOST_INTRUSIVE_DETAIL_NODE_CLONER_DISPOSER_HPP

#ifndef BOOST_CONFIG_HPP
#  include <boost/config.hpp>
#endif

#if defined(BOOST_HAS_PRAGMA_ONCE)
#  pragma once
#endif

#include <boost/intrusive/link_mode.hpp>
#include <boost/intrusive/detail/mpl.hpp>
#include <boost/intrusive/detail/ebo_functor_holder.hpp>
#include <boost/intrusive/detail/algo_type.hpp>
#include <boost/intrusive/detail/assert.hpp>

namespace boost {
namespace intrusive {
namespace detail {

template <typename F, typename ValueTraits, algo_types AlgoType, bool IsConst = true>
struct node_cloner
{
   using value_traits = ValueTraits;
   using node_traits = typename value_traits::node_traits;
   using node_ptr = typename node_traits::node_ptr;
   using node_algorithms = get_algo_t<AlgoType, node_traits>;
   static constexpr bool safemode_or_autounlink =           is_safe_autounlink(value_traits::link_mode);
   using value_type = typename value_traits::value_type;
   using pointer = typename value_traits::pointer;
   using const_pointer = typename value_traits::const_pointer;
   using node = typename node_traits::node;
   using const_node_ptr = typename value_traits::const_node_ptr;
   using reference = typename pointer_traits<pointer>::reference;
   using const_reference = typename pointer_traits<const_pointer>::reference                            ;
   using reference_type = std::conditional_t<IsConst, const_reference, reference>;
   [[no_unique_address]] F                      functor;
   const ValueTraits* const traits_;

   node_cloner(F f, const ValueTraits *traits)
      : functor(f), traits_(traits)
   {}

   // tree-based containers use this method, which is proxy-reference friendly
   inline node_ptr operator()(const node_ptr & p)
   {
      reference_type v = *traits_->to_value_ptr(p);
      node_ptr n = traits_->to_node_ptr(*functor(v));
      //Cloned node must be in default mode if the linking mode requires it
      BOOST_INTRUSIVE_SAFE_HOOK_DEFAULT_ASSERT(!safemode_or_autounlink || node_algorithms::unique(n));
      return n;
   }
};

template <algo_types AlgoType, bool IsConst = true, typename F, typename ValueTraits>
auto make_node_cloner(F f, const ValueTraits* traits)
{
    return node_cloner<F, ValueTraits, AlgoType, IsConst>(f, traits);
}


template <typename F, typename ValueTraits, algo_types AlgoType>
struct node_disposer
{
   using value_traits = ValueTraits;
   using node_traits = typename value_traits::node_traits;
   using node_ptr = typename node_traits::node_ptr;
   using node_algorithms = get_algo_t<AlgoType, node_traits>;
   static constexpr bool safemode_or_autounlink = is_safe_autounlink(value_traits::link_mode);
   [[no_unique_address]] F                      functor;
   const ValueTraits* const                     traits_;

   node_disposer(F f, const ValueTraits *cont)
      : functor(f), traits_(cont)
   {}

   inline void operator()(const node_ptr & p)
   {
      if constexpr (safemode_or_autounlink)
         node_algorithms::init(p);
      functor(traits_->to_value_ptr(p));
   }
};

template <algo_types AlgoType, typename F, typename ValueTraits>
auto make_node_disposer(F f, const ValueTraits* cont)
{
    return node_disposer<F, ValueTraits, AlgoType>(f, cont);
}

}  //namespace detail{
}  //namespace intrusive{
}  //namespace boost{

#endif //BOOST_INTRUSIVE_DETAIL_NODE_CLONER_DISPOSER_HPP
