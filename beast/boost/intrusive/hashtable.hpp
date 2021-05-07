/////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga  2006-2015
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/intrusive for documentation.
//
/////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_INTRUSIVE_HASHTABLE_HPP
#define BOOST_INTRUSIVE_HASHTABLE_HPP

#include <iostream>
#include <boost/intrusive/intrusive_fwd.hpp>

//General intrusive utilities
#include <boost/intrusive/detail/hashtable_node.hpp>
#include <boost/intrusive/detail/transform_iterator.hpp>
#include <boost/intrusive/link_mode.hpp>
#include <boost/intrusive/detail/ebo_functor_holder.hpp>
#include <boost/intrusive/detail/is_stateful_value_traits.hpp>
#include <boost/intrusive/detail/node_to_value.hpp>
#include <boost/intrusive/detail/exception_disposer.hpp>
#include <boost/intrusive/detail/node_cloner_disposer.hpp>
#include <boost/intrusive/detail/simple_disposers.hpp>
#include <boost/intrusive/detail/size_holder.hpp>
#include <boost/intrusive/detail/iterator.hpp>

//Implementation utilities
#include <boost/intrusive/unordered_set_hook.hpp>
#include <boost/intrusive/slist.hpp>
#include <boost/intrusive/pointer_traits.hpp>
#include <boost/intrusive/detail/mpl.hpp>

//boost
#include <boost/intrusive/detail/assert.hpp>

//std C++
#include <algorithm>    //std::lower_bound, std::upper_bound
#include <cstddef>      //std::size_t

#if defined(BOOST_HAS_PRAGMA_ONCE)
#  pragma once
#endif

namespace boost::intrusive {

/// @cond

template<int Dummy = 0>
struct prime_list_holder
{
   private:

   static inline std::size_t suggested_lower_bucket_count_dispatch(std::size_t n)
   {
      return suggested_lower_bucket_count_impl(n);
   }

   static const std::size_t prime_list[];
   static const std::size_t prime_list_size;

   static std::size_t suggested_lower_bucket_count_impl(std::size_t n)
   {
      const std::size_t *primes     = &prime_list_holder<0>::prime_list[0];
      const std::size_t *primes_end = primes + prime_list_holder<0>::prime_list_size;
      std::size_t const* bound = std::lower_bound(primes, primes_end, n);
      //Tables have upper SIZE_MAX, so we must always found an entry
      BOOST_INTRUSIVE_INVARIANT_ASSERT(bound != primes_end);
      bound -= std::size_t(bound != primes);
      return *bound;
   }

   static std::size_t suggested_upper_bucket_count_impl(std::size_t n)
   {
      const std::size_t *primes     = &prime_list_holder<0>::prime_list[0];
      const std::size_t *primes_end = primes + prime_list_holder<0>::prime_list_size;
      std::size_t const* bound = std::upper_bound(primes, primes_end, n);
      bound -= std::size_t(bound == primes_end);
      return *bound;
   }

   public:

   static inline std::size_t suggested_upper_bucket_count(std::size_t n)
   {
      return suggested_upper_bucket_count_impl(n);
   }

   static inline std::size_t suggested_lower_bucket_count(std::size_t n)
   {
      return suggested_lower_bucket_count_dispatch(n);
   }
};

#if !defined(BOOST_INTRUSIVE_DOXYGEN_INVOKED)

//We only support LLP64(Win64) or LP64(most Unix) data models
#ifdef _WIN64  //In 64 bit windows sizeof(size_t) == sizeof(unsigned long long)
   #define BOOST_INTRUSIVE_PRIME_C(NUMBER) NUMBER##ULL
   #define BOOST_INTRUSIVE_64_BIT_SIZE_T 1
#else //In 32 bit windows and 32/64 bit unixes sizeof(size_t) == sizeof(unsigned long)
   #define BOOST_INTRUSIVE_PRIME_C(NUMBER) NUMBER##UL
   #define BOOST_INTRUSIVE_64_BIT_SIZE_T (((((ULONG_MAX>>16)>>16)>>16)>>15) != 0)
#endif

template<int Dummy>
const std::size_t prime_list_holder<Dummy>::prime_list[] = {
   BOOST_INTRUSIVE_PRIME_C(3),                     BOOST_INTRUSIVE_PRIME_C(7),
   BOOST_INTRUSIVE_PRIME_C(11),                    BOOST_INTRUSIVE_PRIME_C(17),
   BOOST_INTRUSIVE_PRIME_C(29),                    BOOST_INTRUSIVE_PRIME_C(53),
   BOOST_INTRUSIVE_PRIME_C(97),                    BOOST_INTRUSIVE_PRIME_C(193),
   BOOST_INTRUSIVE_PRIME_C(389),                   BOOST_INTRUSIVE_PRIME_C(769),
   BOOST_INTRUSIVE_PRIME_C(1543),                  BOOST_INTRUSIVE_PRIME_C(3079),
   BOOST_INTRUSIVE_PRIME_C(6151),                  BOOST_INTRUSIVE_PRIME_C(12289),
   BOOST_INTRUSIVE_PRIME_C(24593),                 BOOST_INTRUSIVE_PRIME_C(49157),
   BOOST_INTRUSIVE_PRIME_C(98317),                 BOOST_INTRUSIVE_PRIME_C(196613),
   BOOST_INTRUSIVE_PRIME_C(393241),                BOOST_INTRUSIVE_PRIME_C(786433),
   BOOST_INTRUSIVE_PRIME_C(1572869),               BOOST_INTRUSIVE_PRIME_C(3145739),
   BOOST_INTRUSIVE_PRIME_C(6291469),               BOOST_INTRUSIVE_PRIME_C(12582917),
   BOOST_INTRUSIVE_PRIME_C(25165843),              BOOST_INTRUSIVE_PRIME_C(50331653),
   BOOST_INTRUSIVE_PRIME_C(100663319),             BOOST_INTRUSIVE_PRIME_C(201326611),
   BOOST_INTRUSIVE_PRIME_C(402653189),             BOOST_INTRUSIVE_PRIME_C(805306457),
   BOOST_INTRUSIVE_PRIME_C(1610612741),            BOOST_INTRUSIVE_PRIME_C(3221225473),
#if BOOST_INTRUSIVE_64_BIT_SIZE_T
   //Taken from Boost.MultiIndex code, thanks to Joaquin M Lopez Munoz.
   BOOST_INTRUSIVE_PRIME_C(6442450939),            BOOST_INTRUSIVE_PRIME_C(12884901893),
   BOOST_INTRUSIVE_PRIME_C(25769803751),           BOOST_INTRUSIVE_PRIME_C(51539607551),
   BOOST_INTRUSIVE_PRIME_C(103079215111),          BOOST_INTRUSIVE_PRIME_C(206158430209),
   BOOST_INTRUSIVE_PRIME_C(412316860441),          BOOST_INTRUSIVE_PRIME_C(824633720831),
   BOOST_INTRUSIVE_PRIME_C(1649267441651),         BOOST_INTRUSIVE_PRIME_C(3298534883309),
   BOOST_INTRUSIVE_PRIME_C(6597069766657),         BOOST_INTRUSIVE_PRIME_C(13194139533299),
   BOOST_INTRUSIVE_PRIME_C(26388279066623),        BOOST_INTRUSIVE_PRIME_C(52776558133303),
   BOOST_INTRUSIVE_PRIME_C(105553116266489),       BOOST_INTRUSIVE_PRIME_C(211106232532969),
   BOOST_INTRUSIVE_PRIME_C(422212465066001),       BOOST_INTRUSIVE_PRIME_C(844424930131963),
   BOOST_INTRUSIVE_PRIME_C(1688849860263953),      BOOST_INTRUSIVE_PRIME_C(3377699720527861),
   BOOST_INTRUSIVE_PRIME_C(6755399441055731),      BOOST_INTRUSIVE_PRIME_C(13510798882111483),
   BOOST_INTRUSIVE_PRIME_C(27021597764222939),     BOOST_INTRUSIVE_PRIME_C(54043195528445957),
   BOOST_INTRUSIVE_PRIME_C(108086391056891903),    BOOST_INTRUSIVE_PRIME_C(216172782113783843),
   BOOST_INTRUSIVE_PRIME_C(432345564227567621),    BOOST_INTRUSIVE_PRIME_C(864691128455135207),
   BOOST_INTRUSIVE_PRIME_C(1729382256910270481),   BOOST_INTRUSIVE_PRIME_C(3458764513820540933),
   BOOST_INTRUSIVE_PRIME_C(6917529027641081903),   BOOST_INTRUSIVE_PRIME_C(13835058055282163729),
   BOOST_INTRUSIVE_PRIME_C(18446744073709551557),  BOOST_INTRUSIVE_PRIME_C(18446744073709551615)   //Upper limit, just in case
#else
   BOOST_INTRUSIVE_PRIME_C(4294967291),            BOOST_INTRUSIVE_PRIME_C(4294967295)             //Upper limit, just in case
#endif
   };

#undef BOOST_INTRUSIVE_PRIME_C
#undef BOOST_INTRUSIVE_64_BIT_SIZE_T

#endif   //#if !defined(BOOST_INTRUSIVE_DOXYGEN_INVOKED)

template<int Dummy>
const std::size_t prime_list_holder<Dummy>::prime_list_size
   = sizeof(prime_list)/sizeof(std::size_t);

struct hash_bool_flags
{
   static const std::size_t unique_keys_pos        = 1u;
   static const std::size_t constant_time_size_pos = 2u;
   static const std::size_t power_2_buckets_pos    = 4u;
   static const std::size_t cache_begin_pos        = 8u;
   static const std::size_t compare_hash_pos       = 16u;
   static const std::size_t incremental_pos        = 32u;
};

namespace detail {

template <typename SupposedValueTraits>
struct get_slist_impl_from_supposed_value_traits
{
   using value_traits = SupposedValueTraits;
   using node_traits = typename detail::get_node_traits<value_traits>::type;
   using type = get_slist_impl_t<reduced_slist_node_traits_t<node_traits>>;
};

template <typename SupposedValueTraits>
using get_slist_impl_from_supposed_value_traits_t =
    typename get_slist_impl_from_supposed_value_traits<SupposedValueTraits>::type;

template <typename SupposedValueTraits>
struct unordered_bucket_impl
{
   using slist_impl = get_slist_impl_from_supposed_value_traits_t<SupposedValueTraits>;
   using type = bucket_impl<slist_impl>;
};

template <typename SupposedValueTraits>
using unordered_bucket_impl_t = typename unordered_bucket_impl<SupposedValueTraits>::type;

template<class SupposedValueTraits>
struct unordered_bucket_ptr_impl
{
   using node_ptr = typename detail::get_node_traits<SupposedValueTraits>::type::node_ptr;
   using bucket_type = unordered_bucket_impl_t<SupposedValueTraits>;
   using type = typename pointer_traits<node_ptr>::template rebind<bucket_type>;
};

template <typename T>
concept have_store_hash_v = T::store_hash == true;

template <typename T>
concept have_optimize_multikey_v = T::optimize_multikey == true;

struct insert_commit_data_impl
{
   std::size_t hash;
};

template<class Node, class SlistNodePtr>
inline typename pointer_traits<SlistNodePtr>::template rebind_pointer<Node>::type
   dcast_bucket_ptr(const SlistNodePtr &p)
{
   typedef typename pointer_traits<SlistNodePtr>::template rebind_pointer<Node>::type node_ptr;
   return pointer_traits<node_ptr>::pointer_to(static_cast<Node&>(*p));
}

template<class NodeTraits>
struct group_functions
{
   //           A group is reverse-linked
   //
   //          A is "first in group"
   //          C is "last  in group"
   //           __________________
   //          |  _____   _____   |
   //          | |     | |      | |  <- Group links
   //          ^ V     ^ V      ^ V
   //           _       _        _      _
   //         A|_|    B|_|     C|_|   D|_|
   //
   //          ^ |     ^ |      ^ |    ^ V  <- Bucket links
   //   _ _____| |_____| |______| |____| |
   //  |B|                               |
   //   ^________________________________|
   //

   using node_traits = NodeTraits;
   using group_traits = unordered_group_adapter<node_traits>;
   using node_ptr = typename node_traits::node_ptr;
   using node = typename node_traits::node;
   using reduced_node_traits = reduced_slist_node_traits_t<node_traits>;
   using slist_node_ptr = typename reduced_node_traits::node_ptr;
   using slist_node = typename reduced_node_traits::node;
   using group_algorithms = circular_slist_algorithms<group_traits>;
   using node_algorithms = circular_slist_algorithms<node_traits>;

   static slist_node_ptr get_bucket_before_begin
      (slist_node_ptr bucket_beg, slist_node_ptr bucket_end, node_ptr p)
   {
      //First find the last node of p's group.
      //This requires checking the first node of the next group or
      //the bucket node.
      node_ptr prev_node = p;
      node_ptr nxt(node_traits::get_next(p));
      while(!(bucket_beg <= nxt && nxt <= bucket_end) &&
             (group_traits::get_next(nxt) == prev_node)){
         prev_node = nxt;
         nxt = node_traits::get_next(nxt);
      }

      //If we've reached the bucket node just return it.
      if(bucket_beg <= nxt && nxt <= bucket_end){
         return nxt;
      }

      //Otherwise, iterate using group links until the bucket node
      node_ptr first_node_of_group  = nxt;
      node_ptr last_node_group      = group_traits::get_next(first_node_of_group);
      slist_node_ptr possible_end   = node_traits::get_next(last_node_group);

      while(!(bucket_beg <= possible_end && possible_end <= bucket_end)){
         first_node_of_group = detail::dcast_bucket_ptr<node>(possible_end);
         last_node_group   = group_traits::get_next(first_node_of_group);
         possible_end      = node_traits::get_next(last_node_group);
      }
      return possible_end;
   }

   static node_ptr get_prev_to_first_in_group(slist_node_ptr bucket_node, node_ptr first_in_group)
   {
      node_ptr nb = detail::dcast_bucket_ptr<node>(bucket_node);
      node_ptr n;
      while((n = node_traits::get_next(nb)) != first_in_group){
         nb = group_traits::get_next(n);  //go to last in group
      }
      return nb;
   }

   static void erase_from_group(slist_node_ptr end_ptr, node_ptr to_erase_ptr)
   {
      node_ptr const nxt_ptr(node_traits::get_next(to_erase_ptr));
      //Check if the next node is in the group (not end node) and reverse linked to
      //'to_erase_ptr'. Erase if that's the case.
      if(nxt_ptr != end_ptr && to_erase_ptr == group_traits::get_next(nxt_ptr)){
         group_algorithms::unlink_after(nxt_ptr);
      }
   }

   template <bool optimize_multikey>
   inline static node_ptr get_last_in_group(node_ptr first_in_group)
   {
       if constexpr (optimize_multikey) {
           return group_traits::get_next(first_in_group);
       }
       else {
           return first_in_group;
       }
   }

   static node_ptr get_first_in_group(node_ptr n)
   {
       node_ptr ng;
       while (n == node_traits::get_next((ng = group_traits::get_next(n)))) {
           n = ng;
       }
       return n;
   }

   inline static node_ptr next_group_if_first_in_group(node_ptr ptr)
   {
      return node_traits::get_next(group_traits::get_next(ptr));
   }

   inline static void insert_in_group(node_ptr first_in_group, node_ptr n)
   {
       group_algorithms::link_after(first_in_group, n);  
   }

   inline static node_ptr split_group(node_ptr const new_first_in_group)
   {
      node_ptr const first(get_first_in_group(new_first_in_group));
      if(first != new_first_in_group){
         node_ptr const last = group_traits::get_next(first);
         group_traits::set_next(first, group_traits::get_next(new_first_in_group));
         group_traits::set_next(new_first_in_group, last);
      }
      return first;
   }
};

template<class BucketType, class SplitTraits>
class incremental_rehash_rollback
{
   private:
   typedef BucketType   bucket_type;
   typedef SplitTraits  split_traits;

   incremental_rehash_rollback();
   incremental_rehash_rollback & operator=(const incremental_rehash_rollback &);
   incremental_rehash_rollback (const incremental_rehash_rollback &);

   public:
   incremental_rehash_rollback
      (bucket_type &source_bucket, bucket_type &destiny_bucket, split_traits &split_traits)
      :  source_bucket_(source_bucket),  destiny_bucket_(destiny_bucket)
      ,  split_traits_(split_traits),  released_(false)
   {}

   inline void release()
   {  released_ = true; }

   ~incremental_rehash_rollback()
   {
      if(!released_){
         //If an exception is thrown, just put all moved nodes back in the old bucket
         //and move back the split mark.
         destiny_bucket_.splice_after(destiny_bucket_.before_begin(), source_bucket_);
         split_traits_.decrement();
      }
   }

