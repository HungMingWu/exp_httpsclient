/////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Olaf Krzikalla 2004-2006.
// (C) Copyright Ion Gaztanaga  2006-2014
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/intrusive for documentation.
//
/////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_INTRUSIVE_SET_HPP
#define BOOST_INTRUSIVE_SET_HPP

#include <boost/intrusive/intrusive_fwd.hpp>

#include <boost/intrusive/detail/mpl.hpp>
#include <boost/intrusive/rbtree.hpp>

#if defined(BOOST_HAS_PRAGMA_ONCE)
#  pragma once
#endif

namespace boost {
namespace intrusive {

#if !defined(BOOST_INTRUSIVE_DOXYGEN_INVOKED)
template<class ValueTraits, class VoidOrKeyOfValue, class Compare, bool ConstantTimeSize, typename HeaderHolder>
class multiset_impl;
#endif

//! The class template set is an intrusive container, that mimics most of
//! the interface of std::set as described in the C++ standard.
//!
//! The template parameter \c T is the type to be managed by the container.
//! The user can specify additional options and if no options are provided
//! default options are used.
//!
//! The container supports the following options:
//! \c base_hook<>/member_hook<>/value_traits<>,
//! \c constant_time_size<>, and
//! \c compare<>.
#if defined(BOOST_INTRUSIVE_DOXYGEN_INVOKED)
template<class T, class ...Options>
#else
template<class ValueTraits, class VoidOrKeyOfValue, class Compare, bool ConstantTimeSize, typename HeaderHolder>
#endif
class set_impl
#ifndef BOOST_INTRUSIVE_DOXYGEN_INVOKED
   : public bstree_impl<ValueTraits, VoidOrKeyOfValue, Compare, ConstantTimeSize, RbTreeAlgorithms, HeaderHolder>
#endif
{
   /// @cond
   typedef bstree_impl<ValueTraits, VoidOrKeyOfValue, Compare, ConstantTimeSize, RbTreeAlgorithms, HeaderHolder> tree_type;

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
   //! @copydoc ::boost::intrusive::rbtree::rbtree()
   set_impl()
      :  tree_type()
   {}

   //! @copydoc ::boost::intrusive::rbtree::rbtree(const key_compare &,const value_traits &)
   explicit set_impl( const key_compare &cmp, const value_traits &v_traits)
      :  tree_type(cmp, v_traits)
   {}

   //! @copydoc ::boost::intrusive::rbtree::rbtree(bool,Iterator,Iterator,const key_compare &,const value_traits &)
   template<class Iterator>
   set_impl( Iterator b, Iterator e
           , const key_compare &cmp = key_compare()
           , const value_traits &v_traits = value_traits())
      : tree_type(true, b, e, cmp, v_traits)
   {}
   set_impl(const set_impl&) = delete;
   set_impl& operator=(const set_impl&) = delete;
   //! @copydoc ::boost::intrusive::rbtree::rbtree(rbtree &&)
   set_impl(set_impl&& x) : tree_type(std::move(x)) {}

   //! @copydoc ::boost::intrusive::rbtree::operator=(rbtree &&)
   set_impl& operator=(set_impl&& x)
   {  return static_cast<set_impl&>(tree_type::operator=(std::move(x))); }

   #ifdef BOOST_INTRUSIVE_DOXYGEN_INVOKED
   //! @copydoc ::boost::intrusive::rbtree::~rbtree()
   ~set_impl();

   //! @copydoc ::boost::intrusive::rbtree::begin()
   iterator begin();

   //! @copydoc ::boost::intrusive::rbtree::begin()const
   const_iterator begin() const;

   //! @copydoc ::boost::intrusive::rbtree::cbegin()const
   const_iterator cbegin() const;

   //! @copydoc ::boost::intrusive::rbtree::end()
   iterator end();

   //! @copydoc ::boost::intrusive::rbtree::end()const
   const_iterator end() const;

   //! @copydoc ::boost::intrusive::rbtree::cend()const
   const_iterator cend() const;

   //! @copydoc ::boost::intrusive::rbtree::rbegin()
   reverse_iterator rbegin();

   //! @copydoc ::boost::intrusive::rbtree::rbegin()const
   const_reverse_iterator rbegin() const;

   //! @copydoc ::boost::intrusive::rbtree::crbegin()const
   const_reverse_iterator crbegin() const;

   //! @copydoc ::boost::intrusive::rbtree::rend()
   reverse_iterator rend();

   //! @copydoc ::boost::intrusive::rbtree::rend()const
   const_reverse_iterator rend() const;

   //! @copydoc ::boost::intrusive::rbtree::crend()const
   const_reverse_iterator crend() const;

   //! @copydoc ::boost::intrusive::rbtree::root()
   iterator root();

   //! @copydoc ::boost::intrusive::rbtree::root()const
   const_iterator root() const;

   //! @copydoc ::boost::intrusive::rbtree::croot()const
   const_iterator croot() const;

   //! @copydoc ::boost::intrusive::rbtree::container_from_end_iterator(iterator)
   static set_impl &container_from_end_iterator(iterator end_iterator);

   //! @copydoc ::boost::intrusive::rbtree::container_from_end_iterator(const_iterator)
   static const set_impl &container_from_end_iterator(const_iterator end_iterator);

   //! @copydoc ::boost::intrusive::rbtree::container_from_iterator(iterator)
   static set_impl &container_from_iterator(iterator it);

   //! @copydoc ::boost::intrusive::rbtree::container_from_iterator(const_iterator)
   static const set_impl &container_from_iterator(const_iterator it);

   //! @copydoc ::boost::intrusive::rbtree::key_comp()const
   key_compare key_comp() const;

   //! @copydoc ::boost::intrusive::rbtree::value_comp()const
   value_compare value_comp() const;

   //! @copydoc ::boost::intrusive::rbtree::empty()const
   bool empty() const;

   //! @copydoc ::boost::intrusive::rbtree::size()const
   std::size_t size() const;

   //! @copydoc ::boost::intrusive::rbtree::swap
   void swap(set_impl& other);

   //! @copydoc ::boost::intrusive::rbtree::clone_from(const rbtree&,Cloner,Disposer)
   template <class Cloner, class Disposer>
   void clone_from(const set_impl &src, Cloner cloner, Disposer disposer);

   #else

   using tree_type::clone_from;

   #endif   //#ifdef BOOST_INTRUSIVE_DOXYGEN_INVOKED

   //! @copydoc ::boost::intrusive::rbtree::clone_from(rbtree&&,Cloner,Disposer)
   template <class Cloner, class Disposer>
   void clone_from(set_impl&& src, Cloner cloner, Disposer disposer)
   {  tree_type::clone_from(std::forward<set_impl>(src), cloner, disposer);  }

   //! @copydoc ::boost::intrusive::rbtree::insert_unique(reference)
   std::pair<iterator, bool> insert(reference value)
   {  return tree_type::insert_unique(value);  }

   //! @copydoc ::boost::intrusive::rbtree::insert_unique(const_iterator,reference)
   iterator insert(const_iterator hint, reference value)
   {  return tree_type::insert_unique(hint, value);  }

   //! @copydoc ::boost::intrusive::rbtree::insert_unique_check(const key_type&,insert_commit_data&)
   std::pair<iterator, bool> insert_check
      (const key_type &key, insert_commit_data &commit_data)
   {  return tree_type::insert_unique_check(key, commit_data); }

   //! @copydoc ::boost::intrusive::rbtree::insert_unique_check(const_iterator,const key_type&,insert_commit_data&)
   std::pair<iterator, bool> insert_check
      (const_iterator hint, const key_type &key
      ,insert_commit_data &commit_data)
   {  return tree_type::insert_unique_check(hint, key, commit_data); }

   //! @copydoc ::boost::intrusive::rbtree::insert_unique_check(const KeyType&,KeyTypeKeyCompare,insert_commit_data&)
   template<class KeyType, class KeyTypeKeyCompare>
   std::pair<iterator, bool> insert_check
      (const KeyType &key, KeyTypeKeyCompare comp, insert_commit_data &commit_data)
   {  return tree_type::insert_unique_check(key, comp, commit_data); }

   //! @copydoc ::boost::intrusive::rbtree::insert_unique_check(const_iterator,const KeyType&,KeyTypeKeyCompare,insert_commit_data&)
   template<class KeyType, class KeyTypeKeyCompare>
   std::pair<iterator, bool> insert_check
      (const_iterator hint, const KeyType &key
      ,KeyTypeKeyCompare comp, insert_commit_data &commit_data)
   {  return tree_type::insert_unique_check(hint, key, comp, commit_data); }

   //! @copydoc ::boost::intrusive::rbtree::insert_unique(Iterator,Iterator)
   template<class Iterator>
   void insert(Iterator b, Iterator e)
   {  tree_type::insert_unique(b, e);  }

   //! @copydoc ::boost::intrusive::rbtree::insert_unique_commit
   iterator insert_commit(reference value, const insert_commit_data &commit_data)
   {  return tree_type::insert_unique_commit(value, commit_data);  }

   #ifdef BOOST_INTRUSIVE_DOXYGEN_INVOKED
   //! @copydoc ::boost::intrusive::rbtree::insert_before
   iterator insert_before(const_iterator pos, reference value);

   //! @copydoc ::boost::intrusive::rbtree::push_back
   void push_back(reference value);

   //! @copydoc ::boost::intrusive::rbtree::push_front
   void push_front(reference value);

   //! @copydoc ::boost::intrusive::rbtree::erase(const_iterator)
   iterator erase(const_iterator i);

   //! @copydoc ::boost::intrusive::rbtree::erase(const_iterator,const_iterator)
   iterator erase(const_iterator b, const_iterator e);

   //! @copydoc ::boost::intrusive::rbtree::erase(const key_type &)
   std::size_t erase(const key_type &key);

   //! @copydoc ::boost::intrusive::rbtree::erase(const KeyType&,KeyTypeKeyCompare)
   template<class KeyType, class KeyTypeKeyCompare>
   std::size_t erase(const KeyType& key, KeyTypeKeyCompare comp);

   //! @copydoc ::boost::intrusive::rbtree::erase_and_dispose(const_iterator,Disposer)
   template<class Disposer>
   iterator erase_and_dispose(const_iterator i, Disposer disposer);

   //! @copydoc ::boost::intrusive::rbtree::erase_and_dispose(const_iterator,const_iterator,Disposer)
   template<class Disposer>
   iterator erase_and_dispose(const_iterator b, const_iterator e, Disposer disposer);

   //! @copydoc ::boost::intrusive::rbtree::erase_and_dispose(const key_type &, Disposer)
   template<class Disposer>
   std::size_t erase_and_dispose(const key_type &key, Disposer disposer);

   //! @copydoc ::boost::intrusive::rbtree::erase_and_dispose(const KeyType&,KeyTypeKeyCompare,Disposer)
   template<class KeyType, class KeyTypeKeyCompare, class Disposer>
   std::size_t erase_and_dispose(const KeyType& key, KeyTypeKeyCompare comp, Disposer disposer);

   //! @copydoc ::boost::intrusive::rbtree::clear
   void clear();

   //! @copydoc ::boost::intrusive::rbtree::clear_and_dispose
   template<class Disposer>
   void clear_and_dispose(Disposer disposer);

   #endif   //   #ifdef BOOST_INTRUSIVE_DOXYGEN_INVOKED

   //! @copydoc ::boost::intrusive::rbtree::count(const key_type &)const
   std::size_t count(const key_type &key) const
   {  return static_cast<std::size_t>(this->tree_type::find(key) != this->tree_type::cend()); }

   //! @copydoc ::boost::intrusive::rbtree::count(const KeyType&,KeyTypeKeyCompare)const
   template<class KeyType, class KeyTypeKeyCompare>
   std::size_t count(const KeyType& key, KeyTypeKeyCompare comp) const
   {  return static_cast<std::size_t>(this->tree_type::find(key, comp) != this->tree_type::cend()); }

   #ifdef BOOST_INTRUSIVE_DOXYGEN_INVOKED

   //! @copydoc ::boost::intrusive::rbtree::lower_bound(const key_type &)
   iterator lower_bound(const key_type &key);

   //! @copydoc ::boost::intrusive::rbtree::lower_bound(const KeyType&,KeyTypeKeyCompare)
   template<class KeyType, class KeyTypeKeyCompare>
   iterator lower_bound(const KeyType& key, KeyTypeKeyCompare comp);

   //! @copydoc ::boost::intrusive::rbtree::lower_bound(const key_type &)const
   const_iterator lower_bound(const key_type &key) const;

   //! @copydoc ::boost::intrusive::rbtree::lower_bound(const KeyType&,KeyTypeKeyCompare)const
   template<class KeyType, class KeyTypeKeyCompare>
   const_iterator lower_bound(const KeyType& key, KeyTypeKeyCompare comp) const;

   //! @copydoc ::boost::intrusive::rbtree::upper_bound(const key_type &)
   iterator upper_bound(const key_type &key);

   //! @copydoc ::boost::intrusive::rbtree::upper_bound(const KeyType&,KeyTypeKeyCompare)
   template<class KeyType, class KeyTypeKeyCompare>
   iterator upper_bound(const KeyType& key, KeyTypeKeyCompare comp);

   //! @copydoc ::boost::intrusive::rbtree::upper_bound(const key_type &)const
   const_iterator upper_bound(const key_type &key) const;

   //! @copydoc ::boost::intrusive::rbtree::upper_bound(const KeyType&,KeyTypeKeyCompare)const
   template<class KeyType, class KeyTypeKeyCompare>
   const_iterator upper_bound(const KeyType& key, KeyTypeKeyCompare comp) const;

   //! @copydoc ::boost::intrusive::rbtree::find(const key_type &)
   iterator find(const key_type &key);

   //! @copydoc ::boost::intrusive::rbtree::find(const KeyType&,KeyTypeKeyCompare)
   template<class KeyType, class KeyTypeKeyCompare>
   iterator find(const KeyType& key, KeyTypeKeyCompare comp);

   //! @copydoc ::boost::intrusive::rbtree::find(const key_type &)const
   const_iterator find(const key_type &key) const;

   //! @copydoc ::boost::intrusive::rbtree::find(const KeyType&,KeyTypeKeyCompare)const
   template<class KeyType, class KeyTypeKeyCompare>
   const_iterator find(const KeyType& key, KeyTypeKeyCompare comp) const;

   #endif   //   #ifdef BOOST_INTRUSIVE_DOXYGEN_INVOKED

   //! @copydoc ::boost::intrusive::rbtree::equal_range(const key_type &)
   std::pair<iterator,iterator> equal_range(const key_type &key)
   {  return this->tree_type::lower_bound_range(key); }

   //! @copydoc ::boost::intrusive::rbtree::equal_range(const KeyType&,KeyTypeKeyCompare)
   template<class KeyType, class KeyTypeKeyCompare>
   std::pair<iterator,iterator> equal_range(const KeyType& key, KeyTypeKeyCompare comp)
   {  return this->tree_type::equal_range(key, comp); }

   //! @copydoc ::boost::intrusive::rbtree::equal_range(const key_type &)const
   std::pair<const_iterator, const_iterator>
      equal_range(const key_type &key) const
   {  return this->tree_type::lower_bound_range(key); }

   //! @copydoc ::boost::intrusive::rbtree::equal_range(const KeyType&,KeyTypeKeyCompare)const
   template<class KeyType, class KeyTypeKeyCompare>
   std::pair<const_iterator, const_iterator>
      equal_range(const KeyType& key, KeyTypeKeyCompare comp) const
   {  return this->tree_type::equal_range(key, comp); }

   #ifdef BOOST_INTRUSIVE_DOXYGEN_INVOKED

   //! @copydoc ::boost::intrusive::rbtree::bounded_range(const key_type &,const key_type &,bool,bool)
   std::pair<iterator,iterator> bounded_range
      (const key_type &lower_key, const key_type &upper_key, bool left_closed, bool right_closed);

   //! @copydoc ::boost::intrusive::rbtree::bounded_range(const KeyType&,const KeyType&,KeyTypeKeyCompare,bool,bool)
   template<class KeyType, class KeyTypeKeyCompare>
   std::pair<iterator,iterator> bounded_range
      (const KeyType& lower_key, const KeyType& upper_key, KeyTypeKeyCompare comp, bool left_closed, bool right_closed);

   //! @copydoc ::boost::intrusive::rbtree::bounded_range(const key_type &,const key_type &,bool,bool)const
   std::pair<const_iterator, const_iterator>
      bounded_range(const key_type &lower_key, const key_type &upper_key, bool left_closed, bool right_closed) const;

   //! @copydoc ::boost::intrusive::rbtree::bounded_range(const KeyType&,const KeyType&,KeyTypeKeyCompare,bool,bool)const
   template<class KeyType, class KeyTypeKeyCompare>
   std::pair<const_iterator, const_iterator> bounded_range
         (const KeyType& lower_key, const KeyType& upper_key, KeyTypeKeyCompare comp, bool left_closed, bool right_closed) const;

   //! @copydoc ::boost::intrusive::rbtree::s_iterator_to(reference)
   static iterator s_iterator_to(reference value);

   //! @copydoc ::boost::intrusive::rbtree::s_iterator_to(const_reference)
   static const_iterator s_iterator_to(const_reference value);

   //! @copydoc ::boost::intrusive::rbtree::iterator_to(reference)
   iterator iterator_to(reference value);

   //! @copydoc ::boost::intrusive::rbtree::iterator_to(const_reference)const
   const_iterator iterator_to(const_reference value) const;

   //! @copydoc ::boost::intrusive::rbtree::init_node(reference)
   static void init_node(reference value);

   //! @copydoc ::boost::intrusive::rbtree::unlink_leftmost_without_rebalance
   pointer unlink_leftmost_without_rebalance();

   //! @copydoc ::boost::intrusive::rbtree::replace_node
   void replace_node(iterator replace_this, reference with_this);

   //! @copydoc ::boost::intrusive::rbtree::remove_node
   void remove_node(reference value);

   //! @copydoc ::boost::intrusive::rbtree::merge_unique
   template<class ...Options2>
   void merge(set<T, Options2...> &source);

   //! @copydoc ::boost::intrusive::rbtree::merge_unique
   template<class ...Options2>
   void merge(multiset<T, Options2...> &source);

   #else

   template<class Compare2>
   void merge(set_impl<ValueTraits, VoidOrKeyOfValue, Compare2, ConstantTimeSize, HeaderHolder> &source)
   {  return tree_type::merge_unique(source);  }


   template<class Compare2>
   void merge(multiset_impl<ValueTraits, VoidOrKeyOfValue, Compare2, ConstantTimeSize, HeaderHolder> &source)
   {  return tree_type::merge_unique(source);  }

   #endif   //#ifdef BOOST_INTRUSIVE_DOXYGEN_INVOKED
};

#if defined(BOOST_INTRUSIVE_DOXYGEN_INVOKED)

template<class T, class ...Options>
bool operator!= (const set_impl<T, Options...> &x, const set_impl<T, Options...> &y);

template<class T, class ...Options>
bool operator>(const set_impl<T, Options...> &x, const set_impl<T, Options...> &y);

template<class T, class ...Options>
bool operator<=(const set_impl<T, Options...> &x, const set_impl<T, Options...> &y);

template<class T, class ...Options>
bool operator>=(const set_impl<T, Options...> &x, const set_impl<T, Options...> &y);

template<class T, class ...Options>
void swap(set_impl<T, Options...> &x, set_impl<T, Options...> &y);

#endif   //#if defined(BOOST_INTRUSIVE_DOXYGEN_INVOKED)

//! Helper metafunction to define a \c set that yields to the same type when the
//! same options (either explicitly or implicitly) are used.
template<class T, class ...Options>
struct make_set
{
   /// @cond
   typedef typename pack_options
      < rbtree_defaults,
      Options...
      >::type packed_options;

