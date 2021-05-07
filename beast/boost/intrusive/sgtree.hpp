/////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2007-2014
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/intrusive for documentation.
//
/////////////////////////////////////////////////////////////////////////////
//
// The option that yields to non-floating point 1/sqrt(2) alpha is taken
// from the scapegoat tree implementation of the PSPP library.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef BOOST_INTRUSIVE_SGTREE_HPP
#define BOOST_INTRUSIVE_SGTREE_HPP

#include <boost/intrusive/intrusive_fwd.hpp>
#include <boost/intrusive/detail/assert.hpp>
#include <boost/intrusive/bs_set_hook.hpp>
#include <boost/intrusive/bstree.hpp>
#include <boost/intrusive/detail/tree_node.hpp>
#include <boost/intrusive/pointer_traits.hpp>
#include <boost/intrusive/detail/mpl.hpp>
#include <boost/intrusive/detail/math.hpp>
#include <boost/intrusive/detail/get_value_traits.hpp>
#include <boost/intrusive/sgtree_algorithms.hpp>
#include <boost/intrusive/detail/key_nodeptr_comp.hpp>
#include <boost/intrusive/link_mode.hpp>

#include <cstddef>

#if defined(BOOST_HAS_PRAGMA_ONCE)
#  pragma once
#endif

namespace boost {
namespace intrusive {

/// @cond

namespace detail{

/////////////////////////////////////////////////////////////
//
//       Halpha for fixed floating_point<false> option
//
/////////////////////////////////////////////////////////////

//! Returns floor(log2(n)/log2(sqrt(2))) -> floor(2*log2(n))
//! Undefined if N is 0.
//!
//! This function does not use float point operations.
inline std::size_t calculate_h_sqrt2 (std::size_t n)
{
   std::size_t f_log2 = detail::floor_log2(n);
   return (2*f_log2) + static_cast<std::size_t>(n >= detail::sqrt2_pow_2xplus1(f_log2));
}

struct h_alpha_sqrt2_t
{
   h_alpha_sqrt2_t(void){}
   std::size_t operator()(std::size_t n) const
   {  return calculate_h_sqrt2(n);  }
};

struct alpha_0_75_by_max_size_t
{
   alpha_0_75_by_max_size_t(void){}

   std::size_t operator()(std::size_t max_tree_size) const
   {
      const std::size_t max_tree_size_limit = ((~std::size_t(0))/std::size_t(3));
      return max_tree_size > max_tree_size_limit ? max_tree_size/4*3 : max_tree_size*3/4;
   }
};

/////////////////////////////////////////////////////////////
//
//       Halpha for fixed floating_point<true> option
//
/////////////////////////////////////////////////////////////

struct h_alpha_t
{
   explicit h_alpha_t(float inv_minus_logalpha)
      :  inv_minus_logalpha_(inv_minus_logalpha)
   {}

   std::size_t operator()(std::size_t n) const
   {
      ////////////////////////////////////////////////////////////
      // This function must return "floor(log2(1/alpha(n)))" ->
      //    floor(log2(n)/log(1/alpha)) ->
      //    floor(log2(n)/-log2(alpha))
      //    floor(log2(n)*(1/-log2(alpha)))
      ////////////////////////////////////////////////////////////
      return static_cast<std::size_t>(detail::fast_log2(float(n))*inv_minus_logalpha_);
   }

   private:
   //Since the function will be repeatedly called
   //precalculate constant data to avoid repeated
   //calls to log and division.
   //This will store 1/(-std::log2(alpha_))
   float inv_minus_logalpha_;
};

struct alpha_by_max_size_t
{
   explicit alpha_by_max_size_t(float alpha)
      :  alpha_(alpha)
   {}

   float operator()(std::size_t max_tree_size) const
   {  return float(max_tree_size)*alpha_;   }

   private:
   float alpha_;
};

struct alpha_holder
{
   typedef boost::intrusive::detail::h_alpha_t           h_alpha_t;
   typedef boost::intrusive::detail::alpha_by_max_size_t multiply_by_alpha_t;

   alpha_holder()
      : max_tree_size_()
   {  set_alpha(0.70711f);   } // ~1/sqrt(2)

   float get_alpha() const
   {  return alpha_;  }

   void set_alpha(float alpha)
   {
      alpha_ = alpha;
      inv_minus_logalpha_ = 1/(-detail::fast_log2(alpha));
   }

   h_alpha_t get_h_alpha_t() const
   {  return h_alpha_t(inv_minus_logalpha_);  }

   multiply_by_alpha_t get_multiply_by_alpha_t() const
   {  return multiply_by_alpha_t(alpha_);  }

   std::size_t &get_max_tree_size()
   {  return max_tree_size_;  }