   private:
   bucket_type &source_bucket_;
   bucket_type &destiny_bucket_;
   split_traits &split_traits_;
   bool released_;
};

template<class NodeTraits>
struct node_functions
{
   inline static void store_hash(typename NodeTraits::node_ptr p, std::size_t h)
   {  return NodeTraits::set_hash(p, h); }
};

inline std::size_t hash_to_bucket(std::size_t hash_value, std::size_t bucket_cnt, std::false_type)
{  return hash_value % bucket_cnt;  }

inline std::size_t hash_to_bucket(std::size_t hash_value, std::size_t bucket_cnt, std::true_type)
{  return hash_value & (bucket_cnt - 1);   }

template<bool Power2Buckets, bool Incremental>
inline std::size_t hash_to_bucket_split(std::size_t hash_value, std::size_t bucket_cnt, std::size_t split)
{
   std::size_t bucket_number = detail::hash_to_bucket(hash_value, bucket_cnt, std::bool_constant<Power2Buckets>());
   if constexpr (Incremental)
      bucket_number -= static_cast<std::size_t>(bucket_number >= split)*(bucket_cnt/2);
   return bucket_number;
}

}  //namespace detail {

//!This metafunction will obtain the type of a bucket
//!from the value_traits or hook option to be used with
//!a hash container.
template<class ValueTraitsOrHookOption>
struct unordered_bucket
   : public detail::unordered_bucket_impl
      <typename ValueTraitsOrHookOption::
         template pack<empty>::proto_value_traits
      >
{};

//!This metafunction will obtain the type of a bucket pointer
//!from the value_traits or hook option to be used with
//!a hash container.
template<class ValueTraitsOrHookOption>
struct unordered_bucket_ptr
   : public detail::unordered_bucket_ptr_impl
      <typename ValueTraitsOrHookOption::
         template pack<empty>::proto_value_traits
      >
{};

//!This metafunction will obtain the type of the default bucket traits
//!(when the user does not specify the bucket_traits<> option) from the
//!value_traits or hook option to be used with
//!a hash container.
template<class ValueTraitsOrHookOption>
struct unordered_default_bucket_traits
{
   typedef typename ValueTraitsOrHookOption::
      template pack<empty>::proto_value_traits   supposed_value_traits;
   using slist_impl = detail::get_slist_impl_from_supposed_value_traits_t<supposed_value_traits>;
   typedef bucket_traits_impl
      <slist_impl>                              implementation_defined;
   typedef implementation_defined               type;
};

struct default_bucket_traits;

//hashtable default hook traits
struct default_hashtable_hook_applier
{  template <class T> struct apply{ typedef typename T::default_hashtable_hook type;  };  };

template<>
struct is_default_hook_tag<default_hashtable_hook_applier>
{  static const bool value = true;  };

struct hashtable_defaults
{
   typedef default_hashtable_hook_applier   proto_value_traits;
   typedef void                        key_of_value;
   typedef void                        equal;
   typedef void                        hash;
   typedef default_bucket_traits       bucket_traits;
   static const bool constant_time_size   = true;
   static const bool power_2_buckets      = false;
   static const bool cache_begin          = false;
   static const bool compare_hash         = false;
   static const bool incremental          = false;
};

template<class ValueTraits, bool IsConst>
struct downcast_node_to_value_t
   :  public detail::node_to_value<ValueTraits, IsConst>
{
   typedef detail::node_to_value<ValueTraits, IsConst>  base_t;
   typedef typename base_t::result_type                 result_type;
   typedef ValueTraits                                  value_traits;
   using slist_impl = get_slist_impl_t<reduced_slist_node_traits_t<typename value_traits::node_traits>>;
   using first_argument_type = std::conditional_t<IsConst,
       std::add_const_t<typename slist_impl::node>, typename slist_impl::node> &;
   using intermediate_argument_type = std::conditional_t<IsConst,
       std::add_const_t<typename ValueTraits::node_traits::node>, typename ValueTraits::node_traits::node>&;
   typedef typename pointer_traits
      <typename ValueTraits::pointer>::
         template rebind_pointer
            <const ValueTraits>::type                   const_value_traits_ptr;

   using base_t::base_t;

   inline result_type operator()(first_argument_type arg) const
   {  return this->base_t::operator()(static_cast<intermediate_argument_type>(arg)); }
};

template<class F, class SlistNodePtr, class NodePtr>
struct node_cast_adaptor
{
   typedef typename pointer_traits<SlistNodePtr>::element_type slist_node;
   typedef typename pointer_traits<NodePtr>::element_type      node;
   [[no_unique_address]] F                      functor;

   template<class ConvertibleToF, class RealValuTraits>
   inline node_cast_adaptor(const ConvertibleToF &c2f, const RealValuTraits *traits)
      : functor(c2f, traits)
   {}

   inline auto operator()(const slist_node &to_clone)
   {  return functor(static_cast<const node &>(to_clone));   }

   inline void operator()(SlistNodePtr to_clone)
   {
       functor(pointer_traits<NodePtr>::pointer_to(static_cast<node &>(*to_clone)));
   }
};

//bucket_plus_vtraits stores ValueTraits + BucketTraits
//this data is needed by iterators to obtain the
//value from the iterator and detect the bucket
template<class ValueTraits, class BucketTraits>
struct bucket_plus_vtraits
{
   using bucket_traits = BucketTraits;
   using value_traits = ValueTraits;

   static constexpr bool safemode_or_autounlink = is_safe_autounlink(value_traits::link_mode);

   using slist_impl = detail::get_slist_impl_from_supposed_value_traits_t<value_traits>;
   using node_traits = typename value_traits::node_traits;
   using group_traits = unordered_group_adapter<node_traits>;
   using siterator = typename slist_impl::iterator;
   using bucket_type = bucket_impl<slist_impl>;
   using group_functions_t = detail::group_functions<node_traits>;
   using node_algorithms = typename slist_impl::node_algorithms;
   using slist_node_ptr = typename slist_impl::node_ptr;
   using node_ptr = typename node_traits::node_ptr;
   using node = typename node_traits::node;
   using value_type = typename value_traits::value_type;
   using pointer = typename value_traits::pointer;
   using const_pointer = typename value_traits::const_pointer;
   using reference = typename pointer_traits<pointer>::reference;
   using const_reference = typename pointer_traits<const_pointer>::reference;
   using group_algorithms = circular_slist_algorithms<group_traits>;
   using const_value_traits_ptr = typename pointer_traits<typename value_traits::pointer>::
       template rebind<const value_traits>;
   using const_bucket_value_traits_ptr = typename pointer_traits<typename value_traits::pointer>::
       template rebind<const bucket_plus_vtraits>;
   using bucket_ptr = typename detail::unordered_bucket_ptr_impl<value_traits>::type;

   template<class BucketTraitsType>
   inline bucket_plus_vtraits(const ValueTraits &val_traits, BucketTraitsType&& b_traits)
      :  data(val_traits, std::forward<BucketTraitsType>(b_traits))
   {}

   inline bucket_plus_vtraits & operator =(const bucket_plus_vtraits &x)
   {  data.bucket_traits_ = x.data.bucket_traits_;  return *this;  }

   inline const_value_traits_ptr priv_value_traits_ptr() const
   {  return pointer_traits<const_value_traits_ptr>::pointer_to(this->priv_value_traits());  }

   //bucket_value_traits
   //
   inline const bucket_plus_vtraits &get_bucket_value_traits() const
   {  return *this;  }

   inline bucket_plus_vtraits &get_bucket_value_traits()
   {  return *this;  }

   inline const_bucket_value_traits_ptr bucket_value_traits_ptr() const
   {  return pointer_traits<const_bucket_value_traits_ptr>::pointer_to(this->get_bucket_value_traits());  }

   //value traits
   //
   inline const value_traits &priv_value_traits() const
   {  return this->data;  }

   inline value_traits &priv_value_traits()
   {  return this->data;  }

   //bucket_traits
   //
   inline const bucket_traits &priv_bucket_traits() const
   {  return this->data.bucket_traits_;  }

   inline bucket_traits &priv_bucket_traits()
   {  return this->data.bucket_traits_;  }

   //bucket operations
   inline bucket_ptr priv_bucket_pointer() const noexcept
   {  return this->priv_bucket_traits().bucket_begin();  }

   std::size_t priv_bucket_count() const noexcept
   {  return this->priv_bucket_traits().bucket_count();  }

   inline bucket_ptr priv_invalid_bucket() const
   {
      const bucket_traits &rbt = this->priv_bucket_traits();
      return rbt.bucket_begin() + rbt.bucket_count();
   }

   inline siterator priv_invalid_local_it() const
   {  return this->priv_bucket_traits().bucket_begin()->before_begin();  }

   template <bool optimize_multikey, typename NodeDisposer>
   static std::size_t priv_erase_from_single_bucket(bucket_type &b, siterator sbefore_first, siterator slast, NodeDisposer node_disposer)
   {
       std::size_t n = 0;
       if constexpr (optimize_multikey) {
           siterator const sfirst(++siterator(sbefore_first));
           if (sfirst != slast) {
               node_ptr const nf = detail::dcast_bucket_ptr<node>(sfirst.pointed_node());
               node_ptr const nl = detail::dcast_bucket_ptr<node>(slast.pointed_node());
               node_ptr const ne = detail::dcast_bucket_ptr<node>(b.end().pointed_node());

               if (group_functions_t::next_group_if_first_in_group(nf) != nf) {
                   // The node is at the beginning of a group.
                   if (nl != ne) {
                       group_functions_t::split_group(nl);
                   }
               }
               else {
                   node_ptr const group1 = group_functions_t::split_group(nf);
                   if (nl != ne) {
                       node_ptr const group2 = group_functions_t::split_group(ne);
                       if (nf == group2) {   //Both first and last in the same group
                                            //so join group1 and group2
                           node_ptr const end1 = group_traits::get_next(group1);
                           node_ptr const end2 = group_traits::get_next(group2);
                           group_traits::set_next(group1, end2);
                           group_traits::set_next(group2, end1);
                       }
                   }
               }

               siterator it(++siterator(sbefore_first));
               while (it != slast) {
                   node_disposer((it++).pointed_node());
                   ++n;
               }
               b.erase_after(sbefore_first, slast);
           }
       }
       else {
           siterator it(++siterator(sbefore_first));
           while (it != slast) {
               node_disposer((it++).pointed_node());
               ++n;
           }
           b.erase_after(sbefore_first, slast);
       }
       return n;
   }

   template <bool optimize_multikey, typename NodeDisposer>
   static void priv_erase_node(bucket_type &b, siterator i, NodeDisposer node_disposer)
   {
       if constexpr (optimize_multikey) {
           node_ptr const ne(detail::dcast_bucket_ptr<node>(b.end().pointed_node()));
           node_ptr n(detail::dcast_bucket_ptr<node>(i.pointed_node()));
           node_ptr pos = node_traits::get_next(group_traits::get_next(n));
           node_ptr bn;
           node_ptr nn(node_traits::get_next(n));

           if (pos != n) {
               //Node is the first of the group
               bn = group_functions_t::get_prev_to_first_in_group(ne, n);

               //Unlink the rest of the group if it's not the last node of its group
               if (nn != ne && group_traits::get_next(nn) == n) {
                   group_algorithms::unlink_after(nn);
               }
           }
           else if (nn != ne && group_traits::get_next(nn) == n) {
               //Node is not the end of the group
               bn = group_traits::get_next(n);
               group_algorithms::unlink_after(nn);
           }
           else {
               //Node is the end of the group
               bn = group_traits::get_next(n);
               node_ptr const x(group_algorithms::get_previous_node(n));
               group_algorithms::unlink_after(x);
           }
           b.erase_after_and_dispose(bucket_type::s_iterator_to(*bn), node_disposer);
       }
       else {
           b.erase_after_and_dispose(b.previous(i), node_disposer);
       }
   }

   template <bool optimize_multikey, typename NodeDisposer>
   std::size_t priv_erase_node_range( siterator const &before_first_it,  std::size_t const first_bucket
                        , siterator const &last_it,          std::size_t const last_bucket
                        , NodeDisposer node_disposer)
   {
      std::size_t num_erased(0);
      siterator last_step_before_it;
      if(first_bucket != last_bucket){
         bucket_type *b = (&this->priv_bucket_pointer()[0]);
         num_erased += this->priv_erase_from_single_bucket<optimize_multikey>
            (b[first_bucket], before_first_it, b[first_bucket].end(), node_disposer);
         for(std::size_t i = 0, n = (last_bucket - first_bucket - 1); i != n; ++i){
            num_erased += this->priv_erase_whole_bucket(b[first_bucket+i+1], node_disposer);
         }
         last_step_before_it = b[last_bucket].before_begin();
      }
      else{
         last_step_before_it = before_first_it;
      }
      num_erased += this->template priv_erase_from_single_bucket<optimize_multikey>
                  (this->priv_bucket_pointer()[last_bucket], last_step_before_it, last_it, node_disposer);
      return num_erased;
   }

   template <bool optimize_multikey>
   static siterator priv_get_last(bucket_type &b)
   {
       if constexpr (optimize_multikey) {
           //First find the last node of p's group.
           //This requires checking the first node of the next group or
           //the bucket node.
           slist_node_ptr end_ptr(b.end().pointed_node());
           node_ptr possible_end(node_traits::get_next(detail::dcast_bucket_ptr<node>(end_ptr)));
           node_ptr last_node_group(possible_end);

           while (end_ptr != possible_end) {
               last_node_group = group_traits::get_next(detail::dcast_bucket_ptr<node>(possible_end));
               possible_end = node_traits::get_next(last_node_group);
           }
           return bucket_type::s_iterator_to(*last_node_group);
       }
       else {
           return b.previous(b.end());
       }
   }

   template<class NodeDisposer>
   std::size_t priv_erase_whole_bucket(bucket_type &b, NodeDisposer node_disposer)
   {
      std::size_t num_erased = 0;
      siterator b_begin(b.before_begin());
      siterator nxt(b_begin);
      ++nxt;
      siterator const end_sit(b.end());
      while(nxt != end_sit){
         //No need to init group links as we'll delete all bucket nodes
         nxt = bucket_type::s_erase_after_and_dispose(b_begin, node_disposer);
         ++num_erased;
      }
      return num_erased;
   }

   template <bool optimize_multikey>
   static siterator priv_get_previous(bucket_type &b, siterator i)
   {
       if constexpr (optimize_multikey) {
           node_ptr const elem(detail::dcast_bucket_ptr<node>(i.pointed_node()));
           node_ptr const prev_in_group(group_traits::get_next(elem));
           bool const first_in_group = node_traits::get_next(prev_in_group) != elem;
           typename bucket_type::node& n = first_in_group
               ? *group_functions_t::get_prev_to_first_in_group(b.end().pointed_node(), elem)
               : *group_traits::get_next(elem)
               ;
           return bucket_type::s_iterator_to(n);
       }
       else {
           return b.previous(i);
       }
   }

   template <bool optimize_multikey>
   std::size_t priv_get_bucket_num_no_hash_store(siterator it)
   {
       if constexpr (optimize_multikey) {
           const bucket_ptr f(this->priv_bucket_pointer()), l(f + this->priv_bucket_count() - 1);
           slist_node_ptr bb = group_functions_t::get_bucket_before_begin
           (f->end().pointed_node()
               , l->end().pointed_node()
               , detail::dcast_bucket_ptr<node>(it.pointed_node()));
           //Now get the bucket_impl from the iterator
           const bucket_type& b = static_cast<const bucket_type&>
               (bucket_type::slist_type::container_from_end_iterator(bucket_type::s_iterator_to(*bb)));
           //Now just calculate the index b has in the bucket array
           return static_cast<std::size_t>(&b - &*f);
       }
       else {
           bucket_ptr f(this->priv_bucket_pointer()), l(f + this->priv_bucket_count() - 1);
           slist_node_ptr first_ptr(f->cend().pointed_node())
               , last_ptr(l->cend().pointed_node());

           //The end node is embedded in the singly linked list:
           //iterate until we reach it.
           while (!(std::less_equal<slist_node_ptr>()(first_ptr, it.pointed_node()) &&
               std::less_equal<slist_node_ptr>()(it.pointed_node(), last_ptr))) {
               ++it;
           }
           //Now get the bucket_impl from the iterator
           const bucket_type& b = static_cast<const bucket_type&>
               (bucket_type::container_from_end_iterator(it));

           //Now just calculate the index b has in the bucket array
           return static_cast<std::size_t>(&b - &*f);
       }
   }

   template <bool store_hash>
   inline static std::size_t priv_stored_hash(slist_node_ptr n)
   {
       if constexpr (store_hash) {
           return node_traits::get_hash(detail::dcast_bucket_ptr<node>(n));
       }
       else {
           return std::size_t(-1);
       }
   }

   inline node &priv_value_to_node(reference v)
   {  return *this->priv_value_traits().to_node_ptr(v);  }

   inline const node &priv_value_to_node(const_reference v) const
   {  return *this->priv_value_traits().to_node_ptr(v);  }

   inline reference priv_value_from_slist_node(slist_node_ptr n)
   {  return *this->priv_value_traits().to_value_ptr(detail::dcast_bucket_ptr<node>(n)); }

   inline const_reference priv_value_from_slist_node(slist_node_ptr n) const
   {  return *this->priv_value_traits().to_value_ptr(detail::dcast_bucket_ptr<node>(n)); }

   void priv_clear_buckets(const bucket_ptr buckets_ptr, const std::size_t bucket_cnt)
   {
      bucket_ptr buckets_it = buckets_ptr;
      for(std::size_t bucket_i = 0; bucket_i != bucket_cnt; ++buckets_it, ++bucket_i){
         if constexpr (safemode_or_autounlink) {
            buckets_it->clear_and_dispose(detail::init_disposer<node_algorithms>());
         }
         else{
            buckets_it->clear();
         }
      }
   }

   inline std::size_t priv_stored_or_compute_hash(const value_type &v, std::true_type) const   //For store_hash == true
   {  return node_traits::get_hash(this->priv_value_traits().to_node_ptr(v));  }

   typedef hashtable_iterator<bucket_plus_vtraits, false>          iterator;
   typedef hashtable_iterator<bucket_plus_vtraits, true>           const_iterator;

   inline iterator end() noexcept
   {  return iterator(this->priv_invalid_local_it(), 0);   }

   inline const_iterator end() const noexcept
   {  return this->cend(); }

   inline const_iterator cend() const noexcept
   {  return const_iterator(this->priv_invalid_local_it(), 0);  }

   //Public functions:
   struct data_type : public ValueTraits
   {
      template<class BucketTraitsType>
      inline data_type(const ValueTraits &val_traits, BucketTraitsType&& b_traits)
         : ValueTraits(val_traits), bucket_traits_(std::forward<BucketTraitsType>(b_traits))
      {}