   using value_traits = detail::get_value_traits_t<T, typename packed_options::proto_value_traits>;

   typedef set_impl
         < value_traits
         , typename packed_options::key_of_value
         , typename packed_options::compare
         , packed_options::constant_time_size
         , typename packed_options::header_holder_type
         > implementation_defined;
   /// @endcond
   typedef implementation_defined type;
};

#ifndef BOOST_INTRUSIVE_DOXYGEN_INVOKED
template<class T, class ...Options>
class set
   :  public make_set<T,
   Options...
   >::type
{
   typedef typename make_set
      <T,
      Options...
      >::type   Base;

   public:
   typedef typename Base::key_compare        key_compare;
   typedef typename Base::value_traits       value_traits;
   typedef typename Base::iterator           iterator;
   typedef typename Base::const_iterator     const_iterator;

   //Assert if passed value traits are compatible with the type
   static_assert(std::is_same_v<typename value_traits::value_type, T>);

   inline set()
      :  Base()
   {}

   set(const set&) = delete;
   set& operator=(const set&) = delete;

   inline explicit set( const key_compare &cmp, const value_traits &v_traits)
      :  Base(cmp, v_traits)
   {}

   template<class Iterator>
   inline set( Iterator b, Iterator e
      , const key_compare &cmp = key_compare()
      , const value_traits &v_traits = value_traits())
      :  Base(b, e, cmp, v_traits)
   {}

   inline set(set&& x) : Base(std::move(x))
   {}

   inline set& operator=(set&& x)
   {  return static_cast<set &>(this->Base::operator=(std::move(x)));  }

   template <class Cloner, class Disposer>
   inline void clone_from(const set &src, Cloner cloner, Disposer disposer)
   {  Base::clone_from(src, cloner, disposer);  }

   template <class Cloner, class Disposer>
   inline void clone_from(set&& src, Cloner cloner, Disposer disposer)
   {  Base::clone_from(std::forward<set>(src), cloner, disposer);  }

   inline static set &container_from_end_iterator(iterator end_iterator)
   {  return static_cast<set &>(Base::container_from_end_iterator(end_iterator));   }

   inline static const set &container_from_end_iterator(const_iterator end_iterator)
   {  return static_cast<const set &>(Base::container_from_end_iterator(end_iterator));   }

   inline static set &container_from_iterator(iterator it)
   {  return static_cast<set &>(Base::container_from_iterator(it));   }

   inline static const set &container_from_iterator(const_iterator it)
   {  return static_cast<const set &>(Base::container_from_iterator(it));   }
};

#endif

//! The class template multiset is an intrusive container, that mimics most of
//! the interface of std::multiset as described in the C++ standard.
//!
//! The template parameter \c T is the type to be managed by the container.
//! The user can specify additional options and if no options are provided
//! default options are used.
//!
//! The container supports the following options:
//! \c base_hook<>/member_hook<>/value_traits<>,
//! \c constant_time_size<>, and
//! \c compare<>.
#if defined(BOOST_INTRUSIVE_DOXYGEN_INVOKED)
template<class T, class ...Options>
#else
template<class ValueTraits, class VoidOrKeyOfValue, class Compare, bool ConstantTimeSize, typename HeaderHolder>
#endif
class multiset_impl
#ifndef BOOST_INTRUSIVE_DOXYGEN_INVOKED
   : public bstree_impl<ValueTraits, VoidOrKeyOfValue, Compare, ConstantTimeSize, RbTreeAlgorithms, HeaderHolder>
#endif
{
   /// @cond
   typedef bstree_impl<ValueTraits, VoidOrKeyOfValue, Compare, ConstantTimeSize, RbTreeAlgorithms, HeaderHolder> tree_type;

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
   //! @copydoc ::boost::intrusive::rbtree::rbtree()
   multiset_impl()
      :  tree_type()
   {}

   //! @copydoc ::boost::intrusive::rbtree::rbtree(const key_compare &,const value_traits &)
   explicit multiset_impl( const key_compare &cmp, const value_traits &v_traits = value_traits())
      :  tree_type(cmp, v_traits)
   {}

   //! @copydoc ::boost::intrusive::rbtree::rbtree(bool,Iterator,Iterator,const key_compare &,const value_traits &)
   template<class Iterator>
   multiset_impl( Iterator b, Iterator e
                , const key_compare &cmp = key_compare()
                , const value_traits &v_traits = value_traits())
      : tree_type(false, b, e, cmp, v_traits)
   {}

   multiset_impl(const multiset_impl&) = delete;
   multiset_impl& operator=(const multiset_impl&) = delete;
   //! @copydoc ::boost::intrusive::rbtree::rbtree(rbtree &&)
   multiset_impl(multiset_impl&& x) : tree_type(std::move(x)) {}

   //! @copydoc ::boost::intrusive::rbtree::operator=(rbtree &&)
   multiset_impl& operator=(multiset_impl&& x)
   {  return static_cast<multiset_impl&>(tree_type::operator=(std::move(x))); }

   #ifdef BOOST_INTRUSIVE_DOXYGEN_INVOKED
   //! @copydoc ::boost::intrusive::rbtree::~rbtree()
   ~multiset_impl();

   //! @copydoc ::boost::intrusive::rbtree::begin()
   iterator begin();

   //! @copydoc ::boost::intrusive::rbtree::begin()const
   const_iterator begin() const;

   //! @copydoc ::boost::intrusive::rbtree::cbegin()const
   const_iterator cbegin() const;

   //! @copydoc ::boost::intrusive::rbtree::end()
   iterator end();

   //! @copydoc ::boost::intrusive::rbtree::end()const
   const_iterator end() const;

   //! @copydoc ::boost::intrusive::rbtree::cend()const
   const_iterator cend() const;

   //! @copydoc ::boost::intrusive::rbtree::rbegin()
   reverse_iterator rbegin();

   //! @copydoc ::boost::intrusive::rbtree::rbegin()const
   const_reverse_iterator rbegin() const;

   //! @copydoc ::boost::intrusive::rbtree::crbegin()const
   const_reverse_iterator crbegin() const;

   //! @copydoc ::boost::intrusive::rbtree::rend()
   reverse_iterator rend();

   //! @copydoc ::boost::intrusive::rbtree::rend()const
   const_reverse_iterator rend() const;

   //! @copydoc ::boost::intrusive::rbtree::crend()const
   const_reverse_iterator crend() const;

   //! @copydoc ::boost::intrusive::rbtree::root()
   iterator root();

   //! @copydoc ::boost::intrusive::rbtree::root()const
   const_iterator root() const;

   //! @copydoc ::boost::intrusive::rbtree::croot()const
   const_iterator croot() const;

   //! @copydoc ::boost::intrusive::rbtree::container_from_end_iterator(iterator)
   static multiset_impl &container_from_end_iterator(iterator end_iterator);

   //! @copydoc ::boost::intrusive::rbtree::container_from_end_iterator(const_iterator)
   static const multiset_impl &container_from_end_iterator(const_iterator end_iterator);

   //! @copydoc ::boost::intrusive::rbtree::container_from_iterator(iterator)
   static multiset_impl &container_from_iterator(iterator it);

   //! @copydoc ::boost::intrusive::rbtree::container_from_iterator(const_iterator)
   static const multiset_impl &container_from_iterator(const_iterator it);

   //! @copydoc ::boost::intrusive::rbtree::key_comp()const
   key_compare key_comp() const;

   //! @copydoc ::boost::intrusive::rbtree::value_comp()const
   value_compare value_comp() const;

   //! @copydoc ::boost::intrusive::rbtree::empty()const
   bool empty() const;

   //! @copydoc ::boost::intrusive::rbtree::size()const
   std::size_t size() const;

   //! @copydoc ::boost::intrusive::rbtree::swap
   void swap(multiset_impl& other);

   //! @copydoc ::boost::intrusive::rbtree::clone_from(const rbtree&,Cloner,Disposer)
   template <class Cloner, class Disposer>
   void clone_from(const multiset_impl &src, Cloner cloner, Disposer disposer);

   #else

   using tree_type::clone_from;

   #endif   //#ifdef BOOST_INTRUSIVE_DOXYGEN_INVOKED

   //! @copydoc ::boost::intrusive::rbtree::clone_from(rbtree&&,Cloner,Disposer)
   template <class Cloner, class Disposer>
   void clone_from(multiset_impl&& src, Cloner cloner, Disposer disposer)
   {  tree_type::clone_from(std::forward<multiset_impl>(src), cloner, disposer);  }

   //! @copydoc ::boost::intrusive::rbtree::insert_equal(reference)
   iterator insert(reference value)
   {  return tree_type::insert_equal(value);  }

   //! @copydoc ::boost::intrusive::rbtree::insert_equal(const_iterator,reference)
   iterator insert(const_iterator hint, reference value)
   {  return tree_type::insert_equal(hint, value);  }

   //! @copydoc ::boost::intrusive::rbtree::insert_equal(Iterator,Iterator)
   template<class Iterator>
   void insert(Iterator b, Iterator e)
   {  tree_type::insert_equal(b, e);  }

   #ifdef BOOST_INTRUSIVE_DOXYGEN_INVOKED
   //! @copydoc ::boost::intrusive::rbtree::insert_before
   iterator insert_before(const_iterator pos, reference value);

   //! @copydoc ::boost::intrusive::rbtree::push_back
   void push_back(reference value);

   //! @copydoc ::boost::intrusive::rbtree::push_front
   void push_front(reference value);

   //! @copydoc ::boost::intrusive::rbtree::erase(const_iterator)
   iterator erase(const_iterator i);

   //! @copydoc ::boost::intrusive::rbtree::erase(const_iterator,const_iterator)
   iterator erase(const_iterator b, const_iterator e);

   //! @copydoc ::boost::intrusive::rbtree::erase(const key_type &)
   std::size_t erase(const key_type &key);

   //! @copydoc ::boost::intrusive::rbtree::erase(const KeyType&,KeyTypeKeyCompare)
   template<class KeyType, class KeyTypeKeyCompare>
   std::size_t erase(const KeyType& key, KeyTypeKeyCompare comp);

   //! @copydoc ::boost::intrusive::rbtree::erase_and_dispose(const_iterator,Disposer)
   template<class Disposer>
   iterator erase_and_dispose(const_iterator i, Disposer disposer);

   //! @copydoc ::boost::intrusive::rbtree::erase_and_dispose(const_iterator,const_iterator,Disposer)
   template<class Disposer>
   iterator erase_and_dispose(const_iterator b, const_iterator e, Disposer disposer);

   //! @copydoc ::boost::intrusive::rbtree::erase_and_dispose(const key_type &, Disposer)
   template<class Disposer>
   std::size_t erase_and_dispose(const key_type &key, Disposer disposer);

   //! @copydoc ::boost::intrusive::rbtree::erase_and_dispose(const KeyType&,KeyTypeKeyCompare,Disposer)
   template<class KeyType, class KeyTypeKeyCompare, class Disposer>
   std::size_t erase_and_dispose(const KeyType& key, KeyTypeKeyCompare comp, Disposer disposer);

   //! @copydoc ::boost::intrusive::rbtree::clear
   void clear();

   //! @copydoc ::boost::intrusive::rbtree::clear_and_dispose
   template<class Disposer>
   void clear_and_dispose(Disposer disposer);

   //! @copydoc ::boost::intrusive::rbtree::count(const key_type &)const
   std::size_t count(const key_type &key) const;

   //! @copydoc ::boost::intrusive::rbtree::count(const KeyType&,KeyTypeKeyCompare)const
   template<class KeyType, class KeyTypeKeyCompare>
   std::size_t count(const KeyType& key, KeyTypeKeyCompare comp) const;

   //! @copydoc ::boost::intrusive::rbtree::lower_bound(const key_type &)
   iterator lower_bound(const key_type &key);

   //! @copydoc ::boost::intrusive::rbtree::lower_bound(const KeyType&,KeyTypeKeyCompare)
   template<class KeyType, class KeyTypeKeyCompare>
   iterator lower_bound(const KeyType& key, KeyTypeKeyCompare comp);

   //! @copydoc ::boost::intrusive::rbtree::lower_bound(const key_type &)const
   const_iterator lower_bound(const key_type &key) const;

   //! @copydoc ::boost::intrusive::rbtree::lower_bound(const KeyType&,KeyTypeKeyCompare)const
   template<class KeyType, class KeyTypeKeyCompare>
   const_iterator lower_bound(const KeyType& key, KeyTypeKeyCompare comp) const;

   //! @copydoc ::boost::intrusive::rbtree::upper_bound(const key_type &)
   iterator upper_bound(const key_type &key);

   //! @copydoc ::boost::intrusive::rbtree::upper_bound(const KeyType&,KeyTypeKeyCompare)
   template<class KeyType, class KeyTypeKeyCompare>
   iterator upper_bound(const KeyType& key, KeyTypeKeyCompare comp);

   //! @copydoc ::boost::intrusive::rbtree::upper_bound(const key_type &)const
   const_iterator upper_bound(const key_type &key) const;

   //! @copydoc ::boost::intrusive::rbtree::upper_bound(const KeyType&,KeyTypeKeyCompare)const
   template<class KeyType, class KeyTypeKeyCompare>
   const_iterator upper_bound(const KeyType& key, KeyTypeKeyCompare comp) const;

   //! @copydoc ::boost::intrusive::rbtree::find(const key_type &)
   iterator find(const key_type &key);

   //! @copydoc ::boost::intrusive::rbtree::find(const KeyType&,KeyTypeKeyCompare)
   template<class KeyType, class KeyTypeKeyCompare>
   iterator find(const KeyType& key, KeyTypeKeyCompare comp);

   //! @copydoc ::boost::intrusive::rbtree::find(const key_type &)const
   const_iterator find(const key_type &key) const;

   //! @copydoc ::boost::intrusive::rbtree::find(const KeyType&,KeyTypeKeyCompare)const
   template<class KeyType, class KeyTypeKeyCompare>
   const_iterator find(const KeyType& key, KeyTypeKeyCompare comp) const;

   //! @copydoc ::boost::intrusive::rbtree::equal_range(const key_type &)
   std::pair<iterator,iterator> equal_range(const key_type &key);

   //! @copydoc ::boost::intrusive::rbtree::equal_range(const KeyType&,KeyTypeKeyCompare)
   template<class KeyType, class KeyTypeKeyCompare>
   std::pair<iterator,iterator> equal_range(const KeyType& key, KeyTypeKeyCompare comp);

   //! @copydoc ::boost::intrusive::rbtree::equal_range(const key_type &)const
   std::pair<const_iterator, const_iterator>
      equal_range(const key_type &key) const;

   //! @copydoc ::boost::intrusive::rbtree::equal_range(const KeyType&,KeyTypeKeyCompare)const
   template<class KeyType, class KeyTypeKeyCompare>
   std::pair<const_iterator, const_iterator>
      equal_range(const KeyType& key, KeyTypeKeyCompare comp) const;

   //! @copydoc ::boost::intrusive::rbtree::bounded_range(const key_type &,const key_type &,bool,bool)
   std::pair<iterator,iterator> bounded_range
      (const key_type &lower_key, const key_type &upper_key, bool left_closed, bool right_closed);

   //! @copydoc ::boost::intrusive::rbtree::bounded_range(const KeyType&,const KeyType&,KeyTypeKeyCompare,bool,bool)
   template<class KeyType, class KeyTypeKeyCompare>
   std::pair<iterator,iterator> bounded_range
      (const KeyType& lower_key, const KeyType& upper_key, KeyTypeKeyCompare comp, bool left_closed, bool right_closed);

   //! @copydoc ::boost::intrusive::rbtree::bounded_range(const key_type &,const key_type &,bool,bool)const
   std::pair<const_iterator, const_iterator>
      bounded_range(const key_type &lower_key, const key_type &upper_key, bool left_closed, bool right_closed) const;

   //! @copydoc ::boost::intrusive::rbtree::bounded_range(const KeyType&,const KeyType&,KeyTypeKeyCompare,bool,bool)const
   template<class KeyType, class KeyTypeKeyCompare>
   std::pair<const_iterator, const_iterator> bounded_range
         (const KeyType& lower_key, const KeyType& upper_key, KeyTypeKeyCompare comp, bool left_closed, bool right_closed) const;

   //! @copydoc ::boost::intrusive::rbtree::s_iterator_to(reference)
   static iterator s_iterator_to(reference value);

   //! @copydoc ::boost::intrusive::rbtree::s_iterator_to(const_reference)
   static const_iterator s_iterator_to(const_reference value);

   //! @copydoc ::boost::intrusive::rbtree::iterator_to(reference)
   iterator iterator_to(reference value);

   //! @copydoc ::boost::intrusive::rbtree::iterator_to(const_reference)const
   const_iterator iterator_to(const_reference value) const;

   //! @copydoc ::boost::intrusive::rbtree::init_node(reference)
   static void init_node(reference value);

   //! @copydoc ::boost::intrusive::rbtree::unlink_leftmost_without_rebalance
   pointer unlink_leftmost_without_rebalance();

   //! @copydoc ::boost::intrusive::rbtree::replace_node
   void replace_node(iterator replace_this, reference with_this);

   //! @copydoc ::boost::intrusive::rbtree::remove_node
   void remove_node(reference value);

   //! @copydoc ::boost::intrusive::rbtree::merge_equal
   template<class ...Options2>
   void merge(multiset<T, Options2...> &source);

   //! @copydoc ::boost::intrusive::rbtree::merge_equal
   template<class ...Options2>
   void merge(set<T, Options2...> &source);

   #else

   template<class Compare2>
   void merge(multiset_impl<ValueTraits, VoidOrKeyOfValue, Compare2, ConstantTimeSize, HeaderHolder> &source)
   {  return tree_type::merge_equal(source);  }

   template<class Compare2>
   void merge(set_impl<ValueTraits, VoidOrKeyOfValue, Compare2, ConstantTimeSize, HeaderHolder> &source)
   {  return tree_type::merge_equal(source);  }

   #endif   //#ifdef BOOST_INTRUSIVE_DOXYGEN_INVOKED
};

#if defined(BOOST_INTRUSIVE_DOXYGEN_INVOKED)

template<class T, class ...Options>
bool operator!= (const multiset_impl<T, Options...> &x, const multiset_impl<T, Options...> &y);

template<class T, class ...Options>
bool operator>(const multiset_impl<T, Options...> &x, const multiset_impl<T, Options...> &y);

template<class T, class ...Options>
bool operator<=(const multiset_impl<T, Options...> &x, const multiset_impl<T, Options...> &y);

template<class T, class ...Options>
bool operator>=(const multiset_impl<T, Options...> &x, const multiset_impl<T, Options...> &y);

template<class T, class ...Options>
void swap(multiset_impl<T, Options...> &x, multiset_impl<T, Options...> &y);

#endif   //#if defined(BOOST_INTRUSIVE_DOXYGEN_INVOKED)

//! Helper metafunction to define a \c multiset that yields to the same type when the
//! same options (either explicitly or implicitly) are used.
template<class T, class ...Options>
struct make_multiset
{
   /// @cond
   typedef typename pack_options
      < rbtree_defaults,
      Options...
      >::type packed_options;

