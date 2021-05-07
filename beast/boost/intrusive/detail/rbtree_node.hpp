/////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Olaf Krzikalla 2004-2006.
// (C) Copyright Ion Gaztanaga  2006-2013.
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/intrusive for documentation.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef BOOST_INTRUSIVE_RBTREE_NODE_HPP
#define BOOST_INTRUSIVE_RBTREE_NODE_HPP

#ifndef BOOST_CONFIG_HPP
#  include <boost/config.hpp>
#endif

#if defined(BOOST_HAS_PRAGMA_ONCE)
#  pragma once
#endif

#include <boost/intrusive/pointer_rebind.hpp>
#include <boost/intrusive/rbtree_algorithms.hpp>
#include <boost/intrusive/pointer_plus_bits.hpp>
#include <boost/intrusive/detail/mpl.hpp>
#include <boost/intrusive/detail/tree_node.hpp>

namespace boost {
namespace intrusive {

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//                Generic node_traits for any pointer type                 //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

//This is the compact representation: 3 pointers
template<class VoidPointer>
struct compact_rbtree_node
{
   using node = compact_rbtree_node<VoidPointer>;
   using node_ptr = pointer_rebind_t<VoidPointer, node>;
   using const_node_ptr = pointer_rebind_t<VoidPointer, const node>;
   enum color { red_t, black_t };
   node_ptr parent_, left_, right_;
};

//This is the normal representation: 3 pointers + enum
template<class VoidPointer>
struct rbtree_node
{
   using node = rbtree_node<VoidPointer>;
   using node_ptr = pointer_rebind_t<VoidPointer, node>;
   using const_node_ptr = pointer_rebind_t<VoidPointer, const node>;

   enum color { red_t, black_t };
   node_ptr parent_, left_, right_;
   color color_;
};

//This is the default node traits implementation
//using a node with 3 generic pointers plus an enum
template<class VoidPointer>
struct default_rbtree_node_traits_impl
{
   typedef rbtree_node<VoidPointer> node;
   typedef typename node::node_ptr        node_ptr;
   typedef typename node::const_node_ptr  const_node_ptr;

   typedef typename node::color color;

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

   inline static color get_color(const const_node_ptr & n)
   {  return n->color_;  }

   inline static color get_color(const node_ptr & n)
   {  return n->color_;  }

   inline static void set_color(const node_ptr & n, color c)
   {  n->color_ = c;  }

   inline static color black()
   {  return node::black_t;  }

   inline static color red()
   {  return node::red_t;  }
};

//This is the compact node traits implementation
//using a node with 3 generic pointers
template<class VoidPointer>
struct compact_rbtree_node_traits_impl
{
   typedef compact_rbtree_node<VoidPointer> node;
   typedef typename node::node_ptr        node_ptr;
   typedef typename node::const_node_ptr  const_node_ptr;

   typedef pointer_plus_bits<node_ptr, 1> ptr_bit;

   typedef typename node::color color;

   inline static node_ptr get_parent(const const_node_ptr & n)
   {  return ptr_bit::get_pointer(n->parent_);  }

   inline static node_ptr get_parent(const node_ptr & n)
   {  return ptr_bit::get_pointer(n->parent_);  }

   inline static void set_parent(node_ptr n, node_ptr p)
   {  ptr_bit::set_pointer(n->parent_, p);  }

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

   inline static color get_color(const const_node_ptr & n)
   {  return (color)ptr_bit::get_bits(n->parent_);  }

   inline static color get_color(const node_ptr & n)
   {  return (color)ptr_bit::get_bits(n->parent_);  }

   inline static void set_color(const node_ptr & n, color c)
   {  ptr_bit::set_bits(n->parent_, c != 0);  }

   inline static color black()
   {  return node::black_t;  }

   inline static color red()
   {  return node::red_t;  }
};

//Dispatches the implementation based on the boolean
template<class VoidPointer, bool Compact>
struct rbtree_node_traits_dispatch
   :  public default_rbtree_node_traits_impl<VoidPointer>
{};

template<class VoidPointer>
struct rbtree_node_traits_dispatch<VoidPointer, true>
   :  public compact_rbtree_node_traits_impl<VoidPointer>
{};

//Inherit from rbtree_node_traits_dispatch depending on the embedding capabilities
template<class VoidPointer, bool OptimizeSize = false>
struct rbtree_node_traits
   :  public rbtree_node_traits_dispatch
         < VoidPointer
         ,  OptimizeSize &&
           (max_pointer_plus_bits
            < VoidPointer
            , std::alignment_of<compact_rbtree_node<VoidPointer> >::value
            >::value >= 1)
         >
{};

} //namespace intrusive
} //namespace boost

#endif //BOOST_INTRUSIVE_RBTREE_NODE_HPP
