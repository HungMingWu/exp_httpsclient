/////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga  2007-2014
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/intrusive for documentation.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef BOOST_INTRUSIVE_HASHTABLE_NODE_HPP
#define BOOST_INTRUSIVE_HASHTABLE_NODE_HPP

#ifndef BOOST_CONFIG_HPP
#  include <boost/config.hpp>
#endif

#if defined(BOOST_HAS_PRAGMA_ONCE)
#  pragma once
#endif

#include <boost/intrusive/detail/assert.hpp>
#include <boost/intrusive/pointer_traits.hpp>
#include <boost/intrusive/detail/mpl.hpp>
#include <boost/intrusive/trivial_value_traits.hpp>
#include <boost/intrusive/slist.hpp> //make_slist
#include <cstddef>
#include <climits>


namespace boost {
namespace intrusive {

template <class Slist>
struct bucket_impl : public Slist
{
   typedef Slist slist_type;
   inline bucket_impl()
   {}

   inline bucket_impl(const bucket_impl &)
   {}

   inline ~bucket_impl()
   {
      //This bucket is still being used!
      BOOST_INTRUSIVE_INVARIANT_ASSERT(Slist::empty());
   }

   inline bucket_impl &operator=(const bucket_impl&)
   {
      //This bucket is still in use!
      BOOST_INTRUSIVE_INVARIANT_ASSERT(Slist::empty());
      return *this;
   }
};

template<class Slist>
struct bucket_traits_impl
{
   public:
   /// @cond

   typedef typename pointer_traits
      <typename Slist::pointer>::template rebind_pointer
         < bucket_impl<Slist> >::type                                bucket_ptr;
   typedef Slist slist;
   /// @endcond

   inline bucket_traits_impl(bucket_ptr buckets, std::size_t len)
      :  buckets_(buckets), buckets_len_(len)
   {}

   inline bucket_traits_impl(const bucket_traits_impl &x)
      : buckets_(x.buckets_), buckets_len_(x.buckets_len_)
   {}

   inline bucket_traits_impl(bucket_traits_impl&& x)
      : buckets_(x.buckets_), buckets_len_(x.buckets_len_)
   {  x.buckets_ = bucket_ptr();   x.buckets_len_ = 0;  }

   inline bucket_traits_impl& operator=(bucket_traits_impl&& x)
   {
      buckets_ = x.buckets_; buckets_len_ = x.buckets_len_;
      x.buckets_ = bucket_ptr();   x.buckets_len_ = 0; return *this;
   }

   inline bucket_traits_impl& operator=(const bucket_traits_impl &x)
   {
      buckets_ = x.buckets_;  buckets_len_ = x.buckets_len_; return *this;
   }

   inline const bucket_ptr &bucket_begin() const
   {  return buckets_;  }

   inline std::size_t  bucket_count() const BOOST_NOEXCEPT
   {  return buckets_len_;  }

   private:
   bucket_ptr  buckets_;
   std::size_t buckets_len_;
};

template <typename T>
using type_reduced_slist_node_traits_t = typename T::reduced_slist_node_traits;

template <typename NodeTraits>
using reduced_slist_node_traits_t = detail::default_action_t<NodeTraits,
                                                             type_reduced_slist_node_traits_t,
                                                             NodeTraits>;

template <typename NodeTraits>
struct get_slist_impl
{
   using trivial_traits = trivial_value_traits<NodeTraits, link_mode_type::normal_link>;

   //Reducing symbol length
   using type = typename make_slist
       < typename NodeTraits::node
       , boost::intrusive::value_traits<trivial_traits>
       , boost::intrusive::constant_time_size<false>
       >::type;
};

template <typename NodeTraits>
using get_slist_impl_t = typename get_slist_impl<NodeTraits>::type;

template<class BucketValueTraits, bool IsConst>
class hashtable_iterator
{
   typedef typename BucketValueTraits::value_traits            value_traits;
   typedef typename BucketValueTraits::bucket_traits           bucket_traits;

   typedef iiterator< value_traits, IsConst
                    , std::forward_iterator_tag>   types_t;
   public:
   typedef typename types_t::iterator_type::difference_type    difference_type;
   typedef typename types_t::iterator_type::value_type         value_type;
   typedef typename types_t::iterator_type::pointer            pointer;
   typedef typename types_t::iterator_type::reference          reference;
   typedef typename types_t::iterator_type::iterator_category  iterator_category;

