/////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga  2006-2014
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/intrusive for documentation.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef BOOST_INTRUSIVE_DETAIL_IITERATOR_HPP
#define BOOST_INTRUSIVE_DETAIL_IITERATOR_HPP

#ifndef BOOST_CONFIG_HPP
#  include <boost/config.hpp>
#endif

#if defined(BOOST_HAS_PRAGMA_ONCE)
#  pragma once
#endif

#include <boost/intrusive/detail/iterator.hpp>
#include <boost/intrusive/pointer_traits.hpp>
#include <boost/intrusive/detail/mpl.hpp>
#include <boost/intrusive/detail/is_stateful_value_traits.hpp>

namespace boost {
namespace intrusive {

template<class ValueTraits>
struct value_traits_pointers
{
   using value_traits_ptr = detail::default_action_t<ValueTraits,
       detail::type_value_traits_ptr_t,
       typename boost::intrusive::pointer_traits<typename ValueTraits::node_traits::node_ptr>::template
       rebind_pointer<ValueTraits>::type>;

   using const_value_traits_ptr = typename boost::intrusive::pointer_traits<value_traits_ptr>::template
      rebind_pointer<ValueTraits const>::type;
};

template<class ValueTraits, bool IsConst, class Category>
struct iiterator
{
   using value_traits = ValueTraits;
   using node_traits = typename value_traits::node_traits;
   using node = typename node_traits::node;
   using node_ptr = typename node_traits::node_ptr;
   using nodepointer_traits_t = ::boost::intrusive::pointer_traits<node_ptr>;
   using void_pointer = typename nodepointer_traits_t::template rebind_pointer<void>::type;
   using value_type = typename ValueTraits::value_type;
   using nonconst_pointer = typename ValueTraits::pointer;
   using yesconst_pointer = typename ValueTraits::const_pointer;
   using nonconst_reference = typename ::boost::intrusive::pointer_traits<nonconst_pointer>::reference;
   using yesconst_reference = typename ::boost::intrusive::pointer_traits<yesconst_pointer>::reference;
   using difference_type = typename nodepointer_traits_t::difference_type;
   using pointer = std::conditional_t<IsConst, yesconst_pointer, nonconst_pointer>;
   using reference = std::conditional_t<IsConst, yesconst_reference, nonconst_reference>;
   using iterator_type = iterator<Category,
                                  value_type,
                                  difference_type,
                                  pointer,
                                  reference>;
   using value_traits_ptr = typename value_traits_pointers<ValueTraits>::value_traits_ptr;
   using const_value_traits_ptr = typename value_traits_pointers<ValueTraits>::const_value_traits_ptr;
   static constexpr bool stateful_value_traits = detail::is_stateful_value_traits_v<value_traits>;
};

template<class NodePtr, class StoredPointer, bool StatefulValueTraits = true>
struct iiterator_members
{

   inline iiterator_members()
      : nodeptr_()//Value initialization to achieve "null iterators" (N3644)
   {}

   inline iiterator_members(const NodePtr &n_ptr, const StoredPointer &data)
      :  nodeptr_(n_ptr), ptr_(data)
   {}

   inline StoredPointer get_ptr() const
   {  return ptr_;  }

   NodePtr nodeptr_;
   StoredPointer ptr_;
};

template<class NodePtr, class StoredPointer>
struct iiterator_members<NodePtr, StoredPointer, false>
{
   inline iiterator_members()
      : nodeptr_()//Value initialization to achieve "null iterators" (N3644)
   {}

   inline iiterator_members(const NodePtr &n_ptr, const StoredPointer &)
      : nodeptr_(n_ptr)
   {}

   inline StoredPointer get_ptr() const
   {  return StoredPointer();  }

   NodePtr nodeptr_;
};

} //namespace intrusive
} //namespace boost

#endif //BOOST_INTRUSIVE_DETAIL_IITERATOR_HPP
