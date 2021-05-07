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

#ifndef BOOST_INTRUSIVE_DETAIL_HOOK_TRAITS_HPP
#define BOOST_INTRUSIVE_DETAIL_HOOK_TRAITS_HPP

#ifndef BOOST_CONFIG_HPP
#  include <boost/config.hpp>
#endif

#if defined(BOOST_HAS_PRAGMA_ONCE)
#  pragma once
#endif

#include <boost/intrusive/pointer_traits.hpp>
#include <boost/intrusive/detail/parent_from_member.hpp>
#include <boost/intrusive/link_mode.hpp>
#include <boost/intrusive/detail/mpl.hpp>
#include <boost/move/detail/to_raw_pointer.hpp>
#include <boost/intrusive/detail/node_holder.hpp>

namespace boost {
namespace intrusive {

template<class T, class NodePtr, class Tag, unsigned int Type>
struct bhtraits_base
{
   public:
   using node_ptr = NodePtr;
   using node = typename pointer_traits<node_ptr>::element_type;
   using node_holder_type = node_holder<node, Tag, Type>;
   using value_type = T;
   using const_node_ptr = typename pointer_traits<node_ptr>::
       template rebind_pointer<const node>::type;
   using pointer = typename pointer_traits<node_ptr>::
       template rebind_pointer<T>::type;
   using const_pointer = typename pointer_traits<node_ptr>::
       template rebind_pointer<const T>::type;
   using reference = T&;
   using const_reference = const T&;
   using node_holder_reference = node_holder_type&;
   using const_node_holder_reference = const node_holder_type&;
   using node_reference = node&;
   using const_node_reference = const node&;

   inline static pointer to_value_ptr(const node_ptr & n)
   {
      pointer p = pointer_traits<pointer>::pointer_to
         (static_cast<reference>(static_cast<node_holder_reference>(*n)));
      BOOST_ASSERT(!!p);
      return p;
   }

   inline static const_pointer to_value_ptr(const const_node_ptr & n)
   {
      const_pointer p = pointer_traits<const_pointer>::pointer_to
         (static_cast<const_reference>(static_cast<const_node_holder_reference>(*n)));
      BOOST_ASSERT(!!p);
      return p;
   }

   inline static node_ptr to_node_ptr(reference value)
   {
      node_ptr p = pointer_traits<node_ptr>::pointer_to
         (static_cast<node_reference>(static_cast<node_holder_reference>(value)));
      BOOST_ASSERT(!!p);
      return p;
   }

   inline static const_node_ptr to_node_ptr(const_reference value)
   {
      const_node_ptr p = pointer_traits<const_node_ptr>::pointer_to
         (static_cast<const_node_reference>(static_cast<const_node_holder_reference>(value)));
      BOOST_ASSERT(!!p);
      return p;
   }
};

template<class T, class NodeTraits, link_mode_type LinkMode, class Tag, unsigned int Type>
struct bhtraits
   : public bhtraits_base<T, typename NodeTraits::node_ptr, Tag, Type>
{
   static const link_mode_type link_mode = LinkMode;
   using node_traits = NodeTraits;
};


template<class T, class Hook, Hook T::* P>
struct mhtraits
{
   public:
   using hook_type = Hook;
   using node_traits = typename hook_type::hooktags::node_traits;
   using node = typename node_traits::node;
   using value_type = T;
   using node_ptr = typename node_traits::node_ptr;
   using const_node_ptr = typename node_traits::const_node_ptr;
   using pointer = typename pointer_traits<node_ptr>::
       template rebind_pointer<T>::type;
   using const_pointer = typename pointer_traits<node_ptr>::
       template rebind_pointer<const T>::type;
   using reference = T&;
   using const_reference = const T&;
   using node_reference = node&;
   using const_node_reference = const node&;
   using hook_reference = hook_type&;
   using const_hook_reference = const hook_type&;

   static const link_mode_type link_mode = Hook::hooktags::link_mode;

   inline static node_ptr to_node_ptr(reference value)
   {
      return pointer_traits<node_ptr>::pointer_to
         (static_cast<node_reference>(static_cast<hook_reference>(value.*P)));
   }

   inline static const_node_ptr to_node_ptr(const_reference value)
   {
      return pointer_traits<const_node_ptr>::pointer_to
         (static_cast<const_node_reference>(static_cast<const_hook_reference>(value.*P)));
   }

   inline static pointer to_value_ptr(const node_ptr & n)
   {
      return pointer_traits<pointer>::pointer_to
         (*detail::parent_from_member<T, Hook>
            (static_cast<Hook*>(boost::movelib::to_raw_pointer(n)), P));
   }

   inline static const_pointer to_value_ptr(const const_node_ptr & n)
   {
      return pointer_traits<const_pointer>::pointer_to
         (*detail::parent_from_member<T, Hook>
            (static_cast<const Hook*>(boost::movelib::to_raw_pointer(n)), P));
   }
};


template<class Functor>
struct fhtraits
{
   public:
   using hook_type = typename Functor::hook_type;
   using hook_ptr = typename Functor::hook_ptr;
   using const_hook_ptr = typename Functor::const_hook_ptr;
   using node_traits = typename hook_type::hooktags::node_traits;
   using node = typename node_traits::node;
   using value_type = typename Functor::value_type;
   using node_ptr = typename node_traits::node_ptr;
   using const_node_ptr = typename node_traits::const_node_ptr;
   using pointer = typename pointer_traits<node_ptr>::
       template rebind_pointer<value_type>::type;
   using const_pointer = typename pointer_traits<node_ptr>::
       template rebind_pointer<const value_type>::type;
   using reference = value_type&;
   using const_reference = const value_type&;
   static const link_mode_type link_mode = hook_type::hooktags::link_mode;

   static node_ptr to_node_ptr(reference value)
   {  return static_cast<node*>(boost::movelib::to_raw_pointer(Functor::to_hook_ptr(value)));  }

   static const_node_ptr to_node_ptr(const_reference value)
   {  return static_cast<const node*>(boost::movelib::to_raw_pointer(Functor::to_hook_ptr(value)));  }

   static pointer to_value_ptr(const node_ptr & n)
   {  return Functor::to_value_ptr(to_hook_ptr(n));  }

   static const_pointer to_value_ptr(const const_node_ptr & n)
   {  return Functor::to_value_ptr(to_hook_ptr(n));  }

   private:
   static hook_ptr to_hook_ptr(const node_ptr & n)
   {  return hook_ptr(&*static_cast<hook_type*>(&*n));  }

   static const_hook_ptr to_hook_ptr(const const_node_ptr & n)
   {  return const_hook_ptr(&*static_cast<const hook_type*>(&*n));  }
};


} //namespace intrusive
} //namespace boost

#endif //BOOST_INTRUSIVE_DETAIL_HOOK_TRAITS_HPP