   private:
   typedef typename value_traits::node_traits                  node_traits;
   typedef typename node_traits::node_ptr                      node_ptr;
   using slist_impl = get_slist_impl_t<reduced_slist_node_traits_t<node_traits>>;
   typedef typename slist_impl::iterator                       siterator;
   typedef typename slist_impl::const_iterator                 const_siterator;
   typedef bucket_impl<slist_impl>                             bucket_type;

   typedef typename pointer_traits
      <pointer>::template rebind_pointer
         < const BucketValueTraits >::type                     const_bucketvaltraits_ptr;
   class nat;
   using nonconst_iterator = std::conditional_t<IsConst, 
                   hashtable_iterator<BucketValueTraits, false>, 
                   nat>;

   inline static node_ptr downcast_bucket(typename bucket_type::node_ptr p)
   {
      return pointer_traits<node_ptr>::
         pointer_to(static_cast<typename node_traits::node&>(*p));
   }

   public:

   inline hashtable_iterator ()
      : slist_it_()  //Value initialization to achieve "null iterators" (N3644)
   {}

   inline explicit hashtable_iterator(siterator ptr, const BucketValueTraits *cont)
      : slist_it_ (ptr)
      , traitsptr_ (cont ? pointer_traits<const_bucketvaltraits_ptr>::pointer_to(*cont) : const_bucketvaltraits_ptr() )
   {}

   inline hashtable_iterator(const hashtable_iterator &other)
      :  slist_it_(other.slist_it()), traitsptr_(other.get_bucket_value_traits())
   {}

   inline hashtable_iterator(const nonconst_iterator &other)
      :  slist_it_(other.slist_it()), traitsptr_(other.get_bucket_value_traits())
   {}

   inline const siterator &slist_it() const
   { return slist_it_; }

   inline hashtable_iterator<BucketValueTraits, false> unconst() const
   {  return hashtable_iterator<BucketValueTraits, false>(this->slist_it(), this->get_bucket_value_traits());   }

   inline hashtable_iterator& operator++()
   {  this->increment();   return *this;   }

   inline hashtable_iterator &operator=(const hashtable_iterator &other)
   {  slist_it_ = other.slist_it(); traitsptr_ = other.get_bucket_value_traits();   return *this;  }

   inline hashtable_iterator operator++(int)
   {
      hashtable_iterator result (*this);
      this->increment();
      return result;
   }

   inline friend bool operator== (const hashtable_iterator& i, const hashtable_iterator& i2)
   { return i.slist_it_ == i2.slist_it_; }

   inline friend bool operator!= (const hashtable_iterator& i, const hashtable_iterator& i2)
   { return !(i == i2); }

   inline reference operator*() const
   { return *this->operator ->(); }

   inline pointer operator->() const
   {
      return this->priv_value_traits().to_value_ptr
         (downcast_bucket(slist_it_.pointed_node()));
   }

   inline const const_bucketvaltraits_ptr &get_bucket_value_traits() const
   {  return traitsptr_;  }

   inline const value_traits &priv_value_traits() const
   {  return traitsptr_->priv_value_traits();  }

   inline const bucket_traits &priv_bucket_traits() const
   {  return traitsptr_->priv_bucket_traits();  }

   private:
   void increment()
   {
      const bucket_traits &rbuck_traits = this->priv_bucket_traits();
      bucket_type* const buckets = boost::movelib::to_raw_pointer(rbuck_traits.bucket_begin());
      const std::size_t buckets_len = rbuck_traits.bucket_count();

      ++slist_it_;
      const typename slist_impl::node_ptr n = slist_it_.pointed_node();
      const siterator first_bucket_bbegin = buckets->end();
      if(first_bucket_bbegin.pointed_node() <= n && n <= buckets[buckets_len-1].cend().pointed_node()){
         //If one-past the node is inside the bucket then look for the next non-empty bucket
         //1. get the bucket_impl from the iterator
         const bucket_type &b = static_cast<const bucket_type&>
            (bucket_type::slist_type::container_from_end_iterator(slist_it_));

         //2. Now just calculate the index b has in the bucket array
         std::size_t n_bucket = static_cast<std::size_t>(&b - buckets);

         //3. Iterate until a non-empty bucket is found
         do{
            if (++n_bucket >= buckets_len){  //bucket overflow, return end() iterator
               slist_it_ = buckets->before_begin();
               return;
            }
         }
         while (buckets[n_bucket].empty());
         slist_it_ = buckets[n_bucket].begin();
      }
      else{
         //++slist_it_ yield to a valid object
      }
   }

   siterator                  slist_it_;
   const_bucketvaltraits_ptr  traitsptr_;
};

}  //namespace intrusive {
}  //namespace boost {

#endif
