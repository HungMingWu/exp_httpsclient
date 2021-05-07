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
#ifndef BOOST_INTRUSIVE_SG_SET_HPP
#define BOOST_INTRUSIVE_SG_SET_HPP

#include <boost/intrusive/intrusive_fwd.hpp>
#include <boost/intrusive/detail/mpl.hpp>
#include <boost/intrusive/sgtree.hpp>

#if defined(BOOST_HAS_PRAGMA_ONCE)
#  pragma once
#endif

namespace boost {
namespace intrusive {

#if !defined(BOOST_INTRUSIVE_DOXYGEN_INVOKED)
template<class ValueTraits, class VoidOrKeyOfValue, class Compare, typename HeaderHolder>
class sg_multiset_impl;
#endif

//! The class template sg_set is an intrusive container, that mimics most of
//! the interface of std::sg_set as described in the C++ standard.
//!
//! The template parameter \c T is the type to be managed by the container.
//! The user can specify additional options and if no options are provided
//! default options are used.
//!
//! The container supports the following options:
//! \c base_hook<>/member_hook<>/value_traits<> and
//! \c compare<>.
#if defined(BOOST_INTRUSIVE_DOXYGEN_INVOKED)
template<class T, class ...Options>
#else
template<class ValueTraits, class VoidOrKeyOfValue, class Compare, typename HeaderHolder>
#endif
class sg_set_impl
#ifndef BOOST_INTRUSIVE_DOXYGEN_INVOKED
   : public sgtree_impl<ValueTraits, VoidOrKeyOfValue, Compare, HeaderHolder>
#endif
{
   /// @cond
   typedef sgtree_impl<ValueTraits, VoidOrKeyOfValue, Compare, HeaderHolder> tree_type;

   typedef tree_type implementation_defined;
   /// @endcond

   public:
   typedef typename implementation_defined::value_type               value_type;
   typedef typename implementation_defined::key_type                 key_type;
   typedef typename implementation_defined::key_of_value             key_of_value;
   typedef typename implementation_defined::value_traits             value_traits;
   typedef typename implementation_defined::pointer                  pointer;
   typedef typename implementation_defined::const_pointer            const_pointer;
   typedef typename implementation_defined::reference                reference;
   typedef typename implementation_defined::const_reference          const_reference;
   typedef typename implementation_defined::difference_type          difference_type;
   typedef typename implementation_defined::value_compare            value_compare;
   typedef typename implementation_defined::key_compare              key_compare;
   typedef typename implementation_defined::iterator                 iterator;
   typedef typename implementation_defined::const_iterator           const_iterator;
   typedef typename implementation_defined::reverse_iterator         reverse_iterator;
   typedef typename implementation_defined::const_reverse_iterator   const_reverse_iterator;
   typedef typename implementation_defined::insert_commit_data       insert_commit_data;
   typedef typename implementation_defined::node_traits              node_traits;
   typedef typename implementation_defined::node                     node;
   typedef typename implementation_defined::node_ptr                 node_ptr;
   typedef typename implementation_defined::const_node_ptr           const_node_ptr;
   typedef typename implementation_defined::node_algorithms          node_algorithms;

   static const bool constant_time_size = tree_type::constant_time_size;

   public:
   //! @copydoc ::boost::intrusive::sgtree::sgtree()
   sg_set_impl()
      :  tree_type()
   {}

   //! @copydoc ::boost::intrusive::sgtree::sgtree(const key_compare &,const value_traits &)
   explicit sg_set_impl( const key_compare &cmp, const value_traits &v_traits = value_traits())
      :  tree_type(cmp, v_traits)
   {}

   //! @copydoc ::boost::intrusive::sgtree::sgtree(bool,Iterator,Iterator,const key_compare &,const value_traits &)
   template<class Iterator>
   sg_set_impl( Iterator b, Iterator e
           , const key_compare &cmp = key_compare()
           , const value_traits &v_traits = value_traits())
      : tree_type(true, b, e, cmp, v_traits)
   {}
   sg_set_impl(const sg_set_impl&) = delete;
   sg_set_impl& operator=(const sg_set_impl&) = delete;
   //! @copydoc ::boost::intrusive::sgtree::sgtree(sgtree &&)
   sg_set_impl(sg_set_impl&& x) : tree_type(std::move(x)) {}

   //! @copydoc ::boost::intrusive::sgtree::operator=(sgtree &&)
   sg_set_impl& operator=(sg_set_impl&& x)
   {  return static_cast<sg_set_impl&>(tree_type::operator=(std::move(x))); }

   #ifdef BOOST_INTRUSIVE_DOXYGEN_INVOKED
   //! @copydoc ::boost::intrusive::sgtree::~sgtree()
   ~sg_set_impl();

   //! @copydoc ::boost::intrusive::sgtree::begin()
   iterator begin() noexcept;

   //! @copydoc ::boost::intrusive::sgtree::begin()const
   const_iterator begin() const noexcept;

   //! @copydoc ::boost::intrusive::sgtree::cbegin()const
   const_iterator cbegin() const noexcept;

   //! @copydoc ::boost::intrusive::sgtree::end()
   iterator end() noexcept;

   //! @copydoc ::boost::intrusive::sgtree::end()const
   const_iterator end() const noexcept;

   //! @copydoc ::boost::intrusive::sgtree::cend()const
   const_iterator cend() const noexcept;

   //! @copydoc ::boost::intrusive::sgtree::rbegin()
   reverse_iterator rbegin() noexcept;

   //! @copydoc ::boost::intrusive::sgtree::rbegin()const
   const_reverse_iterator rbegin() const noexcept;

   //! @copydoc ::boost::intrusive::sgtree::crbegin()const
   const_reverse_iterator crbegin() const noexcept;

   //! @copydoc ::boost::intrusive::sgtree::rend()
   reverse_iterator rend() noexcept;

   //! @copydoc ::boost::intrusive::sgtree::rend()const
   const_reverse_iterator rend() const noexcept;

   //! @copydoc ::boost::intrusive::sgtree::crend()const
   const_reverse_iterator crend() const noexcept;

   //! @copydoc ::boost::intrusive::sgtree::root()
   iterator root() noexcept;

   //! @copydoc ::boost::intrusive::sgtree::root()const
   const_iterator root() const noexcept;

   //! @copydoc ::boost::intrusive::sgtree::croot()const
   const_iterator croot() const noexcept;

   //! @copydoc ::boost::intrusive::sgtree::container_from_end_iterator(iterator)
   static sg_set_impl &container_from_end_iterator(iterator end_iterator) noexcept;

   //! @copydoc ::boost::intrusive::sgtree::container_from_end_iterator(const_iterator)
   static const sg_set_impl &container_from_end_iterator(const_iterator end_iterator) noexcept;

   //! @copydoc ::boost::intrusive::sgtree::container_from_iterator(iterator)
   static sg_set_impl &container_from_iterator(iterator it) noexcept;

   //! @copydoc ::boost::intrusive::sgtree::container_from_iterator(const_iterator)
   static const sg_set_impl &container_from_iterator(const_iterator it) noexcept;

   //! @copydoc ::boost::intrusive::sgtree::key_comp()const
   key_compare key_comp() const;

   //! @copydoc ::boost::intrusive::sgtree::value_comp()const
   value_compare value_comp() const;

   //! @copydoc ::boost::intrusive::sgtree::empty()const
   bool empty() const noexcept;

   //! @copydoc ::boost::intrusive::sgtree::size()const
   std::size_t size() const noexcept;

   //! @copydoc ::boost::intrusive::sgtree::swap
   void swap(sg_set_impl& other);

   //! @copydoc ::boost::intrusive::sgtree::clone_from(const sgtree&,Cloner,Disposer)
   template <class Cloner, class Disposer>
   void clone_from(const sg_set_impl &src, Cloner cloner, Disposer disposer);

   #else

   using tree_type::clone_from;

   #endif   //#ifdef BOOST_INTRUSIVE_DOXYGEN_INVOKED

   //! @copydoc ::boost::intrusive::sgtree::clone_from(sgtree&&,Cloner,Disposer)
   template <class Cloner, class Disposer>
   void clone_from(sg_set_impl&& src, Cloner cloner, Disposer disposer)
   {  tree_type::clone_from(std::forward<sg_set_impl>(src), cloner, disposer);  }

   //! @copydoc ::boost::intrusive::sgtree::insert_unique(reference)
   std::pair<iterator, bool> insert(reference value)
   {  return tree_type::insert_unique(value);  }

   //! @copydoc ::boost::intrusive::sgtree::insert_unique(const_iterator,reference)
   iterator insert(const_iterator hint, reference value)
   {  return tree_type::insert_unique(hint, value);  }

   //! @copydoc ::boost::intrusive::sgtree::insert_unique_check(const key_type&,insert_commit_data&)
   std::pair<iterator, bool> insert_check
      (const key_type &key, insert_commit_data &commit_data)
   {  return tree_type::insert_unique_check(key, commit_data); }

   //! @copydoc ::boost::intrusive::sgtree::insert_unique_check(const_iterator,const key_type&,insert_commit_data&)
   std::pair<iterator, bool> insert_check
      (const_iterator hint, const key_type &key
      ,insert_commit_data &commit_data)
   {  return tree_type::insert_unique_check(hint, key, commit_data); }

   //! @copydoc ::boost::intrusive::sgtree::insert_unique_check(const KeyType&,KeyTypeKeyCompare,insert_commit_data&)
   template<class KeyType, class KeyTypeKeyCompare>
   std::pair<iterator, bool> insert_check
      (const KeyType &key, KeyTypeKeyCompare comp, insert_commit_data &commit_data)
   {  return tree_type::insert_unique_check(key, comp, commit_data); }

   //! @copydoc ::boost::intrusive::sgtree::insert_unique_check(const_iterator,const KeyType&,KeyTypeKeyCompare,insert_commit_data&)
   template<class KeyType, class KeyTypeKeyCompare>
   std::pair<iterator, bool> insert_check
      (const_iterator hint, const KeyType &key
      ,KeyTypeKeyCompare comp, insert_commit_data &commit_data)
   {  return tree_type::insert_unique_check(hint, key, comp, commit_data); }

   //! @copydoc ::boost::intrusive::sgtree::insert_unique(Iterator,Iterator)
   template<class Iterator>
   void insert(Iterator b, Iterator e)
   {  tree_type::insert_unique(b, e);  }

   //! @copydoc ::boost::intrusive::sgtree::insert_unique_commit
   iterator insert_commit(reference value, const insert_commit_data &commit_data) noexcept
   {  return tree_type::insert_unique_commit(value, commit_data);  }

   #ifdef BOOST_INTRUSIVE_DOXYGEN_INVOKED
   //! @copydoc ::boost::intrusive::sgtree::insert_before
   iterator insert_before(const_iterator pos, reference value) noexcept;

   //! @copydoc ::boost::intrusive::sgtree::push_back
   void push_back(reference value) noexcept;

   //! @copydoc ::boost::intrusive::sgtree::push_front
   void push_front(reference value) noexcept;

   //! @copydoc ::boost::intrusive::sgtree::erase(const_iterator)
   iterator erase(const_iterator i) noexcept;

   //! @copydoc ::boost::intrusive::sgtree::erase(const_iterator,const_iterator)
   iterator erase(const_iterator b, const_iterator e) noexcept;

   //! @copydoc ::boost::intrusive::sgtree::erase(const key_type &)
   std::size_t erase(const key_type &key);

   //! @copydoc ::boost::intrusive::sgtree::erase(const KeyType&,KeyTypeKeyCompare)
   template<class KeyType, class KeyTypeKeyCompare>
   std::size_t erase(const KeyType& key, KeyTypeKeyCompare comp);

   //! @copydoc ::boost::intrusive::sgtree::erase_and_dispose(const_iterator,Disposer)
   template<class Disposer>
   iterator erase_and_dispose(const_iterator i, Disposer disposer) noexcept;

   //! @copydoc ::boost::intrusive::sgtree::erase_and_dispose(const_iterator,const_iterator,Disposer)
   template<class Disposer>
   iterator erase_and_dispose(const_iterator b, const_iterator e, Disposer disposer) noexcept;

   //! @copydoc ::boost::intrusive::sgtree::erase_and_dispose(const key_type &, Disposer)
   template<class Disposer>
   std::size_t erase_and_dispose(const key_type &key, Disposer disposer);

   //! @copydoc ::boost::intrusive::sgtree::erase_and_dispose(const KeyType&,KeyTypeKeyCompare,Disposer)
   template<class KeyType, class KeyTypeKeyCompare, class Disposer>
   std::size_t erase_and_dispose(const KeyType& key, KeyTypeKeyCompare comp, Disposer disposer);

   //! @copydoc ::boost::intrusive::sgtree::clear
   void clear() noexcept;

   //! @copydoc ::boost::intrusive::sgtree::clear_and_dispose
   template<class Disposer>
   void clear_and_dispose(Disposer disposer) noexcept;

   #endif   //   #ifdef BOOST_INTRUSIVE_DOXYGEN_INVOKED

   //! @copydoc ::boost::intrusive::sgtree::count(const key_type &)const
   std::size_t count(const key_type &key) const
   {  return static_cast<std::size_t>(this->tree_type::find(key) != this->tree_type::cend()); }

   //! @copydoc ::boost::intrusive::sgtree::count(const KeyType&,KeyTypeKeyCompare)const
   template<class KeyType, class KeyTypeKeyCompare>
   std::size_t count(const KeyType& key, KeyTypeKeyCompare comp) const
   {  return static_cast<std::size_t>(this->tree_type::find(key, comp) != this->tree_type::cend()); }

   #ifdef BOOST_INTRUSIVE_DOXYGEN_INVOKED

   //! @copydoc ::boost::intrusive::sgtree::lower_bound(const key_type &)
   iterator lower_bound(const key_type &key);

   //! @copydoc ::boost::intrusive::sgtree::lower_bound(const KeyType&,KeyTypeKeyCompare)
   template<class KeyType, class KeyTypeKeyCompare>
   iterator lower_bound(const KeyType& key, KeyTypeKeyCompare comp);

   //! @copydoc ::boost::intrusive::sgtree::lower_bound(const key_type &)const
   const_iterator lower_bound(const key_type &key) const;

   //! @copydoc ::boost::intrusive::sgtree::lower_bound(const KeyType&,KeyTypeKeyCompare)const
   template<class KeyType, class KeyTypeKeyCompare>
   const_iterator lower_bound(const KeyType& key, KeyTypeKeyCompare comp) const;

   //! @copydoc ::boost::intrusive::sgtree::upper_bound(const key_type &)
   iterator upper_bound(const key_type &key);

   //! @copydoc ::boost::intrusive::sgtree::upper_bound(const KeyType&,KeyTypeKeyCompare)
   template<class KeyType, class KeyTypeKeyCompare>
   iterator upper_bound(const KeyType& key, KeyTypeKeyCompare comp);

   //! @copydoc ::boost::intrusive::sgtree::upper_bound(const key_type &)const
   const_iterator upper_bound(const key_type &key) const;

   //! @copydoc ::boost::intrusive::sgtree::upper_bound(const KeyType&,KeyTypeKeyCompare)const
   template<class KeyType, class KeyTypeKeyCompare>
   const_iterator upper_bound(const KeyType& key, KeyTypeKeyCompare comp) const;

   //! @copydoc ::boost::intrusive::sgtree::find(const key_type &)
   iterator find(const key_type &key);

   //! @copydoc ::boost::intrusive::sgtree::find(const KeyType&,KeyTypeKeyCompare)
   template<class KeyType, class KeyTypeKeyCompare>
   iterator find(const KeyType& key, KeyTypeKeyCompare comp);

   //! @copydoc ::boost::intrusive::sgtree::find(const key_type &)const
   const_iterator find(const key_type &key) const;

   //! @copydoc ::boost::intrusive::sgtree::find(const KeyType&,KeyTypeKeyCompare)const
   template<class KeyType, class KeyTypeKeyCompare>
   const_iterator find(const KeyType& key, KeyTypeKeyCompare comp) const;

   #endif   //   #ifdef BOOST_INTRUSIVE_DOXYGEN_INVOKED

   //! @copydoc ::boost::intrusive::sgtree::equal_range(const key_type &)
   std::pair<iterator,iterator> equal_range(const key_type &key)
   {  return this->tree_type::lower_bound_range(key); }

   //! @copydoc ::boost::intrusive::sgtree::equal_range(const KeyType&,KeyTypeKeyCompare)
   template<class KeyType, class KeyTypeKeyCompare>
   std::pair<iterator,iterator> equal_range(const KeyType& key, KeyTypeKeyCompare comp)
   {  return this->tree_type::equal_range(key, comp); }

   //! @copydoc ::boost::intrusive::sgtree::equal_range(const key_type &)const
   std::pair<const_iterator, const_iterator>
      equal_range(const key_type &key) const
   {  return this->tree_type::lower_bound_range(key); }

   //! @copydoc ::boost::intrusive::sgtree::equal_range(const KeyType&,KeyTypeKeyCompare)const
   template<class KeyType, class KeyTypeKeyCompare>
   std::pair<const_iterator, const_iterator>
      equal_range(const KeyType& key, KeyTypeKeyCompare comp) const
   {  return this->tree_type::equal_range(key, comp); }

   #ifdef BOOST_INTRUSIVE_DOXYGEN_INVOKED

   //! @copydoc ::boost::intrusive::sgtree::bounded_range(const key_type &,const key_type &,bool,bool)
   std::pair<iterator,iterator> bounded_range
      (const key_type &lower_key, const key_type &upper_key, bool left_closed, bool right_closed);

   //! @copydoc ::boost::intrusive::sgtree::bounded_range(const KeyType&,const KeyType&,KeyTypeKeyCompare,bool,bool)
   template<class KeyType, class KeyTypeKeyCompare>
   std::pair<iterator,iterator> bounded_range
      (const KeyType& lower_key, const KeyType& upper_key, KeyTypeKeyCompare comp, bool left_closed, bool right_closed);

   //! @copydoc ::boost::intrusive::sgtree::bounded_range(const key_type &,const key_type &,bool,bool)const
   std::pair<const_iterator, const_iterator>
      bounded_range(const key_type &lower_key, const key_type &upper_key, bool left_closed, bool right_closed) const;

   //! @copydoc ::boost::intrusive::sgtree::bounded_range(const KeyType&,const KeyType&,KeyTypeKeyCompare,bool,bool)const
   template<class KeyType, class KeyTypeKeyCompare>
   std::pair<const_iterator, const_iterator> bounded_range
         (const KeyType& lower_key, const KeyType& upper_key, KeyTypeKeyCompare comp, bool left_closed, bool right_closed) const;

   //! @copydoc ::boost::intrusive::sgtree::s_iterator_to(reference)
   static iterator s_iterator_to(reference value) noexcept;

   //! @copydoc ::boost::intrusive::sgtree::s_iterator_to(const_reference)
   static const_iterator s_iterator_to(const_reference value) noexcept;

   //! @copydoc ::boost::intrusive::sgtree::iterator_to(reference)
   iterator iterator_to(reference value) noexcept;

   //! @copydoc ::boost::intrusive::sgtree::iterator_to(const_reference)const
   const_iterator iterator_to(const_reference value) const noexcept;

   //! @copydoc ::boost::intrusive::sgtree::init_node(reference)
   static void init_node(reference value) noexcept;

   //! @copydoc ::boost::intrusive::sgtree::unlink_leftmost_without_rebalance
   pointer unlink_leftmost_without_rebalance() noexcept;

   //! @copydoc ::boost::intrusive::sgtree::replace_node
   void replace_node(iterator replace_this, reference with_this) noexcept;

   //! @copydoc ::boost::intrusive::sgtree::remove_node
   void remove_node(reference value) noexcept;

   //! @copydoc ::boost::intrusive::sgtree::rebalance
   void rebalance() noexcept;

   //! @copydoc ::boost::intrusive::sgtree::rebalance_subtree
   iterator rebalance_subtree(iterator root) noexcept;

   //! @copydoc ::boost::intrusive::sgtree::balance_factor()
   float balance_factor() const noexcept;

   //! @copydoc ::boost::intrusive::sgtree::balance_factor(float)
   void balance_factor(float new_alpha) noexcept;

   //! @copydoc ::boost::intrusive::rbtree::merge_unique
   template<class ...Options2>
   void merge(sg_set<T, Options2...> &source);

   //! @copydoc ::boost::intrusive::rbtree::merge_unique
   template<class ...Options2>
   void merge(sg_multiset<T, Options2...> &source);

   #else

   template<class Compare2>
   void merge(sg_set_impl<ValueTraits, VoidOrKeyOfValue, Compare2, HeaderHolder> &source)
   {  return tree_type::merge_unique(source);  }

   template<class Compare2>
   void merge(sg_multiset_impl<ValueTraits, VoidOrKeyOfValue, Compare2, HeaderHolder> &source)
   {  return tree_type::merge_unique(source);  }

   #endif   //#ifdef BOOST_INTRUSIVE_DOXYGEN_INVOKED
};

#if defined(BOOST_INTRUSIVE_DOXYGEN_INVOKED)

template<class T, class ...Options>
bool operator!= (const sg_set_impl<T, Options...> &x, const sg_set_impl<T, Options...> &y);

template<class T, class ...Options>
bool operator>(const sg_set_impl<T, Options...> &x, const sg_set_impl<T, Options...> &y);

template<class T, class ...Options>
bool operator<=(const sg_set_impl<T, Options...> &x, const sg_set_impl<T, Options...> &y);

template<class T, class ...Options>
bool operator>=(const sg_set_impl<T, Options...> &x, const sg_set_impl<T, Options...> &y);

template<class T, class ...Options>
void swap(sg_set_impl<T, Options...> &x, sg_set_impl<T, Options...> &y);

#endif   //#if defined(BOOST_INTRUSIVE_DOXYGEN_INVOKED)

//! Helper metafunction to define a \c sg_set that yields to the same type when the
//! same options (either explicitly or implicitly) are used.
template<class T, class ...Options>
struct make_sg_set
{
   /// @cond
   typedef typename pack_options
      < sgtree_defaults,
      Options...
      >::type packed_options;