   protected:
   float alpha_;
   float inv_minus_logalpha_;
   std::size_t max_tree_size_;
};

}  //namespace detail{

using sgtree_defaults = bstree_defaults;

/// @endcond

//! The class template sgtree is an intrusive scapegoat tree container, that
//! is used to construct intrusive sg_set and sg_multiset containers.
//! The no-throw guarantee holds only, if the value_compare object
//! doesn't throw.
//!
//! The template parameter \c T is the type to be managed by the container.
//! The user can specify additional options and if no options are provided
//! default options are used.
//!
//! The container supports the following options:
//! \c base_hook<>/member_hook<>/value_traits<>,
//! \c floating_point<>, and
//! \c compare<>.
#if defined(BOOST_INTRUSIVE_DOXYGEN_INVOKED)
template<class T, class ...Options>
#else
template<class ValueTraits, class VoidOrKeyOfValue, class VoidOrKeyComp, typename HeaderHolder>
#endif
class sgtree_impl
   /// @cond
   :  public bstree_impl<ValueTraits, VoidOrKeyOfValue, VoidOrKeyComp, true, SgTreeAlgorithms, HeaderHolder>
   ,  public detail::alpha_holder
   /// @endcond
{
   public:
   typedef ValueTraits                                               value_traits;
   /// @cond
   typedef bstree_impl< ValueTraits, VoidOrKeyOfValue, VoidOrKeyComp
                      , true, SgTreeAlgorithms, HeaderHolder>        tree_type;
   typedef tree_type                                                 implementation_defined;

   /// @endcond

   typedef typename implementation_defined::pointer                  pointer;
   typedef typename implementation_defined::const_pointer            const_pointer;
   typedef typename implementation_defined::value_type               value_type;
   typedef typename implementation_defined::key_type                 key_type;
   typedef typename implementation_defined::key_of_value             key_of_value;
   typedef typename implementation_defined::reference                reference;
   typedef typename implementation_defined::const_reference          const_reference;
   typedef typename implementation_defined::difference_type          difference_type;
   typedef typename implementation_defined::value_compare            value_compare;
   typedef typename implementation_defined::key_compare              key_compare;
   typedef typename implementation_defined::iterator                 iterator;
   typedef typename implementation_defined::const_iterator           const_iterator;
   typedef typename implementation_defined::reverse_iterator         reverse_iterator;
   typedef typename implementation_defined::const_reverse_iterator   const_reverse_iterator;
   typedef typename implementation_defined::node_traits              node_traits;
   typedef typename implementation_defined::node                     node;
   typedef typename implementation_defined::node_ptr                 node_ptr;
   typedef typename implementation_defined::const_node_ptr           const_node_ptr;
   typedef sgtree_algorithms<node_traits>                            node_algorithms;

   static const bool constant_time_size      = implementation_defined::constant_time_size;
   static const bool stateful_value_traits   = implementation_defined::stateful_value_traits;

   /// @cond
   private:

   //noncopyable
   using alpha_traits = detail::alpha_holder;
   typedef typename alpha_traits::h_alpha_t                 h_alpha_t;
   typedef typename alpha_traits::multiply_by_alpha_t       multiply_by_alpha_t;

   static_assert(value_traits::link_mode != link_mode_type::auto_unlink);

   enum { safemode_or_autounlink  =
            value_traits::link_mode == link_mode_type::auto_unlink   ||
            value_traits::link_mode == link_mode_type::safe_link     };

   /// @endcond

   public:

   typedef typename node_algorithms::insert_commit_data insert_commit_data;

   //! @copydoc ::boost::intrusive::bstree::bstree()
   sgtree_impl()
      :  tree_type()
   {}

   //! @copydoc ::boost::intrusive::bstree::bstree(const key_compare &,const value_traits &)
   explicit sgtree_impl( const key_compare &cmp, const value_traits &v_traits = value_traits())
      :  tree_type(cmp, v_traits)
   {}

   //! @copydoc ::boost::intrusive::bstree::bstree(bool,Iterator,Iterator,const key_compare &,const value_traits &)
   template<class Iterator>
   sgtree_impl( bool unique, Iterator b, Iterator e
              , const key_compare &cmp     = key_compare()
              , const value_traits &v_traits = value_traits())
      : tree_type(cmp, v_traits)
   {
      if(unique)
         this->insert_unique(b, e);
      else
         this->insert_equal(b, e);
   }
   sgtree_impl(const sgtree_impl&) = delete;
   sgtree_impl& operator=(const sgtree_impl&) = delete;
   //! @copydoc ::boost::intrusive::bstree::bstree(bstree &&)
   sgtree_impl(sgtree_impl&& x)
      :  tree_type(std::move(x)), alpha_traits(x.get_alpha_traits())
   {
       using std::swap;
       swap(this->get_alpha_traits(), x.get_alpha_traits());
   }

   //! @copydoc ::boost::intrusive::bstree::operator=(bstree &&)
   sgtree_impl& operator=(sgtree_impl&& x)
   {
      this->get_alpha_traits() = x.get_alpha_traits();
      return static_cast<sgtree_impl&>(tree_type::operator=(std::move(x)));
   }

   /// @cond
   private:

   const alpha_traits &get_alpha_traits() const
   {  return *this;  }

   alpha_traits &get_alpha_traits()
   {  return *this;  }

   h_alpha_t get_h_alpha_func() const
   {  return this->get_alpha_traits().get_h_alpha_t();  }

   multiply_by_alpha_t get_alpha_by_max_size_func() const
   {  return this->get_alpha_traits().get_multiply_by_alpha_t(); }

   /// @endcond

   public:

   #ifdef BOOST_INTRUSIVE_DOXYGEN_INVOKED
   //! @copydoc ::boost::intrusive::bstree::~bstree()
   ~sgtree_impl();

   //! @copydoc ::boost::intrusive::bstree::begin()
   iterator begin() noexcept;

   //! @copydoc ::boost::intrusive::bstree::begin()const
   const_iterator begin() const noexcept;

   //! @copydoc ::boost::intrusive::bstree::cbegin()const
   const_iterator cbegin() const noexcept;

   //! @copydoc ::boost::intrusive::bstree::end()
   iterator end() noexcept;

   //! @copydoc ::boost::intrusive::bstree::end()const
   const_iterator end() const noexcept;

   //! @copydoc ::boost::intrusive::bstree::cend()const
   const_iterator cend() const noexcept;

   //! @copydoc ::boost::intrusive::bstree::rbegin()
   reverse_iterator rbegin() noexcept;

   //! @copydoc ::boost::intrusive::bstree::rbegin()const
   const_reverse_iterator rbegin() const noexcept;

   //! @copydoc ::boost::intrusive::bstree::crbegin()const
   const_reverse_iterator crbegin() const noexcept;

   //! @copydoc ::boost::intrusive::bstree::rend()
   reverse_iterator rend() noexcept;

   //! @copydoc ::boost::intrusive::bstree::rend()const
   const_reverse_iterator rend() const noexcept;

   //! @copydoc ::boost::intrusive::bstree::crend()const
   const_reverse_iterator crend() const noexcept;

   //! @copydoc ::boost::intrusive::bstree::root()
   iterator root() noexcept;

   //! @copydoc ::boost::intrusive::bstree::root()const
   const_iterator root() const noexcept;

   //! @copydoc ::boost::intrusive::bstree::croot()const
   const_iterator croot() const noexcept;

   //! @copydoc ::boost::intrusive::bstree::container_from_end_iterator(iterator)
   static sgtree_impl &container_from_end_iterator(iterator end_iterator) noexcept;

   //! @copydoc ::boost::intrusive::bstree::container_from_end_iterator(const_iterator)
   static const sgtree_impl &container_from_end_iterator(const_iterator end_iterator) noexcept;

   //! @copydoc ::boost::intrusive::bstree::container_from_iterator(iterator)
   static sgtree_impl &container_from_iterator(iterator it) noexcept;

   //! @copydoc ::boost::intrusive::bstree::container_from_iterator(const_iterator)
   static const sgtree_impl &container_from_iterator(const_iterator it) noexcept;

   //! @copydoc ::boost::intrusive::bstree::key_comp()const
   key_compare key_comp() const;

   //! @copydoc ::boost::intrusive::bstree::value_comp()const
   value_compare value_comp() const;

   //! @copydoc ::boost::intrusive::bstree::empty()const
   bool empty() const noexcept;

   //! @copydoc ::boost::intrusive::bstree::size()const
   std::size_t size() const noexcept;

   #endif   //#ifdef BOOST_INTRUSIVE_DOXYGEN_INVOKED

   //! @copydoc ::boost::intrusive::bstree::swap
   void swap(sgtree_impl& other)
   {
      //This can throw
      this->tree_type::swap(static_cast<tree_type&>(other));
      using std::swap;
      swap(this->get_alpha_traits(), other.get_alpha_traits());
   }

   //! @copydoc ::boost::intrusive::bstree::clone_from(const bstree&,Cloner,Disposer)
   //! Additional notes: it also copies the alpha factor from the source container.
   template <class Cloner, class Disposer>
   void clone_from(const sgtree_impl &src, Cloner cloner, Disposer disposer)
   {
      tree_type::clone_from(src, cloner, disposer);
      this->get_alpha_traits() = src.get_alpha_traits();
   }

   //! @copydoc ::boost::intrusive::bstree::clone_from(bstree&&,Cloner,Disposer)
   //! Additional notes: it also copies the alpha factor from the source container.
   template <class Cloner, class Disposer>
   void clone_from(sgtree_impl&& src, Cloner cloner, Disposer disposer)
   {
      tree_type::clone_from(std::forward<sgtree_impl>(src), cloner, disposer);
      this->get_alpha_traits() = std::move(src.get_alpha_traits());
   }

   //! @copydoc ::boost::intrusive::bstree::insert_equal(reference)
   iterator insert_equal(reference value)
   {
      node_ptr to_insert(this->get_value_traits().to_node_ptr(value));
      BOOST_INTRUSIVE_SAFE_HOOK_DEFAULT_ASSERT(!safemode_or_autounlink || node_algorithms::unique(to_insert));
      std::size_t max_tree_size = (std::size_t)this->max_tree_size_;
      node_ptr p = node_algorithms::insert_equal_upper_bound
         (this->tree_type::header_ptr(), to_insert, this->key_node_comp(this->key_comp())
         , (std::size_t)this->size(), this->get_h_alpha_func(), max_tree_size);
      this->traits.increment();
      this->max_tree_size_ = (std::size_t)max_tree_size;
      return iterator(p, this->priv_value_traits_ptr());
   }

   //! @copydoc ::boost::intrusive::bstree::insert_equal(const_iterator,reference)
   iterator insert_equal(const_iterator hint, reference value)
   {
      node_ptr to_insert(this->get_value_traits().to_node_ptr(value));
      BOOST_INTRUSIVE_SAFE_HOOK_DEFAULT_ASSERT(!safemode_or_autounlink || node_algorithms::unique(to_insert));
      std::size_t max_tree_size = (std::size_t)this->max_tree_size_;
      node_ptr p = node_algorithms::insert_equal
         ( this->tree_type::header_ptr(), hint.pointed_node(), to_insert, this->key_node_comp(this->key_comp())
         , (std::size_t)this->size(), this->get_h_alpha_func(), max_tree_size);
      this->traits.increment();
      this->max_tree_size_ = (std::size_t)max_tree_size;
      return iterator(p, this->priv_value_traits_ptr());
   }

   //! @copydoc ::boost::intrusive::bstree::insert_equal(Iterator,Iterator)
   template<class Iterator>
   void insert_equal(Iterator b, Iterator e)
   {
      iterator iend(this->end());
      for (; b != e; ++b)
         this->insert_equal(iend, *b);
   }

   //! @copydoc ::boost::intrusive::bstree::insert_unique(reference)
   std::pair<iterator, bool> insert_unique(reference value)
   {
      insert_commit_data commit_data;
      std::pair<iterator, bool> ret = this->insert_unique_check
         (value, this->key_comp(), commit_data);
      if(!ret.second)
         return ret;
      return std::pair<iterator, bool> (this->insert_unique_commit(value, commit_data), true);
   }

   //! @copydoc ::boost::intrusive::bstree::insert_unique(const_iterator,reference)
   iterator insert_unique(const_iterator hint, reference value)
   {
      insert_commit_data commit_data;
      std::pair<iterator, bool> ret = this->insert_unique_check
         (hint, key_of_value()(value), this->key_comp(), commit_data);
      if(!ret.second)
         return ret.first;
      return this->insert_unique_commit(value, commit_data);
   }

   //! @copydoc ::boost::intrusive::bstree::insert_unique_check(const KeyType&,KeyTypeKeyCompare,insert_commit_data&)
   template<class KeyType, class KeyTypeKeyCompare>
   typename std::enable_if<
       !std::is_convertible_v<KeyType, const_iterator>, std::pair<iterator, bool>>::type
   insert_unique_check(const KeyType &key, KeyTypeKeyCompare comp, insert_commit_data &commit_data)
   {
      auto [node, success] =
         node_algorithms::insert_unique_check
            (this->tree_type::header_ptr(), key, this->key_node_comp(comp), commit_data);
      return std::pair<iterator, bool>(iterator(node, this->priv_value_traits_ptr()), success);
   }

   //! @copydoc ::boost::intrusive::bstree::insert_unique_check(const_iterator,const KeyType&,KeyTypeKeyCompare,insert_commit_data&)
   template<class KeyType, class KeyTypeKeyCompare>
   std::pair<iterator, bool> insert_unique_check
      (const_iterator hint, const KeyType &key
      ,KeyTypeKeyCompare comp, insert_commit_data &commit_data)
   {
      auto [node, success] =
         node_algorithms::insert_unique_check
            (this->tree_type::header_ptr(), hint.pointed_node(), key, this->key_node_comp(comp), commit_data);
      return std::pair<iterator, bool>(iterator(node, this->priv_value_traits_ptr()), success);
   }

   //! @copydoc ::boost::intrusive::bstree::insert_unique_check(const key_type&,insert_commit_data&)
   std::pair<iterator, bool> insert_unique_check
      (const key_type &key, insert_commit_data &commit_data)
   {  return this->insert_unique_check(key, this->key_comp(), commit_data);   }

   //! @copydoc ::boost::intrusive::bstree::insert_unique_check(const_iterator,const key_type&,insert_commit_data&)
   std::pair<iterator, bool> insert_unique_check
      (const_iterator hint, const key_type &key, insert_commit_data &commit_data)
   {  return this->insert_unique_check(hint, key, this->key_comp(), commit_data);   }

   //! @copydoc ::boost::intrusive::bstree::insert_unique_commit
   iterator insert_unique_commit(reference value, const insert_commit_data &commit_data) noexcept
   {
      node_ptr to_insert(this->get_value_traits().to_node_ptr(value));
      BOOST_INTRUSIVE_SAFE_HOOK_DEFAULT_ASSERT(!safemode_or_autounlink || node_algorithms::unique(to_insert));
      std::size_t max_tree_size = (std::size_t)this->max_tree_size_;
      node_algorithms::insert_unique_commit
         ( this->tree_type::header_ptr(), to_insert, commit_data
         , (std::size_t)this->size(), this->get_h_alpha_func(), max_tree_size);
      this->traits.increment();
      this->max_tree_size_ = (std::size_t)max_tree_size;
      return iterator(to_insert, this->priv_value_traits_ptr());
   }

   //! @copydoc ::boost::intrusive::bstree::insert_unique(Iterator,Iterator)
   template<class Iterator>
   void insert_unique(Iterator b, Iterator e)
   {
      if(this->empty()){
         iterator iend(this->end());
         for (; b != e; ++b)
            this->insert_unique(iend, *b);
      }
      else{
         for (; b != e; ++b)
            this->insert_unique(*b);
      }
   }

   //! @copydoc ::boost::intrusive::bstree::insert_before
   iterator insert_before(const_iterator pos, reference value) noexcept
   {
      node_ptr to_insert(this->get_value_traits().to_node_ptr(value));
      BOOST_INTRUSIVE_SAFE_HOOK_DEFAULT_ASSERT(!safemode_or_autounlink || node_algorithms::unique(to_insert));
      std::size_t max_tree_size = (std::size_t)this->max_tree_size_;
      node_ptr p = node_algorithms::insert_before
         ( this->tree_type::header_ptr(), pos.pointed_node(), to_insert
         , (std::size_t)this->size(), this->get_h_alpha_func(), max_tree_size);
      this->tree_type::sz_traits().increment();
      this->max_tree_size_ = (std::size_t)max_tree_size;
      return iterator(p, this->priv_value_traits_ptr());
   }

   //! @copydoc ::boost::intrusive::bstree::push_back
   void push_back(reference value) noexcept
   {
      node_ptr to_insert(this->get_value_traits().to_node_ptr(value));
      BOOST_INTRUSIVE_SAFE_HOOK_DEFAULT_ASSERT(!safemode_or_autounlink || node_algorithms::unique(to_insert));
      std::size_t max_tree_size = (std::size_t)this->max_tree_size_;
      node_algorithms::push_back
         ( this->tree_type::header_ptr(), to_insert
         , (std::size_t)this->size(), this->get_h_alpha_func(), max_tree_size);
      this->tree_type::sz_traits().increment();
      this->max_tree_size_ = (std::size_t)max_tree_size;
   }

   //! @copydoc ::boost::intrusive::bstree::push_front
   void push_front(reference value) noexcept
   {
      node_ptr to_insert(this->get_value_traits().to_node_ptr(value));
      BOOST_INTRUSIVE_SAFE_HOOK_DEFAULT_ASSERT(!safemode_or_autounlink || node_algorithms::unique(to_insert));
      std::size_t max_tree_size = (std::size_t)this->max_tree_size_;
      node_algorithms::push_front
         ( this->tree_type::header_ptr(), to_insert
         , (std::size_t)this->size(), this->get_h_alpha_func(), max_tree_size);
      this->tree_type::sz_traits().increment();
      this->max_tree_size_ = (std::size_t)max_tree_size;
   }


   //! @copydoc ::boost::intrusive::bstree::erase(const_iterator)
   iterator erase(const_iterator i) noexcept
   {
      const_iterator ret(i);
      ++ret;
      node_ptr to_erase(i.pointed_node());
      BOOST_INTRUSIVE_SAFE_HOOK_DEFAULT_ASSERT(!safemode_or_autounlink || !node_algorithms::unique(to_erase));
      std::size_t max_tree_size = this->max_tree_size_;
      node_algorithms::erase
         ( this->tree_type::header_ptr(), to_erase, (std::size_t)this->size()
         , max_tree_size, this->get_alpha_by_max_size_func());
      this->max_tree_size_ = (std::size_t)max_tree_size;
      this->traits.decrement();
      if constexpr (safemode_or_autounlink)
         node_algorithms::init(to_erase);
      return ret.unconst();
   }

   //! @copydoc ::boost::intrusive::bstree::erase(const_iterator,const_iterator)
   iterator erase(const_iterator b, const_iterator e) noexcept
   {  std::size_t n;   return private_erase(b, e, n);   }

   //! @copydoc ::boost::intrusive::bstree::erase(const key_type &)
   std::size_t erase(const key_type &key)
   {  return this->erase(key, this->key_comp());   }

   //! @copydoc ::boost::intrusive::bstree::erase(const KeyType&,KeyTypeKeyCompare)
   template<class KeyType, class KeyTypeKeyCompare>
   std::enable_if_t<!std::is_convertible_v<KeyTypeKeyCompare, const_iterator>, std::size_t>
      erase(const KeyType& key, KeyTypeKeyCompare comp)
   {
      std::pair<iterator,iterator> p = this->equal_range(key, comp);
      std::size_t n;
      private_erase(p.first, p.second, n);
      return n;
   }

   //! @copydoc ::boost::intrusive::bstree::erase_and_dispose(const_iterator,Disposer)
   template<class Disposer>
   iterator erase_and_dispose(const_iterator i, Disposer disposer) noexcept
   {
      node_ptr to_erase(i.pointed_node());
      iterator ret(this->erase(i));
      disposer(this->get_value_traits().to_value_ptr(to_erase));
      return ret;
   }

   #if !defined(BOOST_INTRUSIVE_DOXYGEN_INVOKED)
   template<class Disposer>
   iterator erase_and_dispose(iterator i, Disposer disposer) noexcept
   {  return this->erase_and_dispose(const_iterator(i), disposer);   }
   #endif

   //! @copydoc ::boost::intrusive::bstree::erase_and_dispose(const_iterator,const_iterator,Disposer)
   template<class Disposer>
   iterator erase_and_dispose(const_iterator b, const_iterator e, Disposer disposer) noexcept
   {  std::size_t n;   return private_erase(b, e, n, disposer);   }

   //! @copydoc ::boost::intrusive::bstree::erase_and_dispose(const key_type &, Disposer)
   template<class Disposer>
   std::size_t erase_and_dispose(const key_type &key, Disposer disposer)
   {
      std::pair<iterator,iterator> p = this->equal_range(key);
      std::size_t n;
      private_erase(p.first, p.second, n, disposer);
      return n;
   }

   //! @copydoc ::boost::intrusive::bstree::erase_and_dispose(const KeyType&,KeyTypeKeyCompare,Disposer)
   template<class KeyType, class KeyTypeKeyCompare, class Disposer>
   std::enable_if_t<!std::is_convertible_v<KeyTypeKeyCompare, const_iterator>, std::size_t>
      erase_and_dispose(const KeyType& key, KeyTypeKeyCompare comp, Disposer disposer)
   {
      std::pair<iterator,iterator> p = this->equal_range(key, comp);
      std::size_t n;
      private_erase(p.first, p.second, n, disposer);
      return n;
   }

   //! @copydoc ::boost::intrusive::bstree::clear
   void clear() noexcept
   {
      tree_type::clear();
      this->max_tree_size_ = 0;
   }

   //! @copydoc ::boost::intrusive::bstree::clear_and_dispose
   template<class Disposer>
   void clear_and_dispose(Disposer disposer) noexcept
   {
      tree_type::clear_and_dispose(disposer);
      this->max_tree_size_ = 0;
   }

   #if defined(BOOST_INTRUSIVE_DOXYGEN_INVOKED)
   //! @copydoc ::boost::intrusive::bstree::merge_unique
   template<class T, class ...Options2> void merge_unique(sgtree<T, Options2...> &);
   #else
   template<class Compare2>
   void merge_unique(sgtree_impl
      <ValueTraits, VoidOrKeyOfValue, Compare2, HeaderHolder> &source)
   #endif
   {
      node_ptr it   (node_algorithms::begin_node(source.header_ptr()))
             , itend(node_algorithms::end_node  (source.header_ptr()));

      while(it != itend){
         node_ptr const p(it);
         BOOST_INTRUSIVE_SAFE_HOOK_DEFAULT_ASSERT(!safemode_or_autounlink || !node_algorithms::unique(p));
         it = node_algorithms::next_node(it);

         std::size_t max_tree1_size = this->max_tree_size_;
         std::size_t max_tree2_size = source.get_max_tree_size();
         if( node_algorithms::transfer_unique
               ( this->header_ptr(), this->key_node_comp(this->key_comp()), this->size(), max_tree1_size
               , source.header_ptr(), p, source.size(), max_tree2_size
               , this->get_h_alpha_func(), this->get_alpha_by_max_size_func()) ){
            this->max_tree_size_  = (std::size_t)max_tree1_size;
            this->traits.increment();
            source.get_max_tree_size() = (std::size_t)max_tree2_size;
            source.traits.decrement();
         }
      }
   }

   #if defined(BOOST_INTRUSIVE_DOXYGEN_INVOKED)
   //! @copydoc ::boost::intrusive::bstree::merge_equal
   template<class T, class ...Options2> void merge_equal(sgtree<T, Options2...> &);
   #else
   template<class Compare2>
   void merge_equal(sgtree_impl
      <ValueTraits, VoidOrKeyOfValue, Compare2, HeaderHolder> &source)
   #endif
   {
      node_ptr it   (node_algorithms::begin_node(source.header_ptr()))
             , itend(node_algorithms::end_node  (source.header_ptr()));

      while(it != itend){
         node_ptr const p(it);
         BOOST_INTRUSIVE_SAFE_HOOK_DEFAULT_ASSERT(!safemode_or_autounlink || !node_algorithms::unique(p));
         it = node_algorithms::next_node(it);
         std::size_t max_tree1_size = this->max_tree_size_;
         std::size_t max_tree2_size = source.get_max_tree_size();
         node_algorithms::transfer_equal
            ( this->header_ptr(), this->key_node_comp(this->key_comp()), this->size(), max_tree1_size
            , source.header_ptr(), p, source.size(), max_tree2_size
            , this->get_h_alpha_func(), this->get_alpha_by_max_size_func());
         this->max_tree_size_  = (std::size_t)max_tree1_size;
         this->traits.increment();
         source.get_max_tree_size() = (std::size_t)max_tree2_size;
         source.traits.decrement();
      }
   }

   #ifdef BOOST_INTRUSIVE_DOXYGEN_INVOKED
   //! @copydoc ::boost::intrusive::bstree::count(const key_type &)const
   std::size_t count(const key_type &key) const;

   //! @copydoc ::boost::intrusive::bstree::count(const KeyType&,KeyTypeKeyCompare)const
   template<class KeyType, class KeyTypeKeyCompare>
   std::size_t count(const KeyType& key, KeyTypeKeyCompare comp) const;

   //! @copydoc ::boost::intrusive::bstree::lower_bound(const key_type &)
   iterator lower_bound(const key_type &key);

   //! @copydoc ::boost::intrusive::bstree::lower_bound(const KeyType&,KeyTypeKeyCompare)
   template<class KeyType, class KeyTypeKeyCompare>
   iterator lower_bound(const KeyType& key, KeyTypeKeyCompare comp);

   //! @copydoc ::boost::intrusive::bstree::lower_bound(const key_type &)const
   const_iterator lower_bound(const key_type &key) const;

   //! @copydoc ::boost::intrusive::bstree::lower_bound(const KeyType&,KeyTypeKeyCompare)const
   template<class KeyType, class KeyTypeKeyCompare>
   const_iterator lower_bound(const KeyType& key, KeyTypeKeyCompare comp) const;

   //! @copydoc ::boost::intrusive::bstree::upper_bound(const key_type &)
   iterator upper_bound(const key_type &key);

   //! @copydoc ::boost::intrusive::bstree::upper_bound(const KeyType&,KeyTypeKeyCompare)
   template<class KeyType, class KeyTypeKeyCompare>
   iterator upper_bound(const KeyType& key, KeyTypeKeyCompare comp);

   //! @copydoc ::boost::intrusive::bstree::upper_bound(const key_type &)const
   const_iterator upper_bound(const key_type &key) const;

   //! @copydoc ::boost::intrusive::bstree::upper_bound(const KeyType&,KeyTypeKeyCompare)const
   template<class KeyType, class KeyTypeKeyCompare>
   const_iterator upper_bound(const KeyType& key, KeyTypeKeyCompare comp) const;

   //! @copydoc ::boost::intrusive::bstree::find(const key_type &)
   iterator find(const key_type &key);

   //! @copydoc ::boost::intrusive::bstree::find(const KeyType&,KeyTypeKeyCompare)
   template<class KeyType, class KeyTypeKeyCompare>
   iterator find(const KeyType& key, KeyTypeKeyCompare comp);

   //! @copydoc ::boost::intrusive::bstree::find(const key_type &)const
   const_iterator find(const key_type &key) const;

   //! @copydoc ::boost::intrusive::bstree::find(const KeyType&,KeyTypeKeyCompare)const
   template<class KeyType, class KeyTypeKeyCompare>
   const_iterator find(const KeyType& key, KeyTypeKeyCompare comp) const;

   //! @copydoc ::boost::intrusive::bstree::equal_range(const key_type &)
   std::pair<iterator,iterator> equal_range(const key_type &key);

   //! @copydoc ::boost::intrusive::bstree::equal_range(const KeyType&,KeyTypeKeyCompare)
   template<class KeyType, class KeyTypeKeyCompare>
   std::pair<iterator,iterator> equal_range(const KeyType& key, KeyTypeKeyCompare comp);

   //! @copydoc ::boost::intrusive::bstree::equal_range(const key_type &)const
   std::pair<const_iterator, const_iterator>
      equal_range(const key_type &key) const;

   //! @copydoc ::boost::intrusive::bstree::equal_range(const KeyType&,KeyTypeKeyCompare)const
   template<class KeyType, class KeyTypeKeyCompare>
   std::pair<const_iterator, const_iterator>
      equal_range(const KeyType& key, KeyTypeKeyCompare comp) const;

   //! @copydoc ::boost::intrusive::bstree::bounded_range(const key_type &,const key_type &,bool,bool)
   std::pair<iterator,iterator> bounded_range
      (const key_type &lower_key, const key_type &upper_key, bool left_closed, bool right_closed);

   //! @copydoc ::boost::intrusive::bstree::bounded_range(const KeyType&,const KeyType&,KeyTypeKeyCompare,bool,bool)
   template<class KeyType, class KeyTypeKeyCompare>
   std::pair<iterator,iterator> bounded_range
      (const KeyType& lower_key, const KeyType& upper_key, KeyTypeKeyCompare comp, bool left_closed, bool right_closed);

   //! @copydoc ::boost::intrusive::bstree::bounded_range(const key_type &,const key_type &,bool,bool)const
   std::pair<const_iterator, const_iterator>
      bounded_range(const key_type &lower_key, const key_type &upper_key, bool left_closed, bool right_closed) const;

   //! @copydoc ::boost::intrusive::bstree::bounded_range(const KeyType&,const KeyType&,KeyTypeKeyCompare,bool,bool)const
   template<class KeyType, class KeyTypeKeyCompare>
   std::pair<const_iterator, const_iterator> bounded_range
         (const KeyType& lower_key, const KeyType& upper_key, KeyTypeKeyCompare comp, bool left_closed, bool right_closed) const;

   //! @copydoc ::boost::intrusive::bstree::s_iterator_to(reference)
   static iterator s_iterator_to(reference value) noexcept;

   //! @copydoc ::boost::intrusive::bstree::s_iterator_to(const_reference)
   static const_iterator s_iterator_to(const_reference value) noexcept;

   //! @copydoc ::boost::intrusive::bstree::iterator_to(reference)
   iterator iterator_to(reference value) noexcept;

   //! @copydoc ::boost::intrusive::bstree::iterator_to(const_reference)const
   const_iterator iterator_to(const_reference value) const noexcept;

   //! @copydoc ::boost::intrusive::bstree::init_node(reference)
   static void init_node(reference value) noexcept;

   //! @copydoc ::boost::intrusive::bstree::unlink_leftmost_without_rebalance
   pointer unlink_leftmost_without_rebalance() noexcept;

   //! @copydoc ::boost::intrusive::bstree::replace_node
   void replace_node(iterator replace_this, reference with_this) noexcept;

   //! @copydoc ::boost::intrusive::bstree::remove_node
   void remove_node(reference value) noexcept;

   //! @copydoc ::boost::intrusive::bstree::rebalance
   void rebalance() noexcept;

   //! @copydoc ::boost::intrusive::bstree::rebalance_subtree
   iterator rebalance_subtree(iterator root) noexcept;

   friend bool operator< (const sgtree_impl &x, const sgtree_impl &y);

   friend bool operator==(const sgtree_impl &x, const sgtree_impl &y);

   friend bool operator!= (const sgtree_impl &x, const sgtree_impl &y);

   friend bool operator>(const sgtree_impl &x, const sgtree_impl &y);

   friend bool operator<=(const sgtree_impl &x, const sgtree_impl &y);

   friend bool operator>=(const sgtree_impl &x, const sgtree_impl &y);

   friend void swap(sgtree_impl &x, sgtree_impl &y);

   #endif   //#ifdef BOOST_INTRUSIVE_DOXYGEN_INVOKED

   //! <b>Returns</b>: The balance factor (alpha) used in this tree
   //!
   //! <b>Throws</b>: Nothing.
   //!
   //! <b>Complexity</b>: Constant.
   float balance_factor() const noexcept
   {  return this->get_alpha_traits().get_alpha(); }

   //! <b>Requires</b>: new_alpha must be a value between 0.5 and 1.0
   //!
   //! <b>Effects</b>: Establishes a new balance factor (alpha) and rebalances
   //!   the tree if the new balance factor is stricter (less) than the old factor.
   //!
   //! <b>Throws</b>: Nothing.
   //!
   //! <b>Complexity</b>: Linear to the elements in the subtree.
   void balance_factor(float new_alpha) noexcept
   {
      //The alpha factor CAN't be changed if the fixed, floating operation-less
      //1/sqrt(2) alpha factor option is activated
      BOOST_INTRUSIVE_INVARIANT_ASSERT((new_alpha > 0.5f && new_alpha < 1.0f));
      if(new_alpha >= 0.5f && new_alpha < 1.0f){
         float old_alpha = this->get_alpha_traits().get_alpha();
         this->get_alpha_traits().set_alpha(new_alpha);
         if(new_alpha < old_alpha){
            this->max_tree_size_ = this->size();
            this->rebalance();
         }
      }
   }

   /// @cond
   private:
   template<class Disposer>
   iterator private_erase(const_iterator b, const_iterator e, std::size_t &n, Disposer disposer) noexcept
   {
      for(n = 0; b != e; ++n)
        this->erase_and_dispose(b++, disposer);
      return b.unconst();
   }

   iterator private_erase(const_iterator b, const_iterator e, std::size_t &n) noexcept
   {
      for(n = 0; b != e; ++n)
        this->erase(b++);
      return b.unconst();
   }
   /// @endcond
};


//! Helper metafunction to define a \c sgtree that yields to the same type when the
//! same options (either explicitly or implicitly) are used.
template<class T, class ...Options>
struct make_sgtree
{
   /// @cond
   typedef typename pack_options
      < sgtree_defaults,
      Options...
      >::type packed_options;