      bucket_traits bucket_traits_;
   } data;
};

template<class Hash, class>
struct get_hash
{
   using type = Hash;
};

template<class T>
struct get_hash<void, T>
{
   using type = std::hash<T>;
};

template <typename ...Ts>
using get_hash_t = typename get_hash<Ts...>::type;

template<class EqualTo, class>
struct get_equal_to
{
   using type = EqualTo;
};

template<class T>
struct get_equal_to<void, T>
{
   using type = std::equal_to<T>;
};

template <typename ...Ts>
using get_equal_to_t = typename get_equal_to<Ts...>::type;

template <typename KeyOfValue, typename T>
struct get_hash_key_of_value
{
   using type = KeyOfValue;
};

template <typename T>
struct get_hash_key_of_value<void, T>
{
   using type = ::boost::intrusive::detail::identity<T>;
};

template <typename KeyOfValue, typename T>
using get_hash_key_of_value_t = typename get_hash_key_of_value<KeyOfValue, T>::type;

template<class T, class VoidOrKeyOfValue>
struct hash_key_types_base
{
   using key_of_value = get_hash_key_of_value_t<VoidOrKeyOfValue, T>;
   using key_type = typename key_of_value::type;
};

template <typename T, typename VoidOrKeyOfValue>
using hash_key_types_t = typename hash_key_types_base<T, VoidOrKeyOfValue>::key_type;

template <typename T, typename VoidOrKeyOfValue, typename VoidOrKeyHash>
using hash_key_hash_t = get_hash_t<VoidOrKeyHash, hash_key_types_t<T, VoidOrKeyOfValue>>;

template <typename T, typename VoidOrKeyOfValue, typename VoidOrKeyEqual>
using hash_key_equal_t = get_equal_to_t<VoidOrKeyEqual, hash_key_types_t<T, VoidOrKeyOfValue>>;

//bucket_hash_t
//Stores bucket_plus_vtraits plust the hash function
template<class ValueTraits, class VoidOrKeyOfValue, class VoidOrKeyHash, class BucketTraits>
struct bucket_hash_t
   : bucket_plus_vtraits<ValueTraits, BucketTraits>  //4
{
   typedef typename bucket_plus_vtraits<ValueTraits,BucketTraits>::value_traits     value_traits;
   typedef typename value_traits::value_type                                        value_type;
   typedef typename value_traits::node_traits                                       node_traits;
   using hasher = hash_key_hash_t<value_type, VoidOrKeyOfValue, VoidOrKeyHash>;
   typedef typename hash_key_types_base<value_type, VoidOrKeyOfValue>::key_of_value key_of_value;

   typedef BucketTraits bucket_traits;
   typedef bucket_plus_vtraits<ValueTraits, BucketTraits> bucket_plus_vtraits_t;
   [[no_unique_address]] hasher functor;

   template<class BucketTraitsType>
   inline bucket_hash_t(const ValueTraits &val_traits, BucketTraitsType&& b_traits, const hasher & h)
      : functor(h), bucket_plus_vtraits_t(val_traits, std::forward<BucketTraitsType>(b_traits))
   {}

   inline const hasher &priv_hasher() const
   {  return functor;  }

   hasher &priv_hasher()
   {  return functor;  }

   using bucket_plus_vtraits_t::priv_stored_or_compute_hash;   //For store_hash == true

   inline std::size_t priv_stored_or_compute_hash(const value_type &v, std::false_type) const  //For store_hash == false
   {  return functor(key_of_value()(v));   }
};

template<class ValueTraits, class BucketTraits, class VoidOrKeyOfValue, class VoidOrKeyEqual>
using hashtable_equal_t =
hash_key_equal_t  < typename bucket_plus_vtraits<ValueTraits, BucketTraits>::value_traits::value_type
    , VoidOrKeyOfValue
    , VoidOrKeyEqual
>;

//bucket_hash_equal_t
//Stores bucket_hash_t and the equality function when the first
//non-empty bucket shall not be cached.
template<class ValueTraits, class VoidOrKeyOfValue, class VoidOrKeyHash, class VoidOrKeyEqual, class BucketTraits, bool>
struct bucket_hash_equal_t
   : public bucket_hash_t<ValueTraits, VoidOrKeyOfValue, VoidOrKeyHash, BucketTraits> //3
{
   using key_equal = hashtable_equal_t<ValueTraits, BucketTraits, VoidOrKeyOfValue, VoidOrKeyEqual>;
   typedef bucket_hash_t<ValueTraits, VoidOrKeyOfValue, VoidOrKeyHash, BucketTraits>   bucket_hash_type;
   typedef bucket_plus_vtraits<ValueTraits,BucketTraits>             bucket_plus_vtraits_t;
   typedef ValueTraits                                               value_traits;
   typedef typename bucket_hash_type::hasher    hasher;
   typedef BucketTraits                         bucket_traits;
   typedef typename bucket_plus_vtraits_t::slist_impl       slist_impl;
   typedef typename slist_impl::iterator                    siterator;
   typedef bucket_impl<slist_impl>                  bucket_type;
   typedef typename detail::unordered_bucket_ptr_impl<value_traits>::type bucket_ptr;
   [[no_unique_address]] key_equal functor;

   template<class BucketTraitsType>
   bucket_hash_equal_t(const ValueTraits &val_traits, BucketTraitsType&& b_traits, const hasher & h, const key_equal &e)
      : bucket_hash_type(val_traits, std::forward<BucketTraitsType>(b_traits), h)
      , functor(e)
   {
   }

   inline bucket_ptr priv_get_cache()
   {  return this->bucket_hash_type::priv_bucket_pointer();   }

   inline void priv_set_cache(const bucket_ptr &)
   {}

   inline std::size_t priv_get_cache_bucket_num()
   {  return 0u;  }

   inline void priv_initialize_cache()
   {}

   inline void priv_swap_cache(bucket_hash_equal_t &)
   {}

   siterator priv_begin() const
   {
      std::size_t n = 0;
      std::size_t bucket_cnt = this->bucket_hash_type::priv_bucket_count();
      for (n = 0; n < bucket_cnt; ++n){
         bucket_type &b = this->bucket_hash_type::priv_bucket_pointer()[n];
         if(!b.empty()){
            return b.begin();
         }
      }
      return this->bucket_hash_type::priv_invalid_local_it();
   }

   inline void priv_insertion_update_cache(std::size_t)
   {}

   inline void priv_erasure_update_cache_range(std::size_t, std::size_t)
   {}

   inline void priv_erasure_update_cache()
   {}

   inline const key_equal &priv_equal() const
   {  return functor;  }

   inline key_equal &priv_equal()
   {  return functor;  }
};

//bucket_hash_equal_t
//Stores bucket_hash_t and the equality function when the first
//non-empty bucket shall be cached.
template<class ValueTraits, class VoidOrKeyOfValue, class VoidOrKeyHash, class VoidOrKeyEqual, class BucketTraits>  //cache_begin == true version
struct bucket_hash_equal_t<ValueTraits, VoidOrKeyOfValue, VoidOrKeyHash, VoidOrKeyEqual, BucketTraits, true>
   : bucket_hash_t<ValueTraits, VoidOrKeyOfValue, VoidOrKeyHash, BucketTraits> //2
{
   using key_equal = hashtable_equal_t<ValueTraits, BucketTraits, VoidOrKeyOfValue, VoidOrKeyEqual>;

   typedef bucket_plus_vtraits<ValueTraits,BucketTraits>             bucket_plus_vtraits_t;
   typedef ValueTraits                                               value_traits;
   typedef bucket_hash_t<ValueTraits, VoidOrKeyOfValue, VoidOrKeyHash, BucketTraits>   bucket_hash_type;
   typedef typename bucket_hash_type::hasher                         hasher;
   typedef BucketTraits                                              bucket_traits;
   typedef typename bucket_plus_vtraits_t::slist_impl::iterator      siterator;
   [[no_unique_address]] key_equal functor;

   template<class BucketTraitsType>
   bucket_hash_equal_t(const ValueTraits &val_traits, BucketTraitsType&& b_traits, const hasher & h, const key_equal &e)
      : bucket_hash_type(val_traits, std::forward<BucketTraitsType>(b_traits), h)
      , functor(e)
   {}

   typedef typename detail::unordered_bucket_ptr_impl
      <typename bucket_hash_type::value_traits>::type bucket_ptr;

   inline bucket_ptr &priv_get_cache()
   {  return cached_begin_;   }

   inline const bucket_ptr &priv_get_cache() const
   {  return cached_begin_;   }

   inline void priv_set_cache(const bucket_ptr &p)
   {  cached_begin_ = p;   }

   inline std::size_t priv_get_cache_bucket_num()
   {  return this->cached_begin_ - this->bucket_hash_type::priv_bucket_pointer();  }

   inline void priv_initialize_cache()
   {  this->cached_begin_ = this->bucket_hash_type::priv_invalid_bucket();  }

   inline void priv_swap_cache(bucket_hash_equal_t &other)
   {
      using std::swap;
      swap(this->cached_begin_, other.cached_begin_);
   }

   siterator priv_begin() const
   {
      if(this->cached_begin_ == this->bucket_hash_type::priv_invalid_bucket()){
         return this->bucket_hash_type::priv_invalid_local_it();
      }
      else{
         return this->cached_begin_->begin();
      }
   }

   void priv_insertion_update_cache(std::size_t insertion_bucket)
   {
      bucket_ptr p = this->bucket_hash_type::priv_bucket_pointer() + insertion_bucket;
      if(p < this->cached_begin_){
         this->cached_begin_ = p;
      }
   }

   inline const key_equal &priv_equal() const
   {  return functor;  }

   inline key_equal &priv_equal()
   {  return functor;  }

   void priv_erasure_update_cache_range(std::size_t first_bucket_num, std::size_t last_bucket_num)
   {
      //If the last bucket is the end, the cache must be updated
      //to the last position if all
      if(this->priv_get_cache_bucket_num() == first_bucket_num   &&
         this->bucket_hash_type::priv_bucket_pointer()[first_bucket_num].empty()          ){
         this->priv_set_cache(this->bucket_hash_type::priv_bucket_pointer() + last_bucket_num);
         this->priv_erasure_update_cache();
      }
   }

   void priv_erasure_update_cache()
   {
      if(this->cached_begin_ != this->bucket_hash_type::priv_invalid_bucket()){
         std::size_t current_n = this->priv_get_cache() - this->bucket_hash_type::priv_bucket_pointer();
         for( const std::size_t num_buckets = this->bucket_hash_type::priv_bucket_count()
            ; current_n < num_buckets
            ; ++current_n, ++this->priv_get_cache()){
            if(!this->priv_get_cache()->empty()){
               return;
            }
         }
         this->priv_initialize_cache();
      }
   }

   bucket_ptr cached_begin_;
};

//hashdata_internal
//Stores bucket_hash_equal_t and split_traits
template<class ValueTraits, class VoidOrKeyOfValue, class VoidOrKeyHash, class VoidOrKeyEqual, class BucketTraits, std::size_t BoolFlags>
struct hashdata_internal
   : bucket_hash_equal_t
         < ValueTraits, VoidOrKeyOfValue, VoidOrKeyHash, VoidOrKeyEqual
         , BucketTraits
         , 0 != (BoolFlags & hash_bool_flags::cache_begin_pos)
         >
{
   typedef bucket_hash_equal_t
         < ValueTraits, VoidOrKeyOfValue, VoidOrKeyHash, VoidOrKeyEqual
         , BucketTraits
         , 0 != (BoolFlags & hash_bool_flags::cache_begin_pos)
         >                                                  internal_type;
   typedef typename internal_type::key_equal                key_equal;
   typedef typename internal_type::hasher                   hasher;
   typedef bucket_plus_vtraits<ValueTraits,BucketTraits>    bucket_plus_vtraits_t;
   typedef typename bucket_plus_vtraits_t::bucket_ptr       bucket_ptr;
   typedef typename bucket_plus_vtraits_t::const_value_traits_ptr   const_value_traits_ptr;
   typedef typename bucket_plus_vtraits_t::siterator        siterator;
   typedef typename bucket_plus_vtraits_t::bucket_traits    bucket_traits;
   typedef typename bucket_plus_vtraits_t::value_traits     value_traits;
   typedef typename bucket_plus_vtraits_t::bucket_type      bucket_type;
   typedef typename value_traits::value_type                value_type;
   typedef typename value_traits::pointer                   pointer;
   typedef typename value_traits::const_pointer             const_pointer;
   typedef typename pointer_traits<pointer>::reference      reference;
   typedef typename pointer_traits
      <const_pointer>::reference                            const_reference;
   typedef typename value_traits::node_traits               node_traits;
   typedef typename node_traits::node                       node;
   typedef typename node_traits::node_ptr                  node_ptr;
   typedef typename node_traits::const_node_ptr          const_node_ptr;
   typedef detail::node_functions<node_traits>              node_functions_t;
   using slist_impl = get_slist_impl_t<reduced_slist_node_traits_t<typename value_traits::node_traits>>;
   typedef typename slist_impl::node_algorithms             node_algorithms;
   typedef typename slist_impl::node_ptr                   slist_node_ptr;

   typedef hash_key_types_base
      < typename ValueTraits::value_type
      , VoidOrKeyOfValue
      >                                                              hash_types_base;
   typedef typename hash_types_base::key_of_value                    key_of_value;

   using split_traits =
       detail::size_holder<(BoolFlags & hash_bool_flags::incremental_pos) != 0, struct hashdata_internal>;

   [[no_unique_address]] split_traits split_traits_;
   static const bool store_hash = detail::have_store_hash_v<node_traits>;
   static constexpr bool safemode_or_autounlink = is_safe_autounlink(value_traits::link_mode);
   static constexpr bool stateful_value_traits = detail::is_stateful_value_traits_v<value_traits>;

   typedef std::bool_constant<store_hash>                                 store_hash_t;

   typedef detail::transform_iterator
      < typename slist_impl::iterator
      , downcast_node_to_value_t
         < value_traits
         , false> >   local_iterator;

   typedef detail::transform_iterator
      < typename slist_impl::iterator
      , downcast_node_to_value_t
         < value_traits
         , true> >    const_local_iterator;
   //

   template<class BucketTraitsType>
   hashdata_internal( const ValueTraits &val_traits, BucketTraitsType&& b_traits
                    , const hasher & h, const key_equal &e)
      :  internal_type(val_traits, std::forward<BucketTraitsType>(b_traits), h, e)
   {}

   ~hashdata_internal()
   {  this->priv_clear_buckets();  }

   void priv_clear_buckets()
   {
      this->internal_type::priv_clear_buckets
         ( this->priv_get_cache()
         , this->internal_type::priv_bucket_count()
            - (this->priv_get_cache()
               - this->internal_type::priv_bucket_pointer()));
   }

   void priv_clear_buckets_and_cache()
   {
      this->priv_clear_buckets();
      this->priv_initialize_cache();
   }

   void priv_initialize_buckets_and_cache()
   {
      this->internal_type::priv_clear_buckets
         ( this->internal_type::priv_bucket_pointer()
         , this->internal_type::priv_bucket_count());
      this->priv_initialize_cache();
   }

   typedef hashtable_iterator<bucket_plus_vtraits_t, false>          iterator;
   typedef hashtable_iterator<bucket_plus_vtraits_t, true>           const_iterator;

   template <bool store_hash>
   static std::size_t priv_stored_hash(slist_node_ptr n)
   {  return bucket_plus_vtraits<ValueTraits, BucketTraits>::template priv_stored_hash<store_hash>(n); }

   //public functions
   inline std::size_t split_count() const noexcept
   {
      return this->split_traits_.get_size();
   }

   inline iterator iterator_to(reference value) noexcept
   {
      return iterator(bucket_type::s_iterator_to
         (this->priv_value_to_node(value)), &this->get_bucket_value_traits());
   }

   const_iterator iterator_to(const_reference value) const noexcept
   {
      siterator const sit = bucket_type::s_iterator_to
         ( *pointer_traits<node_ptr>::const_cast_from
            (pointer_traits<const_node_ptr>::pointer_to(this->priv_value_to_node(value)))
         );
      return const_iterator(sit, &this->get_bucket_value_traits());
   }

   static local_iterator s_local_iterator_to(reference value) noexcept
   {
      static_assert((!stateful_value_traits));
      siterator sit = bucket_type::s_iterator_to(*value_traits::to_node_ptr(value));
      return local_iterator(sit, const_value_traits_ptr());
   }

   static const_local_iterator s_local_iterator_to(const_reference value) noexcept
   {
      static_assert((!stateful_value_traits));
      siterator const sit = bucket_type::s_iterator_to
         ( *pointer_traits<node_ptr>::const_cast_from
            (value_traits::to_node_ptr(value))
         );
      return const_local_iterator(sit, const_value_traits_ptr());
   }

   local_iterator local_iterator_to(reference value) noexcept
   {
      siterator sit = bucket_type::s_iterator_to(this->priv_value_to_node(value));
      return local_iterator(sit, this->priv_value_traits_ptr());
   }

   const_local_iterator local_iterator_to(const_reference value) const noexcept
   {
      siterator sit = bucket_type::s_iterator_to
         ( *pointer_traits<node_ptr>::const_cast_from
            (pointer_traits<const_node_ptr>::pointer_to(this->priv_value_to_node(value)))
         );
      return const_local_iterator(sit, this->priv_value_traits_ptr());
   }

   inline std::size_t bucket_count() const noexcept
   {
      const std::size_t bc = this->priv_bucket_count();
     BOOST_INTRUSIVE_INVARIANT_ASSERT(bc <= std::size_t(-1));
      return static_cast<std::size_t>(bc);
   }

   inline std::size_t bucket_size(std::size_t n) const noexcept
   {  return this->priv_bucket_pointer()[n].size();   }

   inline bucket_ptr bucket_pointer() const noexcept
   {  return this->priv_bucket_pointer();   }

   inline local_iterator begin(std::size_t n) noexcept
   {  return local_iterator(this->priv_bucket_pointer()[n].begin(), this->priv_value_traits_ptr());  }

   inline const_local_iterator begin(std::size_t n) const noexcept
   {  return this->cbegin(n);  }

   static inline std::size_t suggested_upper_bucket_count(std::size_t n) noexcept
   {
      return prime_list_holder<0>::suggested_upper_bucket_count(n);
   }

   static inline std::size_t suggested_lower_bucket_count(std::size_t n) noexcept
   {
      return prime_list_holder<0>::suggested_lower_bucket_count(n);
   }

   const_local_iterator cbegin(std::size_t n) const noexcept
   {
      return const_local_iterator
         ( pointer_traits<bucket_ptr>::const_cast_from(this->priv_bucket_pointer())[n].begin()
         , this->priv_value_traits_ptr());
   }

   using internal_type::end;
   using internal_type::cend;

   local_iterator end(std::size_t n) noexcept
   {  return local_iterator(this->priv_bucket_pointer()[n].end(), this->priv_value_traits_ptr());  }

   inline const_local_iterator end(std::size_t n) const noexcept
   {  return this->cend(n);  }

   const_local_iterator cend(std::size_t n) const noexcept
   {
      return const_local_iterator
         ( pointer_traits<bucket_ptr>::const_cast_from(this->priv_bucket_pointer())[n].end()
         , this->priv_value_traits_ptr());
   }

   //Public functions for hashtable_impl

   inline iterator begin() noexcept
   {  return iterator(this->priv_begin(), &this->get_bucket_value_traits());   }

   inline const_iterator begin() const noexcept
   {  return this->cbegin();  }

   inline const_iterator cbegin() const noexcept
   {  return const_iterator(this->priv_begin(), &this->get_bucket_value_traits());   }

   inline hasher hash_function() const
   {  return this->priv_hasher();  }

   inline key_equal key_eq() const
   {  return this->priv_equal();   }
};

/// @endcond

//! The class template hashtable is an intrusive hash table container, that
//! is used to construct intrusive unordered_set and unordered_multiset containers. The
//! no-throw guarantee holds only, if the VoidOrKeyEqual object and Hasher don't throw.
//!
//! hashtable is a semi-intrusive container: each object to be stored in the
//! container must contain a proper hook, but the container also needs
//! additional auxiliary memory to work: hashtable needs a pointer to an array
//! of type `bucket_type` to be passed in the constructor. This bucket array must
//! have at least the same lifetime as the container. This makes the use of
//! hashtable more complicated than purely intrusive containers.
//! `bucket_type` is default-constructible, copyable and assignable
//!
//! The template parameter \c T is the type to be managed by the container.
//! The user can specify additional options and if no options are provided
//! default options are used.
//!
//! The container supports the following options:
//! \c base_hook<>/member_hook<>/value_traits<>,
//! \c constant_time_size<>, \c hash<> and \c equal<>
//! \c bucket_traits<>, power_2_buckets<>, cache_begin<> and incremental<>.
//!
//! hashtable only provides forward iterators but it provides 4 iterator types:
//! iterator and const_iterator to navigate through the whole container and
//! local_iterator and const_local_iterator to navigate through the values
//! stored in a single bucket. Local iterators are faster and smaller.
//!
//! It's not recommended to use non constant-time size hashtables because several
//! key functions, like "empty()", become non-constant time functions. Non
//! constant_time size hashtables are mainly provided to support auto-unlink hooks.
//!
//! hashtables, does not make automatic rehashings nor
//! offers functions related to a load factor. Rehashing can be explicitly requested
//! and the user must provide a new bucket array that will be used from that moment.
//!
//! Since no automatic rehashing is done, iterators are never invalidated when
//! inserting or erasing elements. Iterators are only invalidated when rehashing.
#if defined(BOOST_INTRUSIVE_DOXYGEN_INVOKED)
template<class T, class ...Options>
#else
template<class ValueTraits, class VoidOrKeyOfValue, class VoidOrKeyHash, class VoidOrKeyEqual, class BucketTraits, std::size_t BoolFlags>
#endif
class hashtable_impl
   : hashdata_internal
         < ValueTraits
         , VoidOrKeyOfValue, VoidOrKeyHash, VoidOrKeyEqual
         , BucketTraits
         , BoolFlags & (hash_bool_flags::incremental_pos | hash_bool_flags::cache_begin_pos) //1
         >
{
   typedef hashdata_internal
         < ValueTraits
         , VoidOrKeyOfValue, VoidOrKeyHash, VoidOrKeyEqual
         , BucketTraits
         , BoolFlags & (hash_bool_flags::incremental_pos | hash_bool_flags::cache_begin_pos) //1
         > internal_type;
   typedef hash_key_types_base
      < typename ValueTraits::value_type
      , VoidOrKeyOfValue
      >                                                              hash_types_base;
   using size_traits =
       detail::size_holder<(BoolFlags & hash_bool_flags::constant_time_size_pos) != 0, struct hashtable_impl>;

   [[no_unique_address]] size_traits size_traits_;
   public:
   typedef ValueTraits  value_traits;

   /// @cond
   typedef BucketTraits                                              bucket_traits;

   typedef typename internal_type::slist_impl                        slist_impl;
   typedef bucket_plus_vtraits<ValueTraits, BucketTraits>            bucket_plus_vtraits_t;
   typedef typename bucket_plus_vtraits_t::const_value_traits_ptr    const_value_traits_ptr;

   using internal_type::begin;
   using internal_type::cbegin;
   using internal_type::end;
   using internal_type::cend;
   using internal_type::hash_function;
   using internal_type::key_eq;
   using internal_type::bucket_size;
   using internal_type::bucket_count;
   using internal_type::local_iterator_to;
   using internal_type::s_local_iterator_to;
   using internal_type::iterator_to;
   using internal_type::bucket_pointer;
   using internal_type::suggested_upper_bucket_count;
   using internal_type::suggested_lower_bucket_count;
   using internal_type::split_count;

   /// @endcond

   typedef typename value_traits::pointer                            pointer;
   typedef typename value_traits::const_pointer                      const_pointer;
   typedef typename value_traits::value_type                         value_type;
   typedef typename hash_types_base::key_type                        key_type;
   typedef typename hash_types_base::key_of_value                    key_of_value;
   typedef typename pointer_traits<pointer>::reference               reference;
   typedef typename pointer_traits<const_pointer>::reference         const_reference;
   typedef typename pointer_traits<pointer>::difference_type         difference_type;
   typedef typename internal_type::key_equal                         key_equal;
   typedef typename internal_type::hasher                            hasher;
   typedef bucket_impl<slist_impl>                           bucket_type;
   typedef typename internal_type::bucket_ptr                        bucket_ptr;
   typedef typename slist_impl::iterator                             siterator;
   typedef typename slist_impl::const_iterator                       const_siterator;
   typedef typename internal_type::iterator                          iterator;
   typedef typename internal_type::const_iterator                    const_iterator;
   typedef typename internal_type::local_iterator                    local_iterator;
   typedef typename internal_type::const_local_iterator              const_local_iterator;
   typedef typename value_traits::node_traits                        node_traits;
   typedef typename node_traits::node                                node;
   typedef typename pointer_traits
      <pointer>::template rebind_pointer
         < node >::type                                              node_ptr;
   typedef typename pointer_traits
      <pointer>::template rebind_pointer
         < const node >::type                                        const_node_ptr;
   typedef typename pointer_traits
      <node_ptr>::reference                                          node_reference;
   typedef typename pointer_traits
      <const_node_ptr>::reference                                    const_node_reference;
   typedef typename slist_impl::node_algorithms                      node_algorithms;

   static const bool stateful_value_traits = internal_type::stateful_value_traits;
   static const bool store_hash = internal_type::store_hash;

   static const bool unique_keys          = 0 != (BoolFlags & hash_bool_flags::unique_keys_pos);
   static constexpr bool constant_time_size   = 0 != (BoolFlags & hash_bool_flags::constant_time_size_pos);
   static constexpr bool cache_begin          = 0 != (BoolFlags & hash_bool_flags::cache_begin_pos);
   static const bool compare_hash         = 0 != (BoolFlags & hash_bool_flags::compare_hash_pos);
   static const bool incremental          = 0 != (BoolFlags & hash_bool_flags::incremental_pos);
   static const bool power_2_buckets      = incremental || (0 != (BoolFlags & hash_bool_flags::power_2_buckets_pos));

   static constexpr bool optimize_multikey
      = detail::have_optimize_multikey_v<node_traits> && !unique_keys;

   /// @cond
   static const bool is_multikey = !unique_keys;
   private:

   //Configuration error: compare_hash<> can't be specified without store_hash<>
   //See documentation for more explanations
   static_assert((!compare_hash || store_hash));

   typedef typename slist_impl::node_ptr                            slist_node_ptr;
   typedef typename pointer_traits
      <slist_node_ptr>::template rebind_pointer
         < void >::type                                              void_pointer;
   //We'll define group traits, but these won't be instantiated if
   //optimize_multikey is not true
   typedef unordered_group_adapter<node_traits>                      group_traits;
   typedef circular_slist_algorithms<group_traits>                   group_algorithms;
   typedef typename internal_type::store_hash_t                      store_hash_t;
   typedef typename internal_type::split_traits                      split_traits;
   typedef detail::group_functions<node_traits>                      group_functions_t;
   typedef detail::node_functions<node_traits>                       node_functions_t;

   private:
   static constexpr bool safemode_or_autounlink = internal_type::safemode_or_autounlink;

   //Constant-time size is incompatible with auto-unlink hooks!
   static_assert(!(constant_time_size && value_traits::link_mode == link_mode_type::auto_unlink));
   //Cache begin is incompatible with auto-unlink hooks!
   static_assert(!(cache_begin && value_traits::link_mode == link_mode_type::auto_unlink));

   template<class Disposer>
   struct typeof_node_disposer
   {
      typedef node_cast_adaptor
         < detail::node_disposer< Disposer, value_traits, CircularSListAlgorithms>
         , slist_node_ptr, node_ptr > type;
   };

   template<class Disposer>
   typename typeof_node_disposer<Disposer>::type
      make_node_disposer(const Disposer &disposer) const
   {
      typedef typename typeof_node_disposer<Disposer>::type return_t;
      return return_t(disposer, &this->priv_value_traits());
   }

   /// @endcond

   public:
   typedef detail::insert_commit_data_impl insert_commit_data;


   public:

   //! <b>Requires</b>: buckets must not be being used by any other resource.
   //!
   //! <b>Effects</b>: Constructs an empty unordered_set, storing a reference
   //!   to the bucket array and copies of the key_hasher and equal_func functors.
   //!
   //! <b>Complexity</b>: Constant.
   //!
   //! <b>Throws</b>: If value_traits::node_traits::node
   //!   constructor throws (this does not happen with predefined Boost.Intrusive hooks)
   //!   or the copy constructor or invocation of hash_func or equal_func throws.
   //!
   //! <b>Notes</b>: buckets array must be disposed only after
   //!   *this is disposed.
   explicit hashtable_impl ( const bucket_traits &b_traits
                           , const hasher & hash_func = hasher()
                           , const key_equal &equal_func = key_equal()
                           , const value_traits &v_traits = value_traits())
      :  internal_type(v_traits, b_traits, hash_func, equal_func)
   {
      this->priv_initialize_buckets_and_cache();
      this->size_traits_.set_size(0);
      std::size_t bucket_sz = this->bucket_count();
     BOOST_INTRUSIVE_INVARIANT_ASSERT(bucket_sz != 0);
      //Check power of two bucket array if the option is activated
     BOOST_INTRUSIVE_INVARIANT_ASSERT
         (!power_2_buckets || (0 == (bucket_sz & (bucket_sz-1))));
      this->split_traits_.set_size(bucket_sz>>1);
   }

   //! <b>Requires</b>: buckets must not be being used by any other resource
   //!   and dereferencing iterator must yield an lvalue of type value_type.
   //!
   //! <b>Effects</b>: Constructs an empty container and inserts elements from
   //!   [b, e).
   //!
   //! <b>Complexity</b>: If N is distance(b, e): Average case is O(N)
   //!   (with a good hash function and with buckets_len >= N),worst case O(N^2).
   //!
   //! <b>Throws</b>: If value_traits::node_traits::node
   //!   constructor throws (this does not happen with predefined Boost.Intrusive hooks)
   //!   or the copy constructor or invocation of hasher or key_equal throws.
   //!
   //! <b>Notes</b>: buckets array must be disposed only after
   //!   *this is disposed.
   template<class Iterator>
   hashtable_impl ( bool unique, Iterator b, Iterator e
                  , const bucket_traits &b_traits
                  , const hasher & hash_func = hasher()
                  , const key_equal &equal_func = key_equal()
                  , const value_traits &v_traits = value_traits())
      :  internal_type(v_traits, b_traits, hash_func, equal_func)
   {
      this->priv_initialize_buckets_and_cache();
      this->size_traits_.set_size(0);
      std::size_t bucket_sz = this->bucket_count();
      BOOST_INTRUSIVE_INVARIANT_ASSERT(bucket_sz != 0);
      //Check power of two bucket array if the option is activated
      BOOST_INTRUSIVE_INVARIANT_ASSERT
         (!power_2_buckets || (0 == (bucket_sz & (bucket_sz-1))));
      this->split_traits_.set_size(bucket_sz>>1);
      //Now insert
      if(unique)
         this->insert_unique(b, e);
      else
         this->insert_equal(b, e);
   }

   hashtable_impl(const hashtable_impl&) = delete;
   hashtable_impl& operator=(const hashtable_impl&) = delete;
   //! <b>Effects</b>: Constructs a container moving resources from another container.
   //!   Internal value traits, bucket traits, hasher and comparison are move constructed and
   //!   nodes belonging to x are linked to *this.
   //!
   //! <b>Complexity</b>: Constant.
   //!
   //! <b>Throws</b>: If value_traits::node_traits::node's
   //!   move constructor throws (this does not happen with predefined Boost.Intrusive hooks)
   //!   or the move constructor of value traits, bucket traits, hasher or comparison throws.
   hashtable_impl(hashtable_impl &&x)
      : internal_type( std::move(x.priv_value_traits())
                     , std::move(x.priv_bucket_traits())
                     , std::move(x.priv_hasher())
                     , std::move(x.priv_equal())
                     )
   {
      this->priv_swap_cache(x);
      x.priv_initialize_cache();
      this->size_traits_.set_size(x.size_traits_.get_size());
      x.size_traits_.set_size(0);
      this->split_traits_.set_size(x.split_traits_.get_size());
      x.split_traits_.set_size(0);
   }

   //! <b>Effects</b>: Equivalent to swap.
   //!
   hashtable_impl& operator=(hashtable_impl&& x)
   {  this->swap(x); return *this;  }

   #if defined(BOOST_INTRUSIVE_DOXYGEN_INVOKED)
   //! <b>Effects</b>: Detaches all elements from this. The objects in the unordered_set
   //!   are not deleted (i.e. no destructors are called).
   //!
   //! <b>Complexity</b>: Linear to the number of elements in the unordered_set, if
   //!   it's a safe-mode or auto-unlink value. Otherwise constant.
   //!
   //! <b>Throws</b>: Nothing.
   ~hashtable_impl();

   //! <b>Effects</b>: Returns an iterator pointing to the beginning of the unordered_set.
   //!
   //! <b>Complexity</b>: Amortized constant time.
   //!   Worst case (empty unordered_set): O(this->bucket_count())
   //!
   //! <b>Throws</b>: Nothing.
   iterator begin() noexcept;

   //! <b>Effects</b>: Returns a const_iterator pointing to the beginning
   //!   of the unordered_set.
   //!
   //! <b>Complexity</b>: Amortized constant time.
   //!   Worst case (empty unordered_set): O(this->bucket_count())
   //!
   //! <b>Throws</b>: Nothing.
   const_iterator begin() const noexcept;

   //! <b>Effects</b>: Returns a const_iterator pointing to the beginning
   //!   of the unordered_set.
   //!
   //! <b>Complexity</b>: Amortized constant time.
   //!   Worst case (empty unordered_set): O(this->bucket_count())
   //!
   //! <b>Throws</b>: Nothing.
   const_iterator cbegin() const noexcept;

   //! <b>Effects</b>: Returns an iterator pointing to the end of the unordered_set.
   //!
   //! <b>Complexity</b>: Constant.
   //!
   //! <b>Throws</b>: Nothing.
   iterator end() noexcept;

   //! <b>Effects</b>: Returns a const_iterator pointing to the end of the unordered_set.
   //!
   //! <b>Complexity</b>: Constant.
   //!
   //! <b>Throws</b>: Nothing.
   const_iterator end() const noexcept;

   //! <b>Effects</b>: Returns a const_iterator pointing to the end of the unordered_set.
   //!
   //! <b>Complexity</b>: Constant.
   //!
   //! <b>Throws</b>: Nothing.
   const_iterator cend() const noexcept;

   //! <b>Effects</b>: Returns the hasher object used by the unordered_set.
   //!
   //! <b>Complexity</b>: Constant.
   //!
   //! <b>Throws</b>: If hasher copy-constructor throws.
   hasher hash_function() const;

   //! <b>Effects</b>: Returns the key_equal object used by the unordered_set.
   //!
   //! <b>Complexity</b>: Constant.
   //!
   //! <b>Throws</b>: If key_equal copy-constructor throws.
   key_equal key_eq() const;

   #endif

   //! <b>Effects</b>: Returns true if the container is empty.
   //!
   //! <b>Complexity</b>: if constant-time size and cache_begin options are disabled,
   //!   average constant time (worst case, with empty() == true: O(this->bucket_count()).
   //!   Otherwise constant.
   //!
   //! <b>Throws</b>: Nothing.
   bool empty() const noexcept
   {
      if constexpr (constant_time_size) {
         return !this->size();
      }
      else if constexpr (cache_begin) {
         return this->begin() == this->end();
      }
      else{
         std::size_t bucket_cnt = this->bucket_count();
         const bucket_type *b = boost::movelib::to_raw_pointer(this->priv_bucket_pointer());
         for (std::size_t n = 0; n < bucket_cnt; ++n, ++b){
            if(!b->empty()){
               return false;
            }
         }
         return true;
      }
   }

   //! <b>Effects</b>: Returns the number of elements stored in the unordered_set.
   //!
   //! <b>Complexity</b>: Linear to elements contained in *this if
   //!   constant_time_size is false. Constant-time otherwise.
   //!
   //! <b>Throws</b>: Nothing.
   std::size_t size() const noexcept
   {
      if constexpr (constant_time_size)
         return this->size_traits_.get_size();
      else{
         std::size_t len = 0;
         std::size_t bucket_cnt = this->bucket_count();
         const bucket_type *b = boost::movelib::to_raw_pointer(this->priv_bucket_pointer());
         for (std::size_t n = 0; n < bucket_cnt; ++n, ++b){
            len += b->size();
         }
         return len;
      }
   }

   //! <b>Requires</b>: the hasher and the equality function unqualified swap
   //!   call should not throw.
   //!
   //! <b>Effects</b>: Swaps the contents of two unordered_sets.
   //!   Swaps also the contained bucket array and equality and hasher functors.
   //!
   //! <b>Complexity</b>: Constant.
   //!
   //! <b>Throws</b>: If the swap() call for the comparison or hash functors
   //!   found using ADL throw. Basic guarantee.
   void swap(hashtable_impl& other)
   {
      //These can throw
      using std::swap;
      swap(this->priv_equal(),  other.priv_equal());
      swap(this->priv_hasher(), other.priv_hasher());
      //These can't throw
      swap(this->priv_bucket_traits(), other.priv_bucket_traits());
      swap(this->priv_value_traits(), other.priv_value_traits());
      this->priv_swap_cache(other);
      this->size_traits_.swap(other.size_traits_);
      this->split_traits_.swap(other.split_traits_);
   }

   //! <b>Requires</b>: Disposer::operator()(pointer) shouldn't throw
   //!   Cloner should yield to nodes that compare equal and produce the same
   //!   hash than the original node.
   //!
   //! <b>Effects</b>: Erases all the elements from *this
   //!   calling Disposer::operator()(pointer), clones all the
   //!   elements from src calling Cloner::operator()(const_reference )
   //!   and inserts them on *this. The hash function and the equality
   //!   predicate are copied from the source.
   //!
   //!   If store_hash option is true, this method does not use the hash function.
   //!
   //!   If any operation throws, all cloned elements are unlinked and disposed
   //!   calling Disposer::operator()(pointer).
   //!
   //! <b>Complexity</b>: Linear to erased plus inserted elements.
   //!
   //! <b>Throws</b>: If cloner or hasher throw or hash or equality predicate copying
   //!   throws. Basic guarantee.
   template <class Cloner, class Disposer>
   inline void clone_from(const hashtable_impl &src, Cloner cloner, Disposer disposer)
   {  this->priv_clone_from(src, cloner, disposer);   }

   //! <b>Requires</b>: Disposer::operator()(pointer) shouldn't throw
   //!   Cloner should yield to nodes that compare equal and produce the same
   //!   hash than the original node.
   //!
   //! <b>Effects</b>: Erases all the elements from *this
   //!   calling Disposer::operator()(pointer), clones all the
   //!   elements from src calling Cloner::operator()(reference)
   //!   and inserts them on *this. The hash function and the equality
   //!   predicate are copied from the source.
   //!
   //!   If store_hash option is true, this method does not use the hash function.
   //!
   //!   If any operation throws, all cloned elements are unlinked and disposed
   //!   calling Disposer::operator()(pointer).
   //!
   //! <b>Complexity</b>: Linear to erased plus inserted elements.
   //!
   //! <b>Throws</b>: If cloner or hasher throw or hash or equality predicate copying
   //!   throws. Basic guarantee.
   template <class Cloner, class Disposer>
   inline void clone_from(hashtable_impl &&src, Cloner cloner, Disposer disposer)
   {  this->priv_clone_from(static_cast<hashtable_impl&>(src), cloner, disposer);   }

   //! <b>Requires</b>: value must be an lvalue
   //!
   //! <b>Effects</b>: Inserts the value into the unordered_set.
   //!
   //! <b>Returns</b>: An iterator to the inserted value.
   //!
   //! <b>Complexity</b>: Average case O(1), worst case O(this->size()).
   //!
   //! <b>Throws</b>: If the internal hasher or the equality functor throws. Strong guarantee.
   //!
   //! <b>Note</b>: Does not affect the validity of iterators and references.
   //!   No copy-constructors are called.
   iterator insert_equal(reference value)
   {
      std::size_t bucket_num;
      std::size_t hash_value;
      siterator prev;
      siterator const it = this->priv_find
         (key_of_value()(value), this->priv_hasher(), this->priv_equal(), bucket_num, hash_value, prev);
      bool const next_is_in_group = optimize_multikey && it != this->priv_invalid_local_it();
      return this->priv_insert_equal_after_find(value, bucket_num, hash_value, prev, next_is_in_group);
   }

   //! <b>Requires</b>: Dereferencing iterator must yield an lvalue
   //!   of type value_type.
   //!
   //! <b>Effects</b>: Equivalent to this->insert_equal(t) for each element in [b, e).
   //!
   //! <b>Complexity</b>: Average case O(N), where N is distance(b, e).
   //!   Worst case O(N*this->size()).
   //!
   //! <b>Throws</b>: If the internal hasher or the equality functor throws. Basic guarantee.
   //!
   //! <b>Note</b>: Does not affect the validity of iterators and references.
   //!   No copy-constructors are called.
   template<class Iterator>
   void insert_equal(Iterator b, Iterator e)
   {
      for (; b != e; ++b)
         this->insert_equal(*b);
   }

   //! <b>Requires</b>: value must be an lvalue
   //!
   //! <b>Effects</b>: Tries to inserts value into the unordered_set.
   //!
   //! <b>Returns</b>: If the value
   //!   is not already present inserts it and returns a pair containing the
   //!   iterator to the new value and true. If there is an equivalent value
   //!   returns a pair containing an iterator to the already present value
   //!   and false.
   //!
   //! <b>Complexity</b>: Average case O(1), worst case O(this->size()).
   //!
   //! <b>Throws</b>: If the internal hasher or the equality functor throws. Strong guarantee.
   //!
   //! <b>Note</b>: Does not affect the validity of iterators and references.
   //!   No copy-constructors are called.
   std::pair<iterator, bool> insert_unique(reference value)
   {
      insert_commit_data commit_data;
      std::pair<iterator, bool> ret = this->insert_unique_check(key_of_value()(value), commit_data);
      if(ret.second){
         ret.first = this->insert_unique_commit(value, commit_data);
      }
      return ret;
   }

   //! <b>Requires</b>: Dereferencing iterator must yield an lvalue
   //!   of type value_type.
   //!
   //! <b>Effects</b>: Equivalent to this->insert_unique(t) for each element in [b, e).
   //!
   //! <b>Complexity</b>: Average case O(N), where N is distance(b, e).
   //!   Worst case O(N*this->size()).
   //!
   //! <b>Throws</b>: If the internal hasher or the equality functor throws. Basic guarantee.
   //!
   //! <b>Note</b>: Does not affect the validity of iterators and references.
   //!   No copy-constructors are called.
   template<class Iterator>
   void insert_unique(Iterator b, Iterator e)
   {
      for (; b != e; ++b)
         this->insert_unique(*b);
   }

   //! <b>Requires</b>: "hash_func" must be a hash function that induces
   //!   the same hash values as the stored hasher. The difference is that
   //!   "hash_func" hashes the given key instead of the value_type.
   //!
   //!   "equal_func" must be a equality function that induces
   //!   the same equality as key_equal. The difference is that
   //!   "equal_func" compares an arbitrary key with the contained values.
   //!
   //! <b>Effects</b>: Checks if a value can be inserted in the unordered_set, using
   //!   a user provided key instead of the value itself.
   //!
   //! <b>Returns</b>: If there is an equivalent value
   //!   returns a pair containing an iterator to the already present value
   //!   and false. If the value can be inserted returns true in the returned
   //!   pair boolean and fills "commit_data" that is meant to be used with
   //!   the "insert_commit" function.
   //!
   //! <b>Complexity</b>: Average case O(1), worst case O(this->size()).
   //!
   //! <b>Throws</b>: If hash_func or equal_func throw. Strong guarantee.
   //!
   //! <b>Notes</b>: This function is used to improve performance when constructing
   //!   a value_type is expensive: if there is an equivalent value
   //!   the constructed object must be discarded. Many times, the part of the
   //!   node that is used to impose the hash or the equality is much cheaper to
   //!   construct than the value_type and this function offers the possibility to
   //!   use that the part to check if the insertion will be successful.
   //!
   //!   If the check is successful, the user can construct the value_type and use
   //!   "insert_commit" to insert the object in constant-time.
   //!
   //!   "commit_data" remains valid for a subsequent "insert_commit" only if no more
   //!   objects are inserted or erased from the unordered_set.
   //!
   //!   After a successful rehashing insert_commit_data remains valid.
   template<class KeyType, class KeyHasher, class KeyEqual>
   std::pair<iterator, bool> insert_unique_check
      ( const KeyType &key
      , KeyHasher hash_func
      , KeyEqual equal_func
      , insert_commit_data &commit_data)
   {
      std::size_t bucket_num;
      siterator prev;
      siterator const pos = this->priv_find(key, hash_func, equal_func, bucket_num, commit_data.hash, prev);
      return std::pair<iterator, bool>
         ( iterator(pos, &this->get_bucket_value_traits())
         , pos == this->priv_invalid_local_it());
   }

   //! <b>Effects</b>: Checks if a value can be inserted in the unordered_set, using
   //!   a user provided key instead of the value itself.
   //!
   //! <b>Returns</b>: If there is an equivalent value
   //!   returns a pair containing an iterator to the already present value
   //!   and false. If the value can be inserted returns true in the returned
   //!   pair boolean and fills "commit_data" that is meant to be used with
   //!   the "insert_commit" function.
   //!
   //! <b>Complexity</b>: Average case O(1), worst case O(this->size()).
   //!
   //! <b>Throws</b>: If hasher or key_compare throw. Strong guarantee.
   //!
   //! <b>Notes</b>: This function is used to improve performance when constructing
   //!   a value_type is expensive: if there is an equivalent value
   //!   the constructed object must be discarded. Many times, the part of the
   //!   node that is used to impose the hash or the equality is much cheaper to
   //!   construct than the value_type and this function offers the possibility to
   //!   use that the part to check if the insertion will be successful.
   //!
   //!   If the check is successful, the user can construct the value_type and use
   //!   "insert_commit" to insert the object in constant-time.
   //!
   //!   "commit_data" remains valid for a subsequent "insert_commit" only if no more
   //!   objects are inserted or erased from the unordered_set.
   //!
   //!   After a successful rehashing insert_commit_data remains valid.
   inline std::pair<iterator, bool> insert_unique_check
      ( const key_type &key, insert_commit_data &commit_data)
   {  return this->insert_unique_check(key, this->priv_hasher(), this->priv_equal(), commit_data);  }

   //! <b>Requires</b>: value must be an lvalue of type value_type. commit_data
   //!   must have been obtained from a previous call to "insert_check".
   //!   No objects should have been inserted or erased from the unordered_set between
   //!   the "insert_check" that filled "commit_data" and the call to "insert_commit".
   //!
   //! <b>Effects</b>: Inserts the value in the unordered_set using the information obtained
   //!   from the "commit_data" that a previous "insert_check" filled.
   //!
   //! <b>Returns</b>: An iterator to the newly inserted object.
   //!
   //! <b>Complexity</b>: Constant time.
   //!
   //! <b>Throws</b>: Nothing.
   //!
   //! <b>Notes</b>: This function has only sense if a "insert_check" has been
   //!   previously executed to fill "commit_data". No value should be inserted or
   //!   erased between the "insert_check" and "insert_commit" calls.
   //!
   //!   After a successful rehashing insert_commit_data remains valid.
   iterator insert_unique_commit(reference value, const insert_commit_data &commit_data) noexcept
   {
      std::size_t bucket_num = this->priv_hash_to_bucket(commit_data.hash);
      bucket_type &b = this->priv_bucket_pointer()[bucket_num];
      this->size_traits_.increment();
      node_ptr const n = pointer_traits<node_ptr>::pointer_to(this->priv_value_to_node(value));
      BOOST_INTRUSIVE_SAFE_HOOK_DEFAULT_ASSERT(!safemode_or_autounlink || node_algorithms::unique(n));
      if constexpr (store_hash_t::value) node_functions_t::store_hash(n, commit_data.hash);
      this->priv_insertion_update_cache(bucket_num);
      if constexpr (optimize_multikey) group_functions_t::insert_in_group(n, n);
      return iterator(b.insert_after(b.before_begin(), *n), &this->get_bucket_value_traits());
   }

   //! <b>Effects</b>: Erases the element pointed to by i.
   //!
   //! <b>Complexity</b>: Average case O(1), worst case O(this->size()).
   //!
   //! <b>Throws</b>: Nothing.
   //!
   //! <b>Note</b>: Invalidates the iterators (but not the references)
   //!    to the erased element. No destructors are called.
   inline void erase(const_iterator i) noexcept
   {  this->erase_and_dispose(i, detail::null_disposer());  }

   //! <b>Effects</b>: Erases the range pointed to by b end e.
   //!
   //! <b>Complexity</b>: Average case O(distance(b, e)),
   //!   worst case O(this->size()).
   //!
   //! <b>Throws</b>: Nothing.
   //!
   //! <b>Note</b>: Invalidates the iterators (but not the references)
   //!    to the erased elements. No destructors are called.
   inline void erase(const_iterator b, const_iterator e) noexcept
   {  this->erase_and_dispose(b, e, detail::null_disposer());  }

   //! <b>Effects</b>: Erases all the elements with the given value.
   //!
   //! <b>Returns</b>: The number of erased elements.
   //!
   //! <b>Complexity</b>: Average case O(this->count(value)).
   //!   Worst case O(this->size()).
   //!
   //! <b>Throws</b>: If the internal hasher or the equality functor throws.
   //!   Basic guarantee.
   //!
   //! <b>Note</b>: Invalidates the iterators (but not the references)
   //!    to the erased elements. No destructors are called.
   inline std::size_t erase(const key_type &key)
   {  return this->erase(key, this->priv_hasher(), this->priv_equal());  }

   //! <b>Requires</b>: "hash_func" must be a hash function that induces
   //!   the same hash values as the stored hasher. The difference is that
   //!   "hash_func" hashes the given key instead of the value_type.
   //!
   //!   "equal_func" must be a equality function that induces
   //!   the same equality as key_equal. The difference is that
   //!   "equal_func" compares an arbitrary key with the contained values.
   //!
   //! <b>Effects</b>: Erases all the elements that have the same hash and
   //!   compare equal with the given key.
   //!
   //! <b>Returns</b>: The number of erased elements.
   //!
   //! <b>Complexity</b>: Average case O(this->count(value)).
   //!   Worst case O(this->size()).
   //!
   //! <b>Throws</b>: If hash_func or equal_func throw. Basic guarantee.
   //!
   //! <b>Note</b>: Invalidates the iterators (but not the references)
   //!    to the erased elements. No destructors are called.
   template<class KeyType, class KeyHasher, class KeyEqual>
   inline std::size_t erase(const KeyType& key, KeyHasher hash_func, KeyEqual equal_func)
   {  return this->erase_and_dispose(key, hash_func, equal_func, detail::null_disposer()); }

   //! <b>Requires</b>: Disposer::operator()(pointer) shouldn't throw.
   //!
   //! <b>Effects</b>: Erases the element pointed to by i.
   //!   Disposer::operator()(pointer) is called for the removed element.
   //!
   //! <b>Complexity</b>: Average case O(1), worst case O(this->size()).
   //!
   //! <b>Throws</b>: Nothing.
   //!
   //! <b>Note</b>: Invalidates the iterators
   //!    to the erased elements.
   template<class Disposer>
   std::enable_if_t<!std::is_convertible_v<Disposer, const_iterator>, void>
   erase_and_dispose(const_iterator i, Disposer disposer) noexcept
   {
      //Get the bucket number and local iterator for both iterators
      siterator const first_local_it(i.slist_it());
      std::size_t const first_bucket_num = this->priv_get_bucket_num(first_local_it);
      this->template priv_erase_node<optimize_multikey>(this->priv_bucket_pointer()[first_bucket_num], first_local_it, make_node_disposer(disposer));
      this->size_traits_.decrement();
      this->priv_erasure_update_cache_range(first_bucket_num, first_bucket_num);
   }

   //! <b>Requires</b>: Disposer::operator()(pointer) shouldn't throw.
   //!
   //! <b>Effects</b>: Erases the range pointed to by b end e.
   //!   Disposer::operator()(pointer) is called for the removed elements.
   //!
   //! <b>Complexity</b>: Average case O(distance(b, e)),
   //!   worst case O(this->size()).
   //!
   //! <b>Throws</b>: Nothing.
   //!
   //! <b>Note</b>: Invalidates the iterators
   //!    to the erased elements.
   template<class Disposer>
   void erase_and_dispose(const_iterator b, const_iterator e, Disposer disposer) noexcept
   {
      if(b != e){
         //Get the bucket number and local iterator for both iterators
         siterator first_local_it(b.slist_it());
         std::size_t first_bucket_num = this->priv_get_bucket_num(first_local_it);

         const bucket_ptr buck_ptr = this->priv_bucket_pointer();
         siterator before_first_local_it
            = this->priv_get_previous(buck_ptr[first_bucket_num], first_local_it);
         std::size_t last_bucket_num;
         siterator last_local_it;

         //For the end iterator, we will assign the end iterator
         //of the last bucket
         if(e == this->end()){
            last_bucket_num   = this->bucket_count() - 1;
            last_local_it     = buck_ptr[last_bucket_num].end();
         }
         else{
            last_local_it     = e.slist_it();
            last_bucket_num = this->priv_get_bucket_num(last_local_it);
         }
         std::size_t const num_erased = this->template priv_erase_node_range<optimize_multikey>
            ( before_first_local_it, first_bucket_num, last_local_it, last_bucket_num
            , make_node_disposer(disposer));
         this->size_traits_.set_size(this->size_traits_.get_size()-num_erased);
         this->priv_erasure_update_cache_range(first_bucket_num, last_bucket_num);
      }
   }

   //! <b>Requires</b>: Disposer::operator()(pointer) shouldn't throw.
   //!
   //! <b>Effects</b>: Erases all the elements with the given value.
   //!   Disposer::operator()(pointer) is called for the removed elements.
   //!
   //! <b>Returns</b>: The number of erased elements.
   //!
   //! <b>Complexity</b>: Average case O(this->count(value)).
   //!   Worst case O(this->size()).
   //!
   //! <b>Throws</b>: If the internal hasher or the equality functor throws.
   //!   Basic guarantee.
   //!
   //! <b>Note</b>: Invalidates the iterators (but not the references)
   //!    to the erased elements. No destructors are called.
   template<class Disposer>
   inline std::size_t erase_and_dispose(const key_type &key, Disposer disposer)
   {  return this->erase_and_dispose(key, this->priv_hasher(), this->priv_equal(), disposer);   }

   //! <b>Requires</b>: Disposer::operator()(pointer) shouldn't throw.
   //!
   //! <b>Effects</b>: Erases all the elements with the given key.
   //!   according to the comparison functor "equal_func".
   //!   Disposer::operator()(pointer) is called for the removed elements.
   //!
   //! <b>Returns</b>: The number of erased elements.
   //!
   //! <b>Complexity</b>: Average case O(this->count(value)).
   //!   Worst case O(this->size()).
   //!
   //! <b>Throws</b>: If hash_func or equal_func throw. Basic guarantee.
   //!
   //! <b>Note</b>: Invalidates the iterators
   //!    to the erased elements.
   template<class KeyType, class KeyHasher, class KeyEqual, class Disposer>
   std::size_t erase_and_dispose(const KeyType& key, KeyHasher hash_func
                              ,KeyEqual equal_func, Disposer disposer)
   {
      std::size_t bucket_num;
      std::size_t h;
      siterator prev;
      siterator it = this->priv_find(key, hash_func, equal_func, bucket_num, h, prev);
      bool const success = it != this->priv_invalid_local_it();

      std::size_t cnt(0);
      if(success){
         if constexpr (optimize_multikey) {
            cnt = this->template priv_erase_from_single_bucket<optimize_multikey>
               (this->priv_bucket_pointer()[bucket_num], prev, ++(priv_last_in_group)(it), make_node_disposer(disposer));
         }
         else{
            bucket_type &b = this->priv_bucket_pointer()[bucket_num];
            siterator const end_sit = b.end();
            do{
               ++cnt;
               ++it;
            }while(it != end_sit && 
                   this->priv_is_value_equal_to_key
                     (this->priv_value_from_slist_node(it.pointed_node()), h, key, equal_func));
            bucket_type::s_erase_after_and_dispose(prev, it, make_node_disposer(disposer));
         }
         this->size_traits_.set_size(this->size_traits_.get_size()-cnt);
         this->priv_erasure_update_cache();
      }

      return cnt;
   }

   //! <b>Effects</b>: Erases all of the elements.
   //!
   //! <b>Complexity</b>: Linear to the number of elements on the container.
   //!   if it's a safe-mode or auto-unlink value_type. Constant time otherwise.
   //!
   //! <b>Throws</b>: Nothing.
   //!
   //! <b>Note</b>: Invalidates the iterators (but not the references)
   //!    to the erased elements. No destructors are called.
   void clear() noexcept
   {
      this->priv_clear_buckets_and_cache();
      this->size_traits_.set_size(0);
   }

   //! <b>Requires</b>: Disposer::operator()(pointer) shouldn't throw.
   //!
   //! <b>Effects</b>: Erases all of the elements.
   //!
   //! <b>Complexity</b>: Linear to the number of elements on the container.
   //!   Disposer::operator()(pointer) is called for the removed elements.
   //!
   //! <b>Throws</b>: Nothing.
   //!
   //! <b>Note</b>: Invalidates the iterators (but not the references)
   //!    to the erased elements. No destructors are called.
   template<class Disposer>
   void clear_and_dispose(Disposer disposer) noexcept
   {
      if(!constant_time_size || !this->empty()){
         std::size_t num_buckets = this->bucket_count();
         bucket_ptr b = this->priv_bucket_pointer();
         typename typeof_node_disposer<Disposer>::type d(disposer, &this->priv_value_traits());
         for(; num_buckets--; ++b){
            b->clear_and_dispose(d);
         }
         this->size_traits_.set_size(0);
      }
      this->priv_initialize_cache();
   }

   //! <b>Effects</b>: Returns the number of contained elements with the given value
   //!
   //! <b>Complexity</b>: Average case O(1), worst case O(this->size()).
   //!
   //! <b>Throws</b>: If the internal hasher or the equality functor throws.
   inline std::size_t count(const key_type &key) const
   {  return this->count(key, this->priv_hasher(), this->priv_equal());  }

   //! <b>Requires</b>: "hash_func" must be a hash function that induces
   //!   the same hash values as the stored hasher. The difference is that
   //!   "hash_func" hashes the given key instead of the value_type.
   //!
   //!   "equal_func" must be a equality function that induces
   //!   the same equality as key_equal. The difference is that
   //!   "equal_func" compares an arbitrary key with the contained values.
   //!
   //! <b>Effects</b>: Returns the number of contained elements with the given key
   //!
   //! <b>Complexity</b>: Average case O(1), worst case O(this->size()).
   //!
   //! <b>Throws</b>: If hash_func or equal throw.
   template<class KeyType, class KeyHasher, class KeyEqual>
   std::size_t count(const KeyType &key, KeyHasher hash_func, KeyEqual equal_func) const
   {
      std::size_t cnt;
      std::size_t n_bucket;
      this->priv_local_equal_range(key, hash_func, equal_func, n_bucket, cnt);
      return cnt;
   }

   //! <b>Effects</b>: Finds an iterator to the first element is equal to
   //!   "value" or end() if that element does not exist.
   //!
   //! <b>Complexity</b>: Average case O(1), worst case O(this->size()).
   //!
   //! <b>Throws</b>: If the internal hasher or the equality functor throws.
   inline iterator find(const key_type &key)
   {  return this->find(key, this->priv_hasher(), this->priv_equal());   }

   //! <b>Requires</b>: "hash_func" must be a hash function that induces
   //!   the same hash values as the stored hasher. The difference is that
   //!   "hash_func" hashes the given key instead of the value_type.
   //!
   //!   "equal_func" must be a equality function that induces
   //!   the same equality as key_equal. The difference is that
   //!   "equal_func" compares an arbitrary key with the contained values.
   //!
   //! <b>Effects</b>: Finds an iterator to the first element whose key is
   //!   "key" according to the given hash and equality functor or end() if
   //!   that element does not exist.
   //!
   //! <b>Complexity</b>: Average case O(1), worst case O(this->size()).
   //!
   //! <b>Throws</b>: If hash_func or equal_func throw.
   //!
   //! <b>Note</b>: This function is used when constructing a value_type
   //!   is expensive and the value_type can be compared with a cheaper
   //!   key type. Usually this key is part of the value_type.
   template<class KeyType, class KeyHasher, class KeyEqual>
   iterator find(const KeyType &key, KeyHasher hash_func, KeyEqual equal_func)
   {
      std::size_t bucket_n;
      std::size_t hash;
      siterator prev;
      return iterator( this->priv_find(key, hash_func, equal_func, bucket_n, hash, prev)
                     , &this->get_bucket_value_traits());
   }

   //! <b>Effects</b>: Finds a const_iterator to the first element whose key is
   //!   "key" or end() if that element does not exist.
   //!
   //! <b>Complexity</b>: Average case O(1), worst case O(this->size()).
   //!
   //! <b>Throws</b>: If the internal hasher or the equality functor throws.
   inline const_iterator find(const key_type &key) const
   {  return this->find(key, this->priv_hasher(), this->priv_equal());   }

   //! <b>Requires</b>: "hash_func" must be a hash function that induces
   //!   the same hash values as the stored hasher. The difference is that
   //!   "hash_func" hashes the given key instead of the value_type.
   //!
   //!   "equal_func" must be a equality function that induces
   //!   the same equality as key_equal. The difference is that
   //!   "equal_func" compares an arbitrary key with the contained values.
   //!
   //! <b>Effects</b>: Finds an iterator to the first element whose key is
   //!   "key" according to the given hasher and equality functor or end() if
   //!   that element does not exist.
   //!
   //! <b>Complexity</b>: Average case O(1), worst case O(this->size()).
   //!
   //! <b>Throws</b>: If hash_func or equal_func throw.
   //!
   //! <b>Note</b>: This function is used when constructing a value_type
   //!   is expensive and the value_type can be compared with a cheaper
   //!   key type. Usually this key is part of the value_type.
   template<class KeyType, class KeyHasher, class KeyEqual>
   const_iterator find
      (const KeyType &key, KeyHasher hash_func, KeyEqual equal_func) const
   {
      std::size_t bucket_n;
      std::size_t hash_value;
      siterator prev;
      return const_iterator( this->priv_find(key, hash_func, equal_func, bucket_n, hash_value, prev)
                           , &this->get_bucket_value_traits());
   }

   //! <b>Effects</b>: Returns a range containing all elements with values equivalent
   //!   to value. Returns std::make_pair(this->end(), this->end()) if no such
   //!   elements exist.
   //!
   //! <b>Complexity</b>: Average case O(this->count(value)). Worst case O(this->size()).
   //!
   //! <b>Throws</b>: If the internal hasher or the equality functor throws.
   inline std::pair<iterator,iterator> equal_range(const key_type &key)
   {  return this->equal_range(key, this->priv_hasher(), this->priv_equal());  }

   //! <b>Requires</b>: "hash_func" must be a hash function that induces
   //!   the same hash values as the stored hasher. The difference is that
   //!   "hash_func" hashes the given key instead of the value_type.
   //!
   //!   "equal_func" must be a equality function that induces
   //!   the same equality as key_equal. The difference is that
   //!   "equal_func" compares an arbitrary key with the contained values.
   //!
   //! <b>Effects</b>: Returns a range containing all elements with equivalent
   //!   keys. Returns std::make_pair(this->end(), this->end()) if no such
   //!   elements exist.
   //!
   //! <b>Complexity</b>: Average case O(this->count(key, hash_func, equal_func)).
   //!   Worst case O(this->size()).
   //!
   //! <b>Throws</b>: If hash_func or the equal_func throw.
   //!
   //! <b>Note</b>: This function is used when constructing a value_type
   //!   is expensive and the value_type can be compared with a cheaper
   //!   key type. Usually this key is part of the value_type.
   template<class KeyType, class KeyHasher, class KeyEqual>
   std::pair<iterator,iterator> equal_range
      (const KeyType &key, KeyHasher hash_func, KeyEqual equal_func)
   {
      auto [first, second] =
         this->priv_equal_range(key, hash_func, equal_func);
      return std::pair<iterator, iterator>
         ( iterator(first, &this->get_bucket_value_traits())
         , iterator(second, &this->get_bucket_value_traits()));
   }

   //! <b>Effects</b>: Returns a range containing all elements with values equivalent
   //!   to value. Returns std::make_pair(this->end(), this->end()) if no such
   //!   elements exist.
   //!
   //! <b>Complexity</b>: Average case O(this->count(value)). Worst case O(this->size()).
   //!
   //! <b>Throws</b>: If the internal hasher or the equality functor throws.
   inline std::pair<const_iterator, const_iterator>
      equal_range(const key_type &key) const
   {  return this->equal_range(key, this->priv_hasher(), this->priv_equal());  }

   //! <b>Requires</b>: "hash_func" must be a hash function that induces
   //!   the same hash values as the stored hasher. The difference is that
   //!   "hash_func" hashes the given key instead of the value_type.
   //!
   //!   "equal_func" must be a equality function that induces
   //!   the same equality as key_equal. The difference is that
   //!   "equal_func" compares an arbitrary key with the contained values.
   //!
   //! <b>Effects</b>: Returns a range containing all elements with equivalent
   //!   keys. Returns std::make_pair(this->end(), this->end()) if no such
   //!   elements exist.
   //!
   //! <b>Complexity</b>: Average case O(this->count(key, hash_func, equal_func)).
   //!   Worst case O(this->size()).
   //!
   //! <b>Throws</b>: If the hasher or equal_func throw.
   //!
   //! <b>Note</b>: This function is used when constructing a value_type
   //!   is expensive and the value_type can be compared with a cheaper
   //!   key type. Usually this key is part of the value_type.
   template<class KeyType, class KeyHasher, class KeyEqual>
   std::pair<const_iterator,const_iterator> equal_range
      (const KeyType &key, KeyHasher hash_func, KeyEqual equal_func) const
   {
      auto [first, second] = this->priv_equal_range(key, hash_func, equal_func);
      return std::pair<const_iterator, const_iterator>
         ( const_iterator(first,  &this->get_bucket_value_traits())
         , const_iterator(second, &this->get_bucket_value_traits()));
   }

   #if defined(BOOST_INTRUSIVE_DOXYGEN_INVOKED)

   //! <b>Requires</b>: value must be an lvalue and shall be in a unordered_set of
   //!   appropriate type. Otherwise the behavior is undefined.
   //!
   //! <b>Effects</b>: Returns: a valid iterator belonging to the unordered_set
   //!   that points to the value
   //!
   //! <b>Complexity</b>: Constant.
   //!
   //! <b>Throws</b>: If the internal hash function throws.
   iterator iterator_to(reference value) noexcept;

   //! <b>Requires</b>: value must be an lvalue and shall be in a unordered_set of
   //!   appropriate type. Otherwise the behavior is undefined.
   //!
   //! <b>Effects</b>: Returns: a valid const_iterator belonging to the
   //!   unordered_set that points to the value
   //!
   //! <b>Complexity</b>: Constant.
   //!
   //! <b>Throws</b>: If the internal hash function throws.
   const_iterator iterator_to(const_reference value) const noexcept;

   //! <b>Requires</b>: value must be an lvalue and shall be in a unordered_set of
   //!   appropriate type. Otherwise the behavior is undefined.
   //!
   //! <b>Effects</b>: Returns: a valid local_iterator belonging to the unordered_set
   //!   that points to the value
   //!
   //! <b>Complexity</b>: Constant.
   //!
   //! <b>Throws</b>: Nothing.
   //!
   //! <b>Note</b>: This static function is available only if the <i>value traits</i>
   //!   is stateless.
   static local_iterator s_local_iterator_to(reference value) noexcept;

   //! <b>Requires</b>: value must be an lvalue and shall be in a unordered_set of
   //!   appropriate type. Otherwise the behavior is undefined.
   //!
   //! <b>Effects</b>: Returns: a valid const_local_iterator belonging to
   //!   the unordered_set that points to the value
   //!
   //! <b>Complexity</b>: Constant.
   //!
   //! <b>Throws</b>: Nothing.
   //!
   //! <b>Note</b>: This static function is available only if the <i>value traits</i>
   //!   is stateless.
   static const_local_iterator s_local_iterator_to(const_reference value) noexcept;

   //! <b>Requires</b>: value must be an lvalue and shall be in a unordered_set of
   //!   appropriate type. Otherwise the behavior is undefined.
   //!
   //! <b>Effects</b>: Returns: a valid local_iterator belonging to the unordered_set
   //!   that points to the value
   //!
   //! <b>Complexity</b>: Constant.
   //!
   //! <b>Throws</b>: Nothing.
   local_iterator local_iterator_to(reference value) noexcept;

   //! <b>Requires</b>: value must be an lvalue and shall be in a unordered_set of
   //!   appropriate type. Otherwise the behavior is undefined.
   //!
   //! <b>Effects</b>: Returns: a valid const_local_iterator belonging to
   //!   the unordered_set that points to the value
   //!
   //! <b>Complexity</b>: Constant.
   //!
   //! <b>Throws</b>: Nothing.
   const_local_iterator local_iterator_to(const_reference value) const noexcept;

   //! <b>Effects</b>: Returns the number of buckets passed in the constructor
   //!   or the last rehash function.
   //!
   //! <b>Complexity</b>: Constant.
   //!
   //! <b>Throws</b>: Nothing.
   std::size_t bucket_count() const noexcept;

   //! <b>Requires</b>: n is in the range [0, this->bucket_count()).
   //!
   //! <b>Effects</b>: Returns the number of elements in the nth bucket.
   //!
   //! <b>Complexity</b>: Constant.
   //!
   //! <b>Throws</b>: Nothing.
   std::size_t bucket_size(std::size_t n) const noexcept;
   #endif  //#if defined(BOOST_INTRUSIVE_DOXYGEN_INVOKED)

   //! <b>Effects</b>: Returns the index of the bucket in which elements
   //!   with keys equivalent to k would be found, if any such element existed.
   //!
   //! <b>Complexity</b>: Constant.
   //!
   //! <b>Throws</b>: If the hash functor throws.
   //!
   //! <b>Note</b>: the return value is in the range [0, this->bucket_count()).
   inline std::size_t bucket(const key_type& k) const
   {  return this->bucket(k, this->priv_hasher());   }

   //! <b>Requires</b>: "hash_func" must be a hash function that induces
   //!   the same hash values as the stored hasher. The difference is that
   //!   "hash_func" hashes the given key instead of the value_type.
   //!
   //! <b>Effects</b>: Returns the index of the bucket in which elements
   //!   with keys equivalent to k would be found, if any such element existed.
   //!
   //! <b>Complexity</b>: Constant.
   //!
   //! <b>Throws</b>: If hash_func throws.
   //!
   //! <b>Note</b>: the return value is in the range [0, this->bucket_count()).
   template<class KeyType, class KeyHasher>
   inline std::size_t bucket(const KeyType& k, KeyHasher hash_func)  const
   {  return this->priv_hash_to_bucket(hash_func(k));   }

   #if defined(BOOST_INTRUSIVE_DOXYGEN_INVOKED)
   //! <b>Effects</b>: Returns the bucket array pointer passed in the constructor
   //!   or the last rehash function.
   //!
   //! <b>Complexity</b>: Constant.
   //!
   //! <b>Throws</b>: Nothing.
   bucket_ptr bucket_pointer() const noexcept;

   //! <b>Requires</b>: n is in the range [0, this->bucket_count()).
   //!
   //! <b>Effects</b>: Returns a local_iterator pointing to the beginning
   //!   of the sequence stored in the bucket n.
   //!
   //! <b>Complexity</b>: Constant.
   //!
   //! <b>Throws</b>: Nothing.
   //!
   //! <b>Note</b>:  [this->begin(n), this->end(n)) is a valid range
   //!   containing all of the elements in the nth bucket.
   local_iterator begin(std::size_t n) noexcept;

   //! <b>Requires</b>: n is in the range [0, this->bucket_count()).
   //!
   //! <b>Effects</b>: Returns a const_local_iterator pointing to the beginning
   //!   of the sequence stored in the bucket n.
   //!
   //! <b>Complexity</b>: Constant.
   //!
   //! <b>Throws</b>: Nothing.
   //!
   //! <b>Note</b>:  [this->begin(n), this->end(n)) is a valid range
   //!   containing all of the elements in the nth bucket.
   const_local_iterator begin(std::size_t n) const noexcept;

   //! <b>Requires</b>: n is in the range [0, this->bucket_count()).
   //!
   //! <b>Effects</b>: Returns a const_local_iterator pointing to the beginning
   //!   of the sequence stored in the bucket n.
   //!
   //! <b>Complexity</b>: Constant.
   //!
   //! <b>Throws</b>: Nothing.
   //!
   //! <b>Note</b>:  [this->begin(n), this->end(n)) is a valid range
   //!   containing all of the elements in the nth bucket.
   const_local_iterator cbegin(std::size_t n) const noexcept;

   //! <b>Requires</b>: n is in the range [0, this->bucket_count()).
   //!
   //! <b>Effects</b>: Returns a local_iterator pointing to the end
   //!   of the sequence stored in the bucket n.
   //!
   //! <b>Complexity</b>: Constant.
   //!
   //! <b>Throws</b>: Nothing.
   //!
   //! <b>Note</b>:  [this->begin(n), this->end(n)) is a valid range
   //!   containing all of the elements in the nth bucket.
   local_iterator end(std::size_t n) noexcept;

   //! <b>Requires</b>: n is in the range [0, this->bucket_count()).
   //!
   //! <b>Effects</b>: Returns a const_local_iterator pointing to the end
   //!   of the sequence stored in the bucket n.
   //!
   //! <b>Complexity</b>: Constant.
   //!
   //! <b>Throws</b>: Nothing.
   //!
   //! <b>Note</b>:  [this->begin(n), this->end(n)) is a valid range
   //!   containing all of the elements in the nth bucket.
   const_local_iterator end(std::size_t n) const noexcept;

   //! <b>Requires</b>: n is in the range [0, this->bucket_count()).
   //!
   //! <b>Effects</b>: Returns a const_local_iterator pointing to the end
   //!   of the sequence stored in the bucket n.
   //!
   //! <b>Complexity</b>: Constant.
   //!
   //! <b>Throws</b>: Nothing.
   //!
   //! <b>Note</b>:  [this->begin(n), this->end(n)) is a valid range
   //!   containing all of the elements in the nth bucket.
   const_local_iterator cend(std::size_t n) const noexcept;
   #endif   //#if defined(BOOST_INTRUSIVE_DOXYGEN_INVOKED)

   //! <b>Requires</b>: new_bucket_traits can hold a pointer to a new bucket array
   //!   or the same as the old bucket array with a different length. new_size is the length of the
   //!   the array pointed by new_buckets. If new_bucket_traits.bucket_begin() == this->bucket_pointer()
   //!   new_bucket_traits.bucket_count() can be bigger or smaller than this->bucket_count().
   //!   'new_bucket_traits' copy constructor should not throw.
   //!
   //! <b>Effects</b>:
   //!   If `new_bucket_traits.bucket_begin() == this->bucket_pointer()` is false,
   //!   unlinks values from the old bucket and inserts then in the new one according
   //!   to the hash value of values.
   //!
   //!   If `new_bucket_traits.bucket_begin() == this->bucket_pointer()` is true,
   //!   the implementations avoids moving values as much as possible.
   //!
   //!   Bucket traits hold by *this is assigned from new_bucket_traits.
   //!   If the container is configured as incremental<>, the split bucket is set
   //!   to the new bucket_count().
   //!
   //!   If store_hash option is true, this method does not use the hash function.
   //!   If false, the implementation tries to minimize calls to the hash function
   //!	 (e.g. once for equivalent values if optimize_multikey<true> is true).
   //!
   //!   If rehash is successful updates the internal bucket_traits with new_bucket_traits.
   //!
   //! <b>Complexity</b>: Average case linear in this->size(), worst case quadratic.
   //!
   //! <b>Throws</b>: If the hasher functor throws. Basic guarantee.
   inline void rehash(const bucket_traits &new_bucket_traits)
   {  this->rehash_impl(new_bucket_traits, false); }

   //! <b>Note</b>: This function is used when keys from inserted elements are changed 
   //!  (e.g. a language change when key is a string) but uniqueness and hash properties are
   //!  preserved so a fast full rehash recovers invariants for *this without extracting and
   //!  reinserting all elements again.
   //!
   //! <b>Requires</b>: Calls produced to the hash function should not alter the value uniqueness
   //!  properties of already inserted elements. If hasher(key1) == hasher(key2) was true when
   //!  elements were inserted, it shall be true during calls produced in the execution of this function.
   //!
   //!  key_equal is not called inside this function so it is assumed that key_equal(value1, value2)
   //!  should produce the same results as before for inserted elements.
   //!
   //! <b>Effects</b>: Reprocesses all values hold by *this, recalculating their hash values
   //!   and redistributing them though the buckets.
   //!
   //!   If store_hash option is true, this method uses the hash function and updates the stored hash value.
   //!
   //! <b>Complexity</b>: Average case linear in this->size(), worst case quadratic.
   //!
   //! <b>Throws</b>: If the hasher functor throws. Basic guarantee.
   inline void full_rehash()
   {  this->rehash_impl(this->priv_bucket_traits(), true);  }

   //! <b>Requires</b>:
   //!
   //! <b>Effects</b>:
   //!
   //! <b>Complexity</b>:
   //!
   //! <b>Throws</b>:
   //!
   //! <b>Note</b>: this method is only available if incremental<true> option is activated.
   bool incremental_rehash(bool grow = true)
   {
      //This function is only available for containers with incremental hashing
      static_assert(( incremental && power_2_buckets ));
      const std::size_t split_idx  = this->split_traits_.get_size();
      const std::size_t bucket_cnt = this->bucket_count();
      const bucket_ptr buck_ptr  = this->priv_bucket_pointer();
      bool ret = false;

      if(grow){
         //Test if the split variable can be changed
         if((ret = split_idx < bucket_cnt)){
            const std::size_t bucket_to_rehash = split_idx - bucket_cnt/2;
            bucket_type &old_bucket = buck_ptr[bucket_to_rehash];
            this->split_traits_.increment();

            //Anti-exception stuff: if an exception is thrown while
            //moving elements from old_bucket to the target bucket, all moved
            //elements are moved back to the original one.
            detail::incremental_rehash_rollback<bucket_type, split_traits> rollback
               ( buck_ptr[split_idx], old_bucket, this->split_traits_);
            for( siterator before_i(old_bucket.before_begin()), i(old_bucket.begin()), end_sit(old_bucket.end())
               ; i != end_sit; i = before_i, ++i){
               const value_type &v = this->priv_value_from_slist_node(i.pointed_node());
               const std::size_t hash_value = this->priv_stored_or_compute_hash(v, store_hash_t());
               const std::size_t new_n = this->priv_hash_to_bucket(hash_value);
               siterator const last = (priv_last_in_group)(i);
               if(new_n == bucket_to_rehash){
                  before_i = last;
               }
               else{
                  bucket_type &new_b = buck_ptr[new_n];
                  new_b.splice_after(new_b.before_begin(), old_bucket, before_i, last);
               }
            }
            rollback.release();
            this->priv_erasure_update_cache();
         }
      }
      else if((ret = split_idx > bucket_cnt/2)){   //!grow
         const std::size_t target_bucket_num = split_idx - 1 - bucket_cnt/2;
         bucket_type &target_bucket = buck_ptr[target_bucket_num];
         bucket_type &source_bucket = buck_ptr[split_idx-1];
         target_bucket.splice_after(target_bucket.cbefore_begin(), source_bucket);
         this->split_traits_.decrement();
         this->priv_insertion_update_cache(target_bucket_num);
      }
      return ret;
   }

   //! <b>Effects</b>: If new_bucket_traits.bucket_count() is not
   //!   this->bucket_count()/2 or this->bucket_count()*2, or
   //!   this->split_bucket() != new_bucket_traits.bucket_count() returns false
   //!   and does nothing.
   //!
   //!   Otherwise, copy assigns new_bucket_traits to the internal bucket_traits
   //!   and transfers all the objects from old buckets to the new ones.
   //!
   //! <b>Complexity</b>: Linear to size().
   //!
   //! <b>Throws</b>: Nothing
   //!
   //! <b>Note</b>: this method is only available if incremental<true> option is activated.
   bool incremental_rehash(const bucket_traits &new_bucket_traits) noexcept
   {
      //This function is only available for containers with incremental hashing
      static_assert(( incremental && power_2_buckets ));
      std::size_t const new_bucket_traits_size = new_bucket_traits.bucket_count();
      std::size_t const cur_bucket_traits      = this->bucket_count();
      const std::size_t split_idx = this->split_count();

      //Test new bucket size is consistent with internal bucket size and split count
      if(new_bucket_traits_size/2 == cur_bucket_traits){
         if(!(split_idx >= cur_bucket_traits))
            return false;
      }
      else if(new_bucket_traits_size == cur_bucket_traits/2){
         if(!(split_idx <= new_bucket_traits_size))
            return false;
      }
      else{
         return false;
      }

      const std::size_t ini_n = this->priv_get_cache_bucket_num();
      const bucket_ptr old_buckets = this->priv_bucket_pointer();
      this->priv_bucket_traits() = new_bucket_traits;
      if(new_bucket_traits.bucket_begin() != old_buckets){
         for(std::size_t n = ini_n; n < split_idx; ++n){
            bucket_type &new_bucket = new_bucket_traits.bucket_begin()[n];
            bucket_type &old_bucket = old_buckets[n];
            new_bucket.splice_after(new_bucket.cbefore_begin(), old_bucket);
         }
         //Put cache to safe position
         this->priv_initialize_cache();
         this->priv_insertion_update_cache(ini_n);
      }
      return true;
   }

   #if defined(BOOST_INTRUSIVE_DOXYGEN_INVOKED)

   //! <b>Requires</b>: incremental<> option must be set
   //!
   //! <b>Effects</b>: returns the current split count
   //!
   //! <b>Complexity</b>: Constant
   //!
   //! <b>Throws</b>: Nothing
   std::size_t split_count() const noexcept;

   //! <b>Effects</b>: Returns the nearest new bucket count optimized for
   //!   the container that is bigger or equal than n. This suggestion can be
   //!   used to create bucket arrays with a size that will usually improve
   //!   container's performance. If such value does not exist, the
   //!   higher possible value is returned.
   //!
   //! <b>Complexity</b>: Amortized constant time.
   //!
   //! <b>Throws</b>: Nothing.
   static std::size_t suggested_upper_bucket_count(std::size_t n) noexcept;

   //! <b>Effects</b>: Returns the nearest new bucket count optimized for
   //!   the container that is smaller or equal than n. This suggestion can be
   //!   used to create bucket arrays with a size that will usually improve
   //!   container's performance. If such value does not exist, the
   //!   lowest possible value is returned.
   //!
   //! <b>Complexity</b>: Amortized constant time.
   //!
   //! <b>Throws</b>: Nothing.
   static std::size_t suggested_lower_bucket_count(std::size_t n) noexcept;
   #endif   //#if defined(BOOST_INTRUSIVE_DOXYGEN_INVOKED)


   friend bool operator==(const hashtable_impl &x, const hashtable_impl &y)
   {
      //Taken from N3068
      if(constant_time_size && x.size() != y.size()){
         return false;
      }
      for (const_iterator ix = x.cbegin(), ex = x.cend(); ix != ex; ++ix){
         std::pair<const_iterator, const_iterator> eqx(x.equal_range(key_of_value()(*ix))),
                                                   eqy(y.equal_range(key_of_value()(*ix)));
         if (boost::intrusive::iterator_distance(eqx.first, eqx.second) !=
             boost::intrusive::iterator_distance(eqy.first, eqy.second) ||
               !(std::is_permutation)(eqx.first, eqx.second, eqy.first)      ){
            return false;
         }
         ix = eqx.second;
      }
      return true;
   }

   friend bool operator!=(const hashtable_impl &x, const hashtable_impl &y)
   {  return !(x == y); }

   friend bool operator<(const hashtable_impl &x, const hashtable_impl &y)
   {  return std::lexicographical_compare(x.begin(), x.end(), y.begin(), y.end());  }

   friend bool operator>(const hashtable_impl &x, const hashtable_impl &y)
   {  return y < x;  }

   friend bool operator<=(const hashtable_impl &x, const hashtable_impl &y)
   {  return !(y < x);  }

   friend bool operator>=(const hashtable_impl &x, const hashtable_impl &y)
   {  return !(x < y);  }

   /// @cond
   inline void check() const {}
   private:

   void rehash_impl(const bucket_traits &new_bucket_traits, bool do_full_rehash)
   {
      const bucket_ptr new_buckets      = new_bucket_traits.bucket_begin();
            std::size_t  new_bucket_count = new_bucket_traits.bucket_count();
      const bucket_ptr old_buckets      = this->priv_bucket_pointer();
            std::size_t  old_bucket_count = this->bucket_count();

      //Check power of two bucket array if the option is activated
     BOOST_INTRUSIVE_INVARIANT_ASSERT
         (!power_2_buckets || (0 == (new_bucket_count & (new_bucket_count-1u))));

      std::size_t n = this->priv_get_cache_bucket_num();
      const bool same_buffer = old_buckets == new_buckets;
      //If the new bucket length is a common factor
      //of the old one we can avoid hash calculations.
      const bool fast_shrink = (!do_full_rehash) && (!incremental) && (old_bucket_count >= new_bucket_count) &&
         (power_2_buckets || (old_bucket_count % new_bucket_count) == 0);
      //If we are shrinking the same bucket array and it's
      //is a fast shrink, just rehash the last nodes
      std::size_t new_first_bucket_num = new_bucket_count;
      if(same_buffer && fast_shrink && (n < new_bucket_count)){
         new_first_bucket_num = n;
         n = new_bucket_count;
      }

      //Anti-exception stuff: they destroy the elements if something goes wrong.
      //If the source and destination buckets are the same, the second rollback function
      //is harmless, because all elements have been already unlinked and destroyed
      typedef detail::init_disposer<node_algorithms> NodeDisposer;
      typedef detail::exception_array_disposer<bucket_type, NodeDisposer> ArrayDisposer;
      NodeDisposer node_disp;
      ArrayDisposer rollback1(new_buckets[0], node_disp, new_bucket_count);
      ArrayDisposer rollback2(old_buckets[0], node_disp, old_bucket_count);

      //Put size in a safe value for rollback exception
      std::size_t const size_backup = this->size_traits_.get_size();
      this->size_traits_.set_size(0);
      //Put cache to safe position
      this->priv_initialize_cache();
      this->priv_insertion_update_cache(0);

      //Iterate through nodes
      for(; n < old_bucket_count; ++n){
         bucket_type &old_bucket = old_buckets[n];
         if(!fast_shrink){
            for( siterator before_i(old_bucket.before_begin()), i(old_bucket.begin()), end_sit(old_bucket.end())
               ; i != end_sit
               ; i = before_i, ++i){

               //First obtain hash value (and store it if do_full_rehash)
               std::size_t hash_value;
               if(do_full_rehash){
                  value_type &v = this->priv_value_from_slist_node(i.pointed_node());
                  hash_value = this->priv_hasher()(key_of_value()(v));
                  if constexpr (store_hash_t::value) 
                      node_functions_t::store_hash(pointer_traits<node_ptr>::pointer_to(this->priv_value_to_node(v)), hash_value);
               }
               else{
                  const value_type &v = this->priv_value_from_slist_node(i.pointed_node());
                  hash_value = this->priv_stored_or_compute_hash(v, store_hash_t());
               }

               //Now calculate the new bucket position
               const std::size_t new_n = detail::hash_to_bucket_split<power_2_buckets, incremental>
                  (hash_value, new_bucket_count, new_bucket_count);

               //Update first used bucket cache
               if(cache_begin && new_n < new_first_bucket_num)
                  new_first_bucket_num = new_n;

               //If the target bucket is new, transfer the whole group
               siterator const last = (priv_last_in_group)(i);

               if(same_buffer && new_n == n){
                  before_i = last;
               }
               else{
                  bucket_type &new_b = new_buckets[new_n];
                  new_b.splice_after(new_b.before_begin(), old_bucket, before_i, last);
               }
            }
         }
         else{
            const std::size_t new_n = detail::hash_to_bucket_split<power_2_buckets, incremental>(n, new_bucket_count, new_bucket_count);
            if(cache_begin && new_n < new_first_bucket_num)
               new_first_bucket_num = new_n;
            bucket_type &new_b = new_buckets[new_n];
            new_b.splice_after( new_b.before_begin()
                              , old_bucket
                              , old_bucket.before_begin()
                              , bucket_plus_vtraits_t::template priv_get_last<optimize_multikey>(old_bucket));
         }
      }

      this->size_traits_.set_size(size_backup);
      this->split_traits_.set_size(new_bucket_count);
      if(&new_bucket_traits != &this->priv_bucket_traits()){
         this->priv_bucket_traits() = new_bucket_traits;
      }
      this->priv_initialize_cache();
      this->priv_insertion_update_cache(new_first_bucket_num);
      rollback1.release();
      rollback2.release();
   }

   template <class MaybeConstHashtableImpl, class Cloner, class Disposer>
   void priv_clone_from(MaybeConstHashtableImpl &src, Cloner cloner, Disposer disposer)
   {
      this->clear_and_dispose(disposer);
      if(!constant_time_size || !src.empty()){
         const std::size_t src_bucket_count = src.bucket_count();
         const std::size_t dst_bucket_count = this->bucket_count();
         //Check power of two bucket array if the option is activated
        BOOST_INTRUSIVE_INVARIANT_ASSERT
            (!power_2_buckets || (0 == (src_bucket_count & (src_bucket_count-1))));
        BOOST_INTRUSIVE_INVARIANT_ASSERT
            (!power_2_buckets || (0 == (dst_bucket_count & (dst_bucket_count-1))));
         //If src bucket count is bigger or equal, structural copy is possible
         const bool structural_copy = (!incremental) && (src_bucket_count >= dst_bucket_count) &&
            (power_2_buckets || (src_bucket_count % dst_bucket_count) == 0);
         if(structural_copy){
            this->priv_structural_clone_from(src, cloner, disposer);
         }
         else{
            //Unlike previous cloning algorithm, this can throw
            //if cloner, hasher or comparison functor throw
            using clone_iterator = std::conditional_t<std::is_const_v<MaybeConstHashtableImpl>
                 , typename MaybeConstHashtableImpl::const_iterator
                 , typename MaybeConstHashtableImpl::iterator
            >;
            clone_iterator b(src.begin()), e(src.end());
            detail::exception_disposer<hashtable_impl, Disposer> rollback(*this, disposer);
            for(; b != e; ++b){
               //No need to check for duplicates and insert it in the first position
               //as this is an unordered container. So use minimal insertion code
               std::size_t const hash_to_store = this->priv_stored_or_compute_hash(*b, store_hash_t());;
               std::size_t const bucket_number = this->priv_hash_to_bucket(hash_to_store);
               using reference_type = std::conditional_t
                  <std::is_const_v<MaybeConstHashtableImpl>, const_reference, reference>;
               reference_type r = *b;
               this->priv_clone_front_in_bucket<reference_type>(bucket_number, r, hash_to_store, cloner);
            }
            rollback.release();
         }
      }
   }

   template<class ValueReference, class Cloner>
   void priv_clone_front_in_bucket( std::size_t const bucket_number
                                  , typename detail::identity<ValueReference>::type src_ref
                                  , std::size_t const hash_to_store, Cloner cloner)
   {
      //No need to check for duplicates and insert it in the first position
      //as this is an unordered container. So use minimal insertion code
      //std::size_t const hash_value = this->priv_stored_or_compute_hash(src_ref, store_hash_t());;
      //std::size_t const bucket_number = this->priv_hash_to_bucket(hash_value);
      bucket_type &cur_bucket = this->priv_bucket_pointer()[bucket_number];
      siterator const prev(cur_bucket.before_begin());
      //Just check if the cloned node is equal to the first inserted value in the new bucket
      //as equal src values were contiguous and they should be already inserted in the
      //destination bucket.
      bool const next_is_in_group = optimize_multikey && !cur_bucket.empty() &&
         this->priv_equal()( key_of_value()(src_ref)
                           , key_of_value()(this->priv_value_from_slist_node((++siterator(prev)).pointed_node())));
      this->priv_insert_equal_after_find(*cloner(src_ref), bucket_number, hash_to_store, prev, next_is_in_group);
   }

   template <class MaybeConstHashtableImpl, class Cloner, class Disposer>
   void priv_structural_clone_from(MaybeConstHashtableImpl &src, Cloner cloner, Disposer disposer)
   {
      //First clone the first ones
      const std::size_t src_bucket_count = src.bucket_count();
      const std::size_t dst_bucket_count = this->bucket_count();
      const bucket_ptr src_buckets = src.priv_bucket_pointer();
      const bucket_ptr dst_buckets = this->priv_bucket_pointer();
      std::size_t constructed = 0;
      typedef node_cast_adaptor< detail::node_disposer<Disposer, value_traits, CircularSListAlgorithms>
                                 , slist_node_ptr, node_ptr > NodeDisposer;
      NodeDisposer node_disp(disposer, &this->priv_value_traits());

      detail::exception_array_disposer<bucket_type, NodeDisposer>
         rollback(dst_buckets[0], node_disp, constructed);
      //Now insert the remaining ones using the modulo trick
      for( //"constructed" already initialized
         ; constructed < src_bucket_count
         ; ++constructed){
         //Since incremental hashing can't be structurally copied, avoid hash_to_bucket_split
         const std::size_t new_n = detail::hash_to_bucket(constructed, dst_bucket_count, std::bool_constant<power_2_buckets>());
         bucket_type &src_b = src_buckets[constructed];
         for( siterator b(src_b.begin()), e(src_b.end()); b != e; ++b){
            slist_node_ptr const n(b.pointed_node());
            using reference_type = std::conditional_t
                <std::is_const_v<MaybeConstHashtableImpl>, const_reference, reference>;
            reference_type r = this->priv_value_from_slist_node(n);
            this->priv_clone_front_in_bucket<reference_type>
               (new_n, r, this->template priv_stored_hash<store_hash_t::value>(n), cloner);
         }
      }
      this->priv_hasher() = src.priv_hasher();
      this->priv_equal()  = src.priv_equal();
      rollback.release();
      this->size_traits_.set_size(src.size_traits_.get_size());
      this->split_traits_.set_size(dst_bucket_count);
      this->priv_insertion_update_cache(0u);
      this->priv_erasure_update_cache();
   }

   std::size_t priv_hash_to_bucket(std::size_t hash_value) const
   {
      return detail::hash_to_bucket_split<power_2_buckets, incremental>
         (hash_value, this->priv_bucket_traits().bucket_count(), this->split_traits_.get_size());
   }

   iterator priv_insert_equal_after_find(reference value, std::size_t bucket_num, std::size_t hash_value, siterator prev, bool const next_is_in_group)
   {
      //Now store hash if needed
      node_ptr n = pointer_traits<node_ptr>::pointer_to(this->priv_value_to_node(value));
      if constexpr (store_hash_t::value) node_functions_t::store_hash(n, hash_value);
      //Checks for some modes
      BOOST_INTRUSIVE_SAFE_HOOK_DEFAULT_ASSERT(!safemode_or_autounlink || node_algorithms::unique(n));
      //Shortcut to optimize_multikey cases
      if constexpr (optimize_multikey) {
          group_functions_t::insert_in_group
          (next_is_in_group ? detail::dcast_bucket_ptr<node>((++siterator(prev)).pointed_node()) : n
              , n);
      }
      //Update cache and increment size if needed
      this->priv_insertion_update_cache(bucket_num);
      this->size_traits_.increment();
      //Insert the element in the bucket after it
      return iterator(bucket_type::s_insert_after(prev, *n), &this->get_bucket_value_traits());
   }

   template<class KeyType, class KeyHasher, class KeyEqual>
   siterator priv_find  //In case it is not found previt is bucket.before_begin()
      ( const KeyType &key,  KeyHasher hash_func
      , KeyEqual equal_func, std::size_t &bucket_number, std::size_t &h, siterator &previt) const
   {
      h = hash_func(key);
      return this->priv_find_with_hash(key, equal_func, bucket_number, h, previt);
   }

   template<class KeyType, class KeyEqual>
   bool priv_is_value_equal_to_key(const value_type &v, const std::size_t h, const KeyType &key, KeyEqual equal_func) const
   {
      (void)h;
      return (!compare_hash || this->priv_stored_or_compute_hash(v, store_hash_t()) == h) && equal_func(key, key_of_value()(v));
   }

   //return previous iterator to the next equal range group in case
   static siterator priv_last_in_group(const siterator &it_first_in_group) noexcept
   {
      return bucket_type::s_iterator_to
         (*group_functions_t::template get_last_in_group<optimize_multikey>
            (detail::dcast_bucket_ptr<node>(it_first_in_group.pointed_node())));
   }

   template<class KeyType, class KeyEqual>
   siterator priv_find_with_hash //In case it is not found previt is bucket.before_begin()
      ( const KeyType &key, KeyEqual equal_func, std::size_t &bucket_number, const std::size_t h, siterator &previt) const
   {
      bucket_number = this->priv_hash_to_bucket(h);
      bucket_type &b = this->priv_bucket_pointer()[bucket_number];
      previt = b.before_begin();
      siterator it = previt;
      siterator const endit = b.end();

      while(++it != endit){
         if(this->priv_is_value_equal_to_key(this->priv_value_from_slist_node(it.pointed_node()), h, key, equal_func)){
            return it;
         }
         previt = it = (priv_last_in_group)(it);
      }
      previt = b.before_begin();
      return this->priv_invalid_local_it();
   }

   template<class KeyType, class KeyHasher, class KeyEqual>
   std::pair<siterator, siterator> priv_local_equal_range
      ( const KeyType &key
      , KeyHasher hash_func
      , KeyEqual equal_func
      , std::size_t&found_bucket
      , std::size_t&cnt) const
   {
      std::size_t internal_cnt = 0;
      //Let's see if the element is present
      
      siterator prev;
      std::size_t n_bucket;
      std::size_t h;
      std::pair<siterator, siterator> to_return
         ( this->priv_find(key, hash_func, equal_func, n_bucket, h, prev)
         , this->priv_invalid_local_it());

      if(to_return.first != to_return.second){
         found_bucket = n_bucket;
         //If it's present, find the first that it's not equal in
         //the same bucket
         bucket_type &b = this->priv_bucket_pointer()[n_bucket];
         siterator it = to_return.first;
         ++internal_cnt;   //At least one is found
         if constexpr (optimize_multikey) {
            to_return.second = ++(priv_last_in_group)(it);
            internal_cnt += boost::intrusive::iterator_distance(++it, to_return.second);
         }
         else{
            siterator const bend = b.end();
            while(++it != bend &&
                  this->priv_is_value_equal_to_key(this->priv_value_from_slist_node(it.pointed_node()), h, key, equal_func)){
               ++internal_cnt;
            }
            to_return.second = it;
         }
      }
      cnt = internal_cnt;
      return to_return;
   }

   template<class KeyType, class KeyHasher, class KeyEqual>
   std::pair<siterator, siterator> priv_equal_range
      ( const KeyType &key
      , KeyHasher hash_func
      , KeyEqual equal_func) const
   {
      std::size_t n_bucket;
      std::size_t cnt;

      //Let's see if the element is present
      std::pair<siterator, siterator> to_return
         (this->priv_local_equal_range(key, hash_func, equal_func, n_bucket, cnt));
      //If not, find the next element as ".second" if ".second" local iterator
      //is not pointing to an element.
      bucket_ptr const bp = this->priv_bucket_pointer();
      if(to_return.first != to_return.second &&
         to_return.second == bp[n_bucket].end()){
         to_return.second = this->priv_invalid_local_it();
         ++n_bucket;
         for( const std::size_t max_bucket = this->bucket_count()
            ; n_bucket != max_bucket
            ; ++n_bucket){
            bucket_type &b = bp[n_bucket];
            if(!b.empty()){
               to_return.second = b.begin();
               break;
            }
         }
      }
      return to_return;
   }

   std::size_t priv_get_bucket_num(siterator it) noexcept
   {  return this->priv_get_bucket_num_hash_dispatch(it, store_hash_t());  }

   std::size_t priv_get_bucket_num_hash_dispatch(siterator it, std::true_type) noexcept    //store_hash
   {
      return this->priv_hash_to_bucket
         (this->template priv_stored_hash<store_hash_t::value>(it.pointed_node()));
   }

   std::size_t priv_get_bucket_num_hash_dispatch(siterator it, std::false_type) noexcept   //NO store_hash
   {  return this->template priv_get_bucket_num_no_hash_store<optimize_multikey>(it);  }

   static siterator priv_get_previous(bucket_type &b, siterator i) noexcept
   {  return bucket_plus_vtraits_t::template priv_get_previous<optimize_multikey>(b, i);   }

   /// @endcond
};

/// @cond
template <typename T, bool UniqueKeys, typename PackedOptions>
struct make_bucket_traits
{
   //Real value traits must be calculated from options
   using value_traits = detail::get_value_traits_t<T, typename PackedOptions::proto_value_traits>;