   using value_traits = detail::get_value_traits_t<T, typename packed_options::proto_value_traits>;

   typedef sg_set_impl
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
class sg_set
   :  public make_sg_set<T,
   Options...
   >::type
{
   typedef typename make_sg_set
      <T,
      Options...
      >::type   Base;

   public:
   typedef typename Base::key_compare        key_compare;
   typedef typename Base::value_traits       value_traits;
   typedef typename Base::iterator           iterator;
   typedef typename Base::const_iterator     const_iterator;

   //Assert if passed value traits are compatible with the type
   static_assert((std::is_same<typename value_traits::value_type, T>::value));

   inline sg_set()
      :  Base()
   {}

   inline explicit sg_set( const key_compare &cmp, const value_traits &v_traits = value_traits())
      :  Base(cmp, v_traits)
   {}

   template<class Iterator>
   inline sg_set( Iterator b, Iterator e
      , const key_compare &cmp = key_compare()
      , const value_traits &v_traits = value_traits())
      :  Base(b, e, cmp, v_traits)
   {}
   sg_set(const sg_set&) = delete;
   sg_set& operator=(const sg_set&) = delete;
   inline sg_set(sg_set&& x) : Base(std::move(x)) {}

   inline sg_set& operator=(sg_set&& x)
   {  return static_cast<sg_set &>(this->Base::operator=(std::move(x)));  }

   template <class Cloner, class Disposer>
   inline void clone_from(const sg_set &src, Cloner cloner, Disposer disposer)
   {  Base::clone_from(src, cloner, disposer);  }

   template <class Cloner, class Disposer>
   inline void clone_from(sg_set&& src, Cloner cloner, Disposer disposer)
   {  Base::clone_from(std::forward<sg_set>(src), cloner, disposer);  }

   inline static sg_set &container_from_end_iterator(iterator end_iterator) noexcept
   {  return static_cast<sg_set &>(Base::container_from_end_iterator(end_iterator));   }

   inline static const sg_set &container_from_end_iterator(const_iterator end_iterator) noexcept
   {  return static_cast<const sg_set &>(Base::container_from_end_iterator(end_iterator));   }

   inline static sg_set &container_from_iterator(iterator it) noexcept
   {  return static_cast<sg_set &>(Base::container_from_iterator(it));   }

   inline static const sg_set &container_from_iterator(const_iterator it) noexcept
   {  return static_cast<const sg_set &>(Base::container_from_iterator(it));   }
};

#endif

//! The class template sg_multiset is an intrusive container, that mimics most of
//! the interface of std::sg_multiset as described in the C++ standard.
//!
//! The template parameter \c T is the type to be managed by the container.
//! The user can specify additional options and if no options are provided
//! default options are used.
//!
//! The container supports the following options:
//! \c base_hook<>/member_hook<>/value_traits<> and
//! \c compare<>.
#if defined(BOOST_INTRUSIVE_DOXYGEN_INVOKED)
template<class T, class ...Options>
#else
template<class ValueTraits, class VoidOrKeyOfValue, class Compare, typename HeaderHolder>
#endif
class sg_multiset_impl
#ifndef BOOST_INTRUSIVE_DOXYGEN_INVOKED
   : public sgtree_impl<ValueTraits, VoidOrKeyOfValue, Compare, HeaderHolder>
#endif
{
   /// @cond
   typedef sgtree_impl<ValueTraits, VoidOrKeyOfValue, Compare, HeaderHolder> tree_type;

   typedef tree_type implementation_defined;
   /// @endcond

   public:
   typedef typename implementation_defined::value_type               value_type;
   typedef typename implementation_defined::key_type                 key_type;
   typedef typename implementation_defined::key_of_value             key_of_value;
   typedef typename implementation_defined::value_traits             value_traits;
   typedef typename implementation_defined::pointer                  pointer;
   typedef typename implementation_defined::const_pointer            const_pointer;
   typedef typename implementation_defined::reference                reference;
   typedef typename implementation_defined::const_reference          const_reference;
   typedef typename implementation_defined::difference_type          difference_type;
   typedef typename implementation_defined::value_compare            value_compare;
   typedef typename implementation_defined::key_compare              key_compare;
   typedef typename implementation_defined::iterator                 iterator;
   typedef typename implementation_defined::const_iterator           const_iterator;
   typedef typename implementation_defined::reverse_iterator         reverse_iterator;
   typedef typename implementation_defined::const_reverse_iterator   const_reverse_iterator;
   typedef typename implementation_defined::insert_commit_data       insert_commit_data;
   typedef typename implementation_defined::node_traits              node_traits;
   typedef typename implementation_defined::node                     node;
   typedef typename implementation_defined::node_ptr                 node_ptr;
   typedef typename implementation_defined::const_node_ptr           const_node_ptr;
   typedef typename implementation_defined::node_algorithms          node_algorithms;

   static const bool constant_time_size = tree_type::constant_time_size;

   public:
   //! @copydoc ::boost::intrusive::sgtree::sgtree()
   sg_multiset_impl()
      :  tree_type()
   {}

   //! @copydoc ::boost::intrusive::sgtree::sgtree(const key_compare &,const value_traits &)
   explicit sg_multiset_impl( const key_compare &cmp, const value_traits &v_traits = value_traits())
      :  tree_type(cmp, v_traits)
   {}

   //! @copydoc ::boost::intrusive::sgtree::sgtree(bool,Iterator,Iterator,const key_compare &,const value_traits &)
   template<class Iterator>
   sg_multiset_impl( Iterator b, Iterator e
                , const key_compare &cmp = key_compare()
                , const value_traits &v_traits = value_traits())
      : tree_type(false, b, e, cmp, v_traits)
   {}
   sg_multiset_impl(const sg_multiset_impl&) = delete;
   sg_multiset_impl& operator=(const sg_multiset_impl&) = delete;
   //! @copydoc ::boost::intrusive::sgtree::sgtree(sgtree &&)
   sg_multiset_impl(sg_multiset_impl&& x) : tree_type(std::move(x)) {}

   //! @copydoc ::boost::intrusive::sgtree::operator=(sgtree &&)
   sg_multiset_impl& operator=(sg_multiset_impl&& x)
   {  return static_cast<sg_multiset_impl&>(tree_type::operator=(std::move(x))); }

   #ifdef BOOST_INTRUSIVE_DOXYGEN_INVOKED
   //! @copydoc ::boost::intrusive::sgtree::~sgtree()
   ~sg_multiset_impl();

   //! @copydoc ::boost::intrusive::sgtree::begin()
   iterator begin() noexcept;

   //! @copydoc ::boost::intrusive::sgtree::begin()const
   const_iterator begin() const noexcept;

   //! @copydoc ::boost::intrusive::sgtree::cbegin()const
   const_iterator cbegin() const noexcept;

   //! @copydoc ::boost::intrusive::sgtree::end()
   iterator end() noexcept;

   //! @copydoc ::boost::intrusive::sgtree::end()const
   const_iterator end() const noexcept;

   //! @copydoc ::boost::intrusive::sgtree::cend()const
   const_iterator cend() const noexcept;

   //! @copydoc ::boost::intrusive::sgtree::rbegin()
   reverse_iterator rbegin() noexcept;

   //! @copydoc ::boost::intrusive::sgtree::rbegin()const
   const_reverse_iterator rbegin() const noexcept;

   //! @copydoc ::boost::intrusive::sgtree::crbegin()const
   const_reverse_iterator crbegin() const noexcept;

   //! @copydoc ::boost::intrusive::sgtree::rend()
   reverse_iterator rend() noexcept;

   //! @copydoc ::boost::intrusive::sgtree::rend()const
   const_reverse_iterator rend() const noexcept;

   //! @copydoc ::boost::intrusive::sgtree::crend()const
   const_reverse_iterator crend() const noexcept;

   //! @copydoc ::boost::intrusive::sgtree::root()
   iterator root() noexcept;

   //! @copydoc ::boost::intrusive::sgtree::root()const
   const_iterator root() const noexcept;

   //! @copydoc ::boost::intrusive::sgtree::croot()const
   const_iterator croot() const noexcept;

   //! @copydoc ::boost::intrusive::sgtree::container_from_end_iterator(iterator)
   static sg_multiset_impl &container_from_end_iterator(iterator end_iterator) noexcept;

   //! @copydoc ::boost::intrusive::sgtree::container_from_end_iterator(const_iterator)
   static const sg_multiset_impl &container_from_end_iterator(const_iterator end_iterator) noexcept;

   //! @copydoc ::boost::intrusive::sgtree::container_from_iterator(iterator)
   static sg_multiset_impl &container_from_iterator(iterator it) noexcept;

   //! @copydoc ::boost::intrusive::sgtree::container_from_iterator(const_iterator)
   static const sg_multiset_impl &container_from_iterator(const_iterator it) noexcept;

   //! @copydoc ::boost::intrusive::sgtree::key_comp()const
   key_compare key_comp() const;

   //! @copydoc ::boost::intrusive::sgtree::value_comp()const
   value_compare value_comp() const;

   //! @copydoc ::boost::intrusive::sgtree::empty()const
   bool empty() const noexcept;

   //! @copydoc ::boost::intrusive::sgtree::size()const
   std::size_t size() const noexcept;

   //! @copydoc ::boost::intrusive::sgtree::swap
   void swap(sg_multiset_impl& other);

   //! @copydoc ::boost::intrusive::sgtree::clone_from(const sgtree&,Cloner,Disposer)
   template <class Cloner, class Disposer>
   void clone_from(const sg_multiset_impl &src, Cloner cloner, Disposer disposer);

   #else

   using tree_type::clone_from;

   #endif   //#ifdef BOOST_INTRUSIVE_DOXYGEN_INVOKED

   //! @copydoc ::boost::intrusive::sgtree::clone_from(sgtree&&,Cloner,Disposer)
   template <class Cloner, class Disposer>
   void clone_from(sg_multiset_impl&& src, Cloner cloner, Disposer disposer)
   {  tree_type::clone_from(std::forward<sg_multiset_impl>(src), cloner, disposer);  }

   //! @copydoc ::boost::intrusive::sgtree::insert_equal(reference)
   iterator insert(reference value)
   {  return tree_type::insert_equal(value);  }

   //! @copydoc ::boost::intrusive::sgtree::insert_equal(const_iterator,reference)
   iterator insert(const_iterator hint, reference value)
   {  return tree_type::insert_equal(hint, value);  }

   //! @copydoc ::boost::intrusive::sgtree::insert_equal(Iterator,Iterator)
   template<class Iterator>
   void insert(Iterator b, Iterator e)
   {  tree_type::insert_equal(b, e);  }

   #ifdef BOOST_INTRUSIVE_DOXYGEN_INVOKED
   //! @copydoc ::boost::intrusive::sgtree::insert_before
   iterator insert_before(const_iterator pos, reference value) noexcept;

   //! @copydoc ::boost::intrusive::sgtree::push_back
   void push_back(reference value) noexcept;

   //! @copydoc ::boost::intrusive::sgtree::push_front
   void push_front(reference value) noexcept;

   //! @copydoc ::boost::intrusive::sgtree::erase(const_iterator)
   iterator erase(const_iterator i) noexcept;

   //! @copydoc ::boost::intrusive::sgtree::erase(const_iterator,const_iterator)
   iterator erase(const_iterator b, const_iterator e) noexcept;

   //! @copydoc ::boost::intrusive::sgtree::erase(const key_type &)
   std::size_t erase(const key_type &key);

   //! @copydoc ::boost::intrusive::sgtree::erase(const KeyType&,KeyTypeKeyCompare)
   template<class KeyType, class KeyTypeKeyCompare>
   std::size_t erase(const KeyType& key, KeyTypeKeyCompare comp);

   //! @copydoc ::boost::intrusive::sgtree::erase_and_dispose(const_iterator,Disposer)
   template<class Disposer>
   iterator erase_and_dispose(const_iterator i, Disposer disposer) noexcept;

   //! @copydoc ::boost::intrusive::sgtree::erase_and_dispose(const_iterator,const_iterator,Disposer)
   template<class Disposer>
   iterator erase_and_dispose(const_iterator b, const_iterator e, Disposer disposer) noexcept;

   //! @copydoc ::boost::intrusive::sgtree::erase_and_dispose(const key_type &, Disposer)
   template<class Disposer>
   std::size_t erase_and_dispose(const key_type &key, Disposer disposer);

   //! @copydoc ::boost::intrusive::sgtree::erase_and_dispose(const KeyType&,KeyTypeKeyCompare,Disposer)
   template<class KeyType, class KeyTypeKeyCompare, class Disposer>
   std::size_t erase_and_dispose(const KeyType& key, KeyTypeKeyCompare comp, Disposer disposer);

   //! @copydoc ::boost::intrusive::sgtree::clear
   void clear() noexcept;

   //! @copydoc ::boost::intrusive::sgtree::clear_and_dispose
   template<class Disposer>
   void clear_and_dispose(Disposer disposer) noexcept;

   //! @copydoc ::boost::intrusive::sgtree::count(const key_type &)const
   std::size_t count(const key_type &key) const;

   //! @copydoc ::boost::intrusive::sgtree::count(const KeyType&,KeyTypeKeyCompare)const
   template<class KeyType, class KeyTypeKeyCompare>
   std::size_t count(const KeyType& key, KeyTypeKeyCompare comp) const;

   //! @copydoc ::boost::intrusive::sgtree::lower_bound(const key_type &)
   iterator lower_bound(const key_type &key);

   //! @copydoc ::boost::intrusive::sgtree::lower_bound(const KeyType&,KeyTypeKeyCompare)
   template<class KeyType, class KeyTypeKeyCompare>
   iterator lower_bound(const KeyType& key, KeyTypeKeyCompare comp);

   //! @copydoc ::boost::intrusive::sgtree::lower_bound(const key_type &)const
   const_iterator lower_bound(const key_type &key) const;

   //! @copydoc ::boost::intrusive::sgtree::lower_bound(const KeyType&,KeyTypeKeyCompare)const
   template<class KeyType, class KeyTypeKeyCompare>
   const_iterator lower_bound(const KeyType& key, KeyTypeKeyCompare comp) const;

   //! @copydoc ::boost::intrusive::sgtree::upper_bound(const key_type &)
   iterator upper_bound(const key_type &key);

   //! @copydoc ::boost::intrusive::sgtree::upper_bound(const KeyType&,KeyTypeKeyCompare)
   template<class KeyType, class KeyTypeKeyCompare>
   iterator upper_bound(const KeyType& key, KeyTypeKeyCompare comp);

   //! @copydoc ::boost::intrusive::sgtree::upper_bound(const key_type &)const
   const_iterator upper_bound(const key_type &key) const;

   //! @copydoc ::boost::intrusive::sgtree::upper_bound(const KeyType&,KeyTypeKeyCompare)const
   template<class KeyType, class KeyTypeKeyCompare>
   const_iterator upper_bound(const KeyType& key, KeyTypeKeyCompare comp) const;

   //! @copydoc ::boost::intrusive::sgtree::find(const key_type &)
   iterator find(const key_type &key);

   //! @copydoc ::boost::intrusive::sgtree::find(const KeyType&,KeyTypeKeyCompare)
   template<class KeyType, class KeyTypeKeyCompare>
   iterator find(const KeyType& key, KeyTypeKeyCompare comp);

   //! @copydoc ::boost::intrusive::sgtree::find(const key_type &)const
   const_iterator find(const key_type &key) const;

   //! @copydoc ::boost::intrusive::sgtree::find(const KeyType&,KeyTypeKeyCompare)const
   template<class KeyType, class KeyTypeKeyCompare>
   const_iterator find(const KeyType& key, KeyTypeKeyCompare comp) const;

   //! @copydoc ::boost::intrusive::sgtree::equal_range(const key_type &)
   std::pair<iterator,iterator> equal_range(const key_type &key);

   //! @copydoc ::boost::intrusive::sgtree::equal_range(const KeyType&,KeyTypeKeyCompare)
   template<class KeyType, class KeyTypeKeyCompare>
   std::pair<iterator,iterator> equal_range(const KeyType& key, KeyTypeKeyCompare comp);

   //! @copydoc ::boost::intrusive::sgtree::equal_range(const key_type &)const
   std::pair<const_iterator, const_iterator>
      equal_range(const key_type &key) const;

   //! @copydoc ::boost::intrusive::sgtree::equal_range(const KeyType&,KeyTypeKeyCompare)const
   template<class KeyType, class KeyTypeKeyCompare>
   std::pair<const_iterator, const_iterator>
      equal_range(const KeyType& key, KeyTypeKeyCompare comp) const;

   //! @copydoc ::boost::intrusive::sgtree::bounded_range(const key_type &,const key_type &,bool,bool)
   std::pair<iterator,iterator> bounded_range
      (const key_type &lower_key, const key_type &upper_key, bool left_closed, bool right_closed);

   //! @copydoc ::boost::intrusive::sgtree::bounded_range(const KeyType&,const KeyType&,KeyTypeKeyCompare,bool,bool)
   template<class KeyType, class KeyTypeKeyCompare>
   std::pair<iterator,iterator> bounded_range
      (const KeyType& lower_key, const KeyType& upper_key, KeyTypeKeyCompare comp, bool left_closed, bool right_closed);

   //! @copydoc ::boost::intrusive::sgtree::bounded_range(const key_type &,const key_type &,bool,bool)const
   std::pair<const_iterator, const_iterator>
      bounded_range(const key_type &lower_key, const key_type &upper_key, bool left_closed, bool right_closed) const;

   //! @copydoc ::boost::intrusive::sgtree::bounded_range(const KeyType&,const KeyType&,KeyTypeKeyCompare,bool,bool)const
   template<class KeyType, class KeyTypeKeyCompare>
   std::pair<const_iterator, const_iterator> bounded_range
         (const KeyType& lower_key, const KeyType& upper_key, KeyTypeKeyCompare comp, bool left_closed, bool right_closed) const;

   //! @copydoc ::boost::intrusive::sgtree::s_iterator_to(reference)
   static iterator s_iterator_to(reference value) noexcept;

   //! @copydoc ::boost::intrusive::sgtree::s_iterator_to(const_reference)
   static const_iterator s_iterator_to(const_reference value) noexcept;

   //! @copydoc ::boost::intrusive::sgtree::iterator_to(reference)
   iterator iterator_to(reference value) noexcept;

   //! @copydoc ::boost::intrusive::sgtree::iterator_to(const_reference)const
   const_iterator iterator_to(const_reference value) const noexcept;

   //! @copydoc ::boost::intrusive::sgtree::init_node(reference)
   static void init_node(reference value) noexcept;

   //! @copydoc ::boost::intrusive::sgtree::unlink_leftmost_without_rebalance
   pointer unlink_leftmost_without_rebalance() noexcept;

   //! @copydoc ::boost::intrusive::sgtree::replace_node
   void replace_node(iterator replace_this, reference with_this) noexcept;

   //! @copydoc ::boost::intrusive::sgtree::remove_node
   void remove_node(reference value) noexcept;

   //! @copydoc ::boost::intrusive::sgtree::rebalance
   void rebalance() noexcept;

   //! @copydoc ::boost::intrusive::sgtree::rebalance_subtree
   iterator rebalance_subtree(iterator root) noexcept;

   //! @copydoc ::boost::intrusive::sgtree::balance_factor()
   float balance_factor() const noexcept;

   //! @copydoc ::boost::intrusive::sgtree::balance_factor(float)
   void balance_factor(float new_alpha) noexcept;

   //! @copydoc ::boost::intrusive::treap::merge_unique
   template<class ...Options2>
   void merge(sg_multiset<T, Options2...> &source);

   //! @copydoc ::boost::intrusive::treap::merge_unique
   template<class ...Options2>
   void merge(sg_set<T, Options2...> &source);

   #else

   template<class Compare2>
   void merge(sg_multiset_impl<ValueTraits, VoidOrKeyOfValue, Compare2, HeaderHolder> &source)
   {  return tree_type::merge_equal(source);  }

   template<class Compare2>
   void merge(sg_set_impl<ValueTraits, VoidOrKeyOfValue, Compare2, HeaderHolder> &source)
   {  return tree_type::merge_equal(source);  }

   #endif   //#ifdef BOOST_INTRUSIVE_DOXYGEN_INVOKED
};

#if defined(BOOST_INTRUSIVE_DOXYGEN_INVOKED)

template<class T, class ...Options>
bool operator!= (const sg_multiset_impl<T, Options...> &x, const sg_multiset_impl<T, Options...> &y);

template<class T, class ...Options>
bool operator>(const sg_multiset_impl<T, Options...> &x, const sg_multiset_impl<T, Options...> &y);

template<class T, class ...Options>
bool operator<=(const sg_multiset_impl<T, Options...> &x, const sg_multiset_impl<T, Options...> &y);

template<class T, class ...Options>
bool operator>=(const sg_multiset_impl<T, Options...> &x, const sg_multiset_impl<T, Options...> &y);

template<class T, class ...Options>
void swap(sg_multiset_impl<T, Options...> &x, sg_multiset_impl<T, Options...> &y);

#endif   //#if defined(BOOST_INTRUSIVE_DOXYGEN_INVOKED)

//! Helper metafunction to define a \c sg_multiset that yields to the same type when the
//! same options (either explicitly or implicitly) are used.
template<class T, class ...Options>
struct make_sg_multiset
{
   /// @cond
   typedef typename pack_options
      < sgtree_defaults,
      Options...
      >::type packed_options;

