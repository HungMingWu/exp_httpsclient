/////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Olaf Krzikalla 2004-2006.
// (C) Copyright Ion Gaztanaga  2006-2013
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/intrusive for documentation.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef BOOST_INTRUSIVE_SLIST_ITERATOR_HPP
#define BOOST_INTRUSIVE_SLIST_ITERATOR_HPP

#ifndef BOOST_CONFIG_HPP
#  include <boost/config.hpp>
#endif

#if defined(BOOST_HAS_PRAGMA_ONCE)
#  pragma once
#endif

#include <boost/intrusive/detail/iiterator.hpp>
#include <boost/intrusive/detail/mpl.hpp>

namespace boost {
namespace intrusive {


// slist_iterator provides some basic functions for a
// node oriented bidirectional iterator:
template<class ValueTraits, bool IsConst>
class slist_iterator
{
   private:
   using types_t = iiterator<ValueTraits, IsConst, std::forward_iterator_tag>;

   static const bool stateful_value_traits =                types_t::stateful_value_traits;

   using value_traits = ValueTraits;
   using node_traits = typename types_t::node_traits;

   using node = typename types_t::node;
   using node_ptr = typename types_t::node_ptr;
   using const_value_traits_ptr = typename types_t::const_value_traits_ptr;
   class nat;
   using nonconst_iterator = std::conditional_t<IsConst
                  , slist_iterator<value_traits, false>
                  , nat>;

   public:
   using difference_type = typename types_t::iterator_type::difference_type;
   using value_type = typename types_t::iterator_type::value_type;
   using pointer = typename types_t::iterator_type::pointer;
   using reference = typename types_t::iterator_type::reference;
   using iterator_category = typename types_t::iterator_type::iterator_category;

   inline slist_iterator()
   {}

   inline explicit slist_iterator(const node_ptr & nodeptr, const const_value_traits_ptr &traits_ptr)
      : members_(nodeptr, traits_ptr)
   {}

   inline slist_iterator(const slist_iterator &other)
      :  members_(other.pointed_node(), other.get_value_traits())
   {}

   inline slist_iterator(const nonconst_iterator &other)
      :  members_(other.pointed_node(), other.get_value_traits())
   {}

   inline slist_iterator &operator=(const slist_iterator &other)
   {  members_.nodeptr_ = other.members_.nodeptr_;  return *this;  }

   inline node_ptr pointed_node() const
   { return members_.nodeptr_; }

   inline slist_iterator &operator=(const node_ptr &node)
   {  members_.nodeptr_ = node;  return static_cast<slist_iterator&>(*this);  }

   inline const_value_traits_ptr get_value_traits() const
   {  return members_.get_ptr(); }

   public:
   inline slist_iterator& operator++()
   {
      members_.nodeptr_ = node_traits::get_next(members_.nodeptr_);
      return static_cast<slist_iterator&> (*this);
   }

   inline slist_iterator operator++(int)
   {
      slist_iterator result (*this);
      members_.nodeptr_ = node_traits::get_next(members_.nodeptr_);
      return result;
   }

   inline friend bool operator== (const slist_iterator& l, const slist_iterator& r)
   {  return l.pointed_node() == r.pointed_node();   }

   inline friend bool operator!= (const slist_iterator& l, const slist_iterator& r)
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

   inline slist_iterator<ValueTraits, false> unconst() const
   {  return slist_iterator<ValueTraits, false>(this->pointed_node(), this->get_value_traits());   }

   private:
   iiterator_members<node_ptr, const_value_traits_ptr, stateful_value_traits> members_;
};

} //namespace intrusive
} //namespace boost

#endif //BOOST_INTRUSIVE_SLIST_ITERATOR_HPP
