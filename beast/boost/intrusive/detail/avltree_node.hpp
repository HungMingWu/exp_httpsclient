/////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2007-2013
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/intrusive for documentation.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef BOOST_INTRUSIVE_AVLTREE_NODE_HPP
#define BOOST_INTRUSIVE_AVLTREE_NODE_HPP

#ifndef BOOST_CONFIG_HPP
#  include <boost/config.hpp>
#endif

#if defined(BOOST_HAS_PRAGMA_ONCE)
#  pragma once
#endif

#include <boost/intrusive/pointer_rebind.hpp>
#include <boost/intrusive/avltree_algorithms.hpp>
#include <boost/intrusive/pointer_plus_bits.hpp>
#include <boost/intrusive/detail/mpl.hpp>

namespace boost {
namespace intrusive {

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//                Generic node_traits for any pointer type                 //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

//This is the compact representation: 3 pointers
template<class VoidPointer>
struct compact_avltree_node
{
   typedef typename pointer_rebind<VoidPointer, compact_avltree_node<VoidPointer> >::type       node_ptr;
   typedef typename pointer_rebind<VoidPointer, const compact_avltree_node<VoidPointer> >::type const_node_ptr;
   enum class balance : std::size_t { negative_t, zero_t, positive_t };
   node_ptr parent_, left_, right_;
};

//This is the normal representation: 3 pointers + enum
template<class VoidPointer>
struct avltree_node
{
   typedef typename pointer_rebind<VoidPointer, avltree_node<VoidPointer> >::type         node_ptr;
   typedef typename pointer_rebind<VoidPointer, const avltree_node<VoidPointer> >::type   const_node_ptr;
   enum class balance : std::size_t { negative_t, zero_t, positive_t };
   node_ptr parent_, left_, right_;
   balance balance_;
};

//This is the default node traits implementation
//using a node with 3 generic pointers plus an enum
template<class VoidPointer>
struct default_avltree_node_traits_impl
{
   typedef avltree_node<VoidPointer>      node;
   typedef typename node::node_ptr        node_ptr;
   typedef typename node::const_node_ptr  const_node_ptr;

   typedef typename node::balance balance;

   inline static node_ptr get_parent(const const_node_ptr & n)
   {  return n->parent_;  }

   inline static node_ptr get_parent(const node_ptr & n)
   {  return n->parent_;  }

   inline static void set_parent(node_ptr n, node_ptr p)
   {  n->parent_ = p;  }

   inline static node_ptr get_left(const const_node_ptr & n)
   {  return n->left_;  }

   inline static node_ptr get_left(const node_ptr & n)
   {  return n->left_;  }

   inline static void set_left(node_ptr n, node_ptr l)
   {  n->left_ = l;  }

   inline static node_ptr get_right(const const_node_ptr & n)
   {  return n->right_;  }

   inline static node_ptr get_right(const node_ptr & n)
   {  return n->right_;  }

   inline static void set_right(node_ptr n, node_ptr r)
   {  n->right_ = r;  }

   inline static balance get_balance(const const_node_ptr & n)
   {  return n->balance_;  }

   inline static balance get_balance(const node_ptr & n)
   {  return n->balance_;  }

   inline static void set_balance(const node_ptr & n, balance b)
   {  n->balance_ = b;  }

   inline static balance negative()
   {  return node::balance::negative_t;  }

   inline static balance zero()
   {  return node::balance::zero_t;  }

   inline static balance positive()
   {  return node::balance::positive_t;  }
};

//This is the compact node traits implementation
//using a node with 3 generic pointers
template<class VoidPointer>
struct compact_avltree_node_traits_impl
{
   typedef compact_avltree_node<VoidPointer> node;
   typedef typename node::node_ptr           node_ptr;
   typedef typename node::const_node_ptr     const_node_ptr;
   typedef typename node::balance balance;

   typedef pointer_plus_bits<node_ptr, 2> ptr_bit;

   inline static node_ptr get_parent(const const_node_ptr & n)
   {  return ptr_bit::get_pointer(n->parent_);  }

   inline static void set_parent(node_ptr n, node_ptr p)
   {  ptr_bit::set_pointer(n->parent_, p);  }

   inline static node_ptr get_left(const const_node_ptr & n)
   {  return n->left_;  }

   inline static void set_left(node_ptr n, node_ptr l)
   {  n->left_ = l;  }

   inline static node_ptr get_right(const const_node_ptr & n)
   {  return n->right_;  }

   inline static void set_right(node_ptr n, node_ptr r)
   {  n->right_ = r;  }

   inline static balance get_balance(const const_node_ptr & n)
   {  return (balance)ptr_bit::get_bits(n->parent_);  }

   inline static void set_balance(const node_ptr & n, balance b)
   {  ptr_bit::set_bits(n->parent_, (std::size_t)b);  }

   inline static balance negative()
   {  return node::balance::negative_t;  }

   inline static balance zero()
   {  return node::balance::zero_t;  }

   inline static balance positive()
   {  return node::balance::positive_t;  }
};

//Dispatches the implementation based on the boolean
template<class VoidPointer, bool Compact>
struct avltree_node_traits_dispatch
   :  public default_avltree_node_traits_impl<VoidPointer>
{};

template<class VoidPointer>
struct avltree_node_traits_dispatch<VoidPointer, true>
   :  public compact_avltree_node_traits_impl<VoidPointer>
{};

//Inherit from rbtree_node_traits_dispatch depending on the embedding capabilities
template<class VoidPointer, bool OptimizeSize = false>
using avltree_node_traits =
     avltree_node_traits_dispatch
         < VoidPointer
         , OptimizeSize &&
            max_pointer_plus_bits
            < VoidPointer
            , std::alignment_of<compact_avltree_node<VoidPointer> >::value
            >::value >= 2u
         >;

} //namespace intrusive
} //namespace boost

#endif //BOOST_INTRUSIVE_AVLTREE_NODE_HPP
