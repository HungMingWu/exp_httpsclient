/////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga  2007-2013
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/intrusive for documentation.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef BOOST_INTRUSIVE_TREE_ITERATOR_HPP
#define BOOST_INTRUSIVE_TREE_ITERATOR_HPP

#ifndef BOOST_CONFIG_HPP
#  include <boost/config.hpp>
#endif

#if defined(BOOST_HAS_PRAGMA_ONCE)
#  pragma once
#endif

#include <boost/intrusive/detail/iiterator.hpp>
#include <boost/intrusive/detail/bstree_algorithms_base.hpp>

namespace boost {
namespace intrusive {

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//                   Implementation of the tree iterator                   //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

// tree_iterator provides some basic functions for a
// node oriented bidirectional iterator:
template<class ValueTraits, bool IsConst>
class tree_iterator
{
   private:
   using types_t = iiterator<ValueTraits, IsConst, std::bidirectional_iterator_tag>;
   using value_traits = typename types_t::value_traits;
   using node_traits = typename types_t::node_traits;
   using node = typename types_t::node;
   using node_ptr = typename types_t::node_ptr;
   using const_value_traits_ptr = typename types_t::const_value_traits_ptr;
   using node_algorithms = bstree_algorithms_base<node_traits>;

   static const bool stateful_value_traits = types_t::stateful_value_traits;

   class nat;
   using nonconst_iterator = std::conditional_t<IsConst, tree_iterator<value_traits, false>, nat>;

   public:
   using difference_type = typename types_t::iterator_type::difference_type;
   using value_type = typename types_t::iterator_type::value_type;
   using pointer = typename types_t::iterator_type::pointer;
   using reference = typename types_t::iterator_type::reference;
   using iterator_category = typename types_t::iterator_type::iterator_category;

   inline tree_iterator()
   {}

   inline explicit tree_iterator(const node_ptr & nodeptr, const const_value_traits_ptr &traits_ptr)
      : members_(nodeptr, traits_ptr)
   {}

   inline tree_iterator(const tree_iterator &other)
      :  members_(other.pointed_node(), other.get_value_traits())
   {}

   inline tree_iterator(const nonconst_iterator &other)
      :  members_(other.pointed_node(), other.get_value_traits())
   {}

   inline tree_iterator &operator=(const tree_iterator &other)
   {  members_.nodeptr_ = other.members_.nodeptr_;  return *this;  }

   inline tree_iterator &operator=(const node_ptr &nodeptr)
   {  members_.nodeptr_ = nodeptr;  return *this;  }

   inline node_ptr pointed_node() const
   { return members_.nodeptr_; }

   public:
   inline tree_iterator& operator++()
   {
      members_.nodeptr_ = node_algorithms::next_node(members_.nodeptr_);
      return *this;
   }

   tree_iterator operator++(int)
   {
      tree_iterator result (*this);
      members_.nodeptr_ = node_algorithms::next_node(members_.nodeptr_);
      return result;
   }

   inline tree_iterator& operator--()
   {
      members_.nodeptr_ = node_algorithms::prev_node(members_.nodeptr_);
      return *this;
   }

   tree_iterator operator--(int)
   {
      tree_iterator result (*this);
      members_.nodeptr_ = node_algorithms::prev_node(members_.nodeptr_);
      return result;
   }

   inline tree_iterator&  go_left()
   {
      members_.nodeptr_ = node_traits::get_left(members_.nodeptr_);
      return *this;
   }

   inline tree_iterator&  go_right()
   {
      members_.nodeptr_ = node_traits::get_right(members_.nodeptr_);
      return *this;
   }

   inline tree_iterator&  go_parent()
   {
      members_.nodeptr_ = node_traits::get_parent(members_.nodeptr_);
      return *this;
   }

   inline bool operator! () const
   {  return !members_.nodeptr_;   }

   inline friend bool operator== (const tree_iterator& l, const tree_iterator& r)
   { return l.pointed_node() == r.pointed_node(); }

   inline friend bool operator!= (const tree_iterator& l, const tree_iterator& r)
   {  return !(l == r);   }

   inline reference operator*() const
   {  return *operator->();   }

   inline pointer operator->() const
   {
       if constexpr (stateful_value_traits) {
           return this->get_value_traits()->to_value_ptr(members_.nodeptr_);
       }
       else {
           return ValueTraits::to_value_ptr(members_.nodeptr_);
       }
   }

   inline const_value_traits_ptr get_value_traits() const
   {  return members_.get_ptr();  }

   tree_iterator end_iterator_from_it() const
   {
      return tree_iterator(node_algorithms::get_header(this->pointed_node()), this->get_value_traits());
   }

   tree_iterator<value_traits, false> unconst() const
   {  return tree_iterator<value_traits, false>(this->pointed_node(), this->get_value_traits());   }

   private:
   iiterator_members<node_ptr, const_value_traits_ptr, stateful_value_traits> members_;
};

} //namespace intrusive
} //namespace boost

#endif //BOOST_INTRUSIVE_TREE_ITERATOR_HPP