   typedef typename PackedOptions::bucket_traits            specified_bucket_traits;

   //Real bucket traits must be calculated from options and calculated value_traits
   using slist_impl = get_slist_impl_t<reduced_slist_node_traits_t<typename value_traits::node_traits>>;

   using type = std::conditional_t< std::is_same_v
       < specified_bucket_traits
       , default_bucket_traits>
       , bucket_traits_impl<slist_impl>
       , specified_bucket_traits
   >;
};

template <typename T, bool UniqueKeys, class PackedOptions>
using make_bucket_traits_t = typename make_bucket_traits<T, UniqueKeys, PackedOptions>::type;

/// @endcond

//! Helper metafunction to define a \c hashtable that yields to the same type when the
//! same options (either explicitly or implicitly) are used.
template<class T, class ...Options>
struct make_hashtable
{
   /// @cond
   typedef typename pack_options
      < hashtable_defaults,
         Options...
      >::type packed_options;

   using value_traits = detail::get_value_traits_t<T, typename packed_options::proto_value_traits>;

   using bucket_traits = make_bucket_traits_t<T, false, packed_options>;

   typedef hashtable_impl
      < value_traits
      , typename packed_options::key_of_value
      , typename packed_options::hash
      , typename packed_options::equal
      , bucket_traits
      ,  (std::size_t(false)*hash_bool_flags::unique_keys_pos)
        |(std::size_t(packed_options::constant_time_size)*hash_bool_flags::constant_time_size_pos)
        |(std::size_t(packed_options::power_2_buckets)*hash_bool_flags::power_2_buckets_pos)
        |(std::size_t(packed_options::cache_begin)*hash_bool_flags::cache_begin_pos)
        |(std::size_t(packed_options::compare_hash)*hash_bool_flags::compare_hash_pos)
        |(std::size_t(packed_options::incremental)*hash_bool_flags::incremental_pos)
      > implementation_defined;