   using value_traits = detail::get_value_traits_t<T, typename packed_options::proto_value_traits>;

   typedef sgtree_impl
         < value_traits
         , typename packed_options::key_of_value
         , typename packed_options::compare
         , typename packed_options::header_holder_type
         > implementation_defined;
   /// @endcond
   typedef implementation_defined type;
};


#ifndef BOOST_INTRUSIVE_DOXYGEN_INVOKED

template<class T, class ...Options>
class sgtree
   :  public make_sgtree<T,
      Options...
      >::type
{
   typedef typename make_sgtree
      <T,
      Options...
      >::type   Base;

   public:
   typedef typename Base::key_compare        key_compare;
   typedef typename Base::value_traits       value_traits;
   typedef typename Base::iterator           iterator;
   typedef typename Base::const_iterator     const_iterator;
   typedef typename Base::reverse_iterator           reverse_iterator;
   typedef typename Base::const_reverse_iterator     const_reverse_iterator;

   //Assert if passed value traits are compatible with the type
   static_assert(std::is_same_v<typename value_traits::value_type, T>);

   inline sgtree()
      :  Base()
   {}

   inline explicit sgtree(const key_compare &cmp, const value_traits &v_traits = value_traits())
      :  Base(cmp, v_traits)
   {}

   template<class Iterator>
   inline sgtree( bool unique, Iterator b, Iterator e
         , const key_compare &cmp = key_compare()
         , const value_traits &v_traits = value_traits())
      :  Base(unique, b, e, cmp, v_traits)
   {}

   sgtree(const sgtree&) = delete;
   sgtree& operator=(const sgtree&) = delete;
   inline sgtree(sgtree&& x) : Base(std::move(x)) {}

   inline sgtree& operator=(sgtree&& x)
   {  return static_cast<sgtree &>(this->Base::operator=(std::move(x)));  }

   template <class Cloner, class Disposer>
   inline void clone_from(const sgtree &src, Cloner cloner, Disposer disposer)
   {  Base::clone_from(src, cloner, disposer);  }

   template <class Cloner, class Disposer>
   inline void clone_from(sgtree&& src, Cloner cloner, Disposer disposer)
   {  Base::clone_from(std::forward<sgtree>(src), cloner, disposer);  }

   inline static sgtree &container_from_end_iterator(iterator end_iterator) noexcept
   {  return static_cast<sgtree &>(Base::container_from_end_iterator(end_iterator));   }

   inline static const sgtree &container_from_end_iterator(const_iterator end_iterator) noexcept
   {  return static_cast<const sgtree &>(Base::container_from_end_iterator(end_iterator));   }

   inline static sgtree &container_from_iterator(iterator it) noexcept
   {  return static_cast<sgtree &>(Base::container_from_iterator(it));   }

   inline static const sgtree &container_from_iterator(const_iterator it) noexcept
   {  return static_cast<const sgtree &>(Base::container_from_iterator(it));   }
};

#endif

} //namespace intrusive
} //namespace boost

#endif //BOOST_INTRUSIVE_SGTREE_HPP
