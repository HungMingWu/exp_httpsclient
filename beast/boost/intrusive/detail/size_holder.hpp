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

#ifndef BOOST_INTRUSIVE_DETAIL_SIZE_HOLDER_HPP
#define BOOST_INTRUSIVE_DETAIL_SIZE_HOLDER_HPP

#ifndef BOOST_CONFIG_HPP
#  include <boost/config.hpp>
#endif

#if defined(BOOST_HAS_PRAGMA_ONCE)
#  pragma once
#endif

namespace boost {
namespace intrusive {
namespace detail {

template <bool ConstantSize, class Tag = void>
struct size_holder
{
   inline std::size_t get_size() const
   {  return size_;  }

   inline void set_size(std::size_t size)
   {  size_ = size; }

   inline void decrement()
   {  --size_; }

   inline void increment()
   {  ++size_; }

   inline void increase(std::size_t n)
   {  size_ += n; }

   inline void decrease(std::size_t n)
   {  size_ -= n; }

   inline void swap(size_holder &other)
   {  std::size_t tmp(size_); size_ = other.size_; other.size_ = tmp; }

   std::size_t size_ = { 0 };
};

template <class Tag>
struct size_holder<false, Tag>
{
   inline std::size_t get_size() const
   {  return 0;  }

   inline void set_size(std::size_t)
   {}

   inline void decrement()
   {}

   inline void increment()
   {}

   inline void increase(std::size_t)
   {}

   inline void decrease(std::size_t)
   {}

   inline void swap(size_holder){}
};

}  //namespace detail{
}  //namespace intrusive{
}  //namespace boost{

#endif //BOOST_INTRUSIVE_DETAIL_SIZE_HOLDER_HPP
