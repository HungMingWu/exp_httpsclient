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

#ifndef BOOST_INTRUSIVE_DETAIL_DEFAULT_HEADER_HOLDER_HPP
#define BOOST_INTRUSIVE_DETAIL_DEFAULT_HEADER_HOLDER_HPP

#ifndef BOOST_CONFIG_HPP
#  include <boost/config.hpp>
#endif

#if defined(BOOST_HAS_PRAGMA_ONCE)
#  pragma once
#endif

#include <boost/intrusive/pointer_traits.hpp>
#include <boost/move/detail/to_raw_pointer.hpp>

namespace boost {
namespace intrusive {
namespace detail {

// trivial header node holder
template <typename NodeTraits>
struct default_header_holder : public NodeTraits::node
{
   using node_traits = NodeTraits;
   using node = typename node_traits::node;
   using node_ptr = typename node_traits::node_ptr;
   using const_node_ptr = typename node_traits::const_node_ptr;

   using node::node;

   inline const_node_ptr get_node() const
   {
	   return pointer_traits<const_node_ptr>::pointer_to(static_cast<const node&>(*this));
   }

   inline node_ptr get_node()
   {
	   return pointer_traits<node_ptr>::pointer_to(static_cast<node&>(*this));
   }

   // (unsafe) downcast used to implement container-from-iterator
   inline static default_header_holder* get_holder(const node_ptr &p)
   {
	   return static_cast<default_header_holder*>(boost::movelib::to_raw_pointer(p));
   }
};

// type function producing the header node holder
template <typename ValueTraits, typename HeaderHolder>
struct get_header_holder_type
{
	using type = HeaderHolder;
};

template <typename ValueTraits>
struct get_header_holder_type<ValueTraits, void>
{
	using type = default_header_holder<typename ValueTraits::node_traits>;
};

template <typename ValueTraits, typename HeaderHolder>
using get_header_holder_type_t = typename get_header_holder_type<ValueTraits, HeaderHolder>::type;

} //namespace detail
} //namespace intrusive
} //namespace boost

#endif //BOOST_INTRUSIVE_DETAIL_DEFAULT_HEADER_HOLDER_HPP