   /// @endcond
   typedef implementation_defined type;
};

#if !defined(BOOST_INTRUSIVE_DOXYGEN_INVOKED)

template<class T, class ...Options>
class hashtable
   :  public make_hashtable<T,
         Options...
         >::type
{
   typedef typename make_hashtable<T,
      Options...
      >::type   Base;

   public:
   typedef typename Base::value_traits       value_traits;
   typedef typename Base::iterator           iterator;
   typedef typename Base::const_iterator     const_iterator;
   typedef typename Base::bucket_ptr         bucket_ptr;
   typedef typename Base::hasher             hasher;
   typedef typename Base::bucket_traits      bucket_traits;
   typedef typename Base::key_equal          key_equal;

   //Assert if passed value traits are compatible with the type
   static_assert(std::is_same_v<typename value_traits::value_type, T>);

   inline explicit hashtable ( const bucket_traits &b_traits
             , const hasher & hash_func = hasher()
             , const key_equal &equal_func = key_equal()
             , const value_traits &v_traits = value_traits())
      :  Base(b_traits, hash_func, equal_func, v_traits)
   {}
   hashtable(const hashtable&) = delete;
   hashtable& operator=(const hashtable&) = delete;
   inline hashtable(hashtable&& x) : Base(std::move(x)) {}

   inline hashtable& operator=(hashtable&& x)
   {  return static_cast<hashtable&>(this->Base::operator=(std::move(x)));  }

   template <class Cloner, class Disposer>
   inline void clone_from(hashtable&& src, Cloner cloner, Disposer disposer)
   {  Base::clone_from(std::forward<hashtable>(src), cloner, disposer);  }
};

#endif

} //namespace boost::intrusive

#endif //BOOST_INTRUSIVE_HASHTABLE_HPP