   using value_traits = detail::get_value_traits_t<T, typename packed_options::proto_value_traits>;

   typedef multiset_impl
         < value_traits
         , typename packed_options::key_of_value
         , typename packed_options::compare
         , packed_options::constant_time_size
         , typename packed_options::header_holder_type
         > implementation_defined;
   /// @endcond
   typedef implementation_defined type;
};

#ifndef BOOST_INTRUSIVE_DOXYGEN_INVOKED

template<class T, class ...Options>
class multiset
   :  public make_multiset<T,
      Options...
      >::type
{
   typedef typename make_multiset<T,
      Options...
      >::type   Base;

   public:
   typedef typename Base::key_compare        key_compare;
   typedef typename Base::value_traits       value_traits;
   typedef typename Base::iterator           iterator;
   typedef typename Base::const_iterator     const_iterator;

   //Assert if passed value traits are compatible with the type
   static_assert((std::is_same<typename value_traits::value_type, T>::value));

   inline multiset()
      :  Base()
   {}

   inline explicit multiset( const key_compare &cmp, const value_traits &v_traits = value_traits())
      :  Base(cmp, v_traits)
   {}

   template<class Iterator>
   inline multiset( Iterator b, Iterator e
           , const key_compare &cmp = key_compare()
           , const value_traits &v_traits = value_traits())
      :  Base(b, e, cmp, v_traits)
   {}

   multiset(const multiset&) = delete;
   multiset& operator=(const multiset&) = delete;
   inline multiset(multiset&& x) : Base(std::move(x)) {}

   inline multiset& operator=(multiset&& x)
   {  return static_cast<multiset &>(this->Base::operator=(std::move(x)));  }

   template <class Cloner, class Disposer>
   inline void clone_from(const multiset &src, Cloner cloner, Disposer disposer)
   {  Base::clone_from(src, cloner, disposer);  }

   template <class Cloner, class Disposer>
   inline void clone_from(multiset&& src, Cloner cloner, Disposer disposer)
   {  Base::clone_from(std::forward<multiset>(src), cloner, disposer);  }

   inline static multiset &container_from_end_iterator(iterator end_iterator)
   {  return static_cast<multiset &>(Base::container_from_end_iterator(end_iterator));   }

   inline static const multiset &container_from_end_iterator(const_iterator end_iterator)
   {  return static_cast<const multiset &>(Base::container_from_end_iterator(end_iterator));   }

   inline static multiset &container_from_iterator(iterator it)
   {  return static_cast<multiset &>(Base::container_from_iterator(it));   }

   inline static const multiset &container_from_iterator(const_iterator it)
   {  return static_cast<const multiset &>(Base::container_from_iterator(it));   }
};

#endif

} //namespace intrusive
} //namespace boost

#endif //BOOST_INTRUSIVE_SET_HPP