   using value_traits = detail::get_value_traits_t<T, typename packed_options::proto_value_traits>;

   typedef sg_multiset_impl
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
class sg_multiset
   :  public make_sg_multiset<T,
      Options...
      >::type
{
   typedef typename make_sg_multiset<T,
      Options...
      >::type   Base;

   public:
   typedef typename Base::key_compare        key_compare;
   typedef typename Base::value_traits       value_traits;
   typedef typename Base::iterator           iterator;
   typedef typename Base::const_iterator     const_iterator;

   //Assert if passed value traits are compatible with the type
   static_assert((std::is_same<typename value_traits::value_type, T>::value));

   inline sg_multiset()
      :  Base()
   {}

   inline explicit sg_multiset( const key_compare &cmp, const value_traits &v_traits = value_traits())
      :  Base(cmp, v_traits)
   {}

   template<class Iterator>
   inline sg_multiset( Iterator b, Iterator e
           , const key_compare &cmp = key_compare()
           , const value_traits &v_traits = value_traits())
      :  Base(b, e, cmp, v_traits)
   {}

   sg_multiset(const sg_multiset&) = delete;
   sg_multiset& operator=(const sg_multiset&) = delete;
   inline sg_multiset(sg_multiset&& x) : Base(std::move(x)) {}

   inline sg_multiset& operator=(sg_multiset&& x)
   {  return static_cast<sg_multiset &>(this->Base::operator=(std::move(x)));  }

   template <class Cloner, class Disposer>
   inline void clone_from(const sg_multiset &src, Cloner cloner, Disposer disposer)
   {  Base::clone_from(src, cloner, disposer);  }

   template <class Cloner, class Disposer>
   inline void clone_from(sg_multiset&& src, Cloner cloner, Disposer disposer)
   {  Base::clone_from(std::forward<sg_multiset>(src), cloner, disposer);  }

   inline static sg_multiset &container_from_end_iterator(iterator end_iterator) noexcept
   {  return static_cast<sg_multiset &>(Base::container_from_end_iterator(end_iterator));   }

   inline static const sg_multiset &container_from_end_iterator(const_iterator end_iterator) noexcept
   {  return static_cast<const sg_multiset &>(Base::container_from_end_iterator(end_iterator));   }

   inline static sg_multiset &container_from_iterator(iterator it) noexcept
   {  return static_cast<sg_multiset &>(Base::container_from_iterator(it));   }

   inline static const sg_multiset &container_from_iterator(const_iterator it) noexcept
   {  return static_cast<const sg_multiset &>(Base::container_from_iterator(it));   }
};

#endif

} //namespace intrusive
} //namespace boost

#endif //BOOST_INTRUSIVE_SG_SET_HPP
