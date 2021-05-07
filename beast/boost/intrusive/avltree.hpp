/////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2007-2013
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/intrusive for documentation.
//
/////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_INTRUSIVE_AVLTREE_HPP
#define BOOST_INTRUSIVE_AVLTREE_HPP

#include <boost/intrusive/intrusive_fwd.hpp>
#include <cstddef>

#include <boost/intrusive/avl_set_hook.hpp>
#include <boost/intrusive/detail/avltree_node.hpp>
#include <boost/intrusive/bstree.hpp>
#include <boost/intrusive/detail/tree_node.hpp>
#include <boost/intrusive/detail/ebo_functor_holder.hpp>
#include <boost/intrusive/detail/mpl.hpp>
#include <boost/intrusive/pointer_traits.hpp>
#include <boost/intrusive/detail/get_value_traits.hpp>
#include <boost/intrusive/avltree_algorithms.hpp>
#include <boost/intrusive/link_mode.hpp>

#if defined(BOOST_HAS_PRAGMA_ONCE)
#  pragma once
#endif

namespace boost {
namespace intrusive {

/// @cond

struct default_avltree_hook_applier
{  template <class T> struct apply{ typedef typename T::default_avltree_hook type;  };  };

template<>
struct is_default_hook_tag<default_avltree_hook_applier>
{  static const bool value = true;  };

struct avltree_defaults
   : bstree_defaults
{
   typedef default_avltree_hook_applier proto_value_traits;
};

/// @endcond

//! The class template avltree is an intrusive AVL tree container, that
//! is used to construct intrusive avl_set and avl_multiset containers.
//! The no-throw guarantee holds only, if the key_compare object
//! doesn't throw.
//!
//! The template parameter \c T is the type to be managed by the container.
//! The user can specify additional options and if no options are provided
//! default options are used.
//!
//! The container supports the following options:
//! \c base_hook<>/member_hook<>/value_traits<>,
//! \c constant_time_size<> and
//! \c compare<>.
#if defined(BOOST_INTRUSIVE_DOXYGEN_INVOKED)
template<class T, class ...Options>
#else
template<class ValueTraits, class VoidOrKeyOfValue, class VoidOrKeyComp, bool ConstantTimeSize, typename HeaderHolder>
#endif
class avltree_impl
   /// @cond
   :  public bstree_impl<ValueTraits, VoidOrKeyOfValue, VoidOrKeyComp, ConstantTimeSize, AvlTreeAlgorithms, HeaderHolder>
   /// @endcond
{
   public:
   typedef ValueTraits value_traits;
   /// @cond
   typedef bstree_impl< ValueTraits, VoidOrKeyOfValue, VoidOrKeyComp
                      , ConstantTimeSize, AvlTreeAlgorithms
                      , HeaderHolder>                                tree_type;
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
   typedef typename implementation_defined::node_algorithms          node_algorithms;

   static const bool constant_time_size = implementation_defined::constant_time_size;
   /// @cond
   private:

   avltree_impl(const avltree_impl&) = delete;
   avltree_impl& operator=(const avltree_impl&) = delete;

   /// @endcond

   public:

   typedef typename implementation_defined::insert_commit_data insert_commit_data;

   //! @copydoc ::boost::intrusive::bstree::bstree()
   avltree_impl()
      :  tree_type()
   {}

   //! @copydoc ::boost::intrusive::bstree::bstree(const key_compare &,const value_traits &)
   explicit avltree_impl( const key_compare &cmp, const value_traits &v_traits = value_traits())
      :  tree_type(cmp, v_traits)
   {}

   //! @copydoc ::boost::intrusive::bstree::bstree(bool,Iterator,Iterator,const key_compare &,const value_traits &)
   template<class Iterator>
   avltree_impl( bool unique, Iterator b, Iterator e
              , const key_compare &cmp     = key_compare()
              , const value_traits &v_traits = value_traits())
      : tree_type(unique, b, e, cmp, v_traits)
   {}

   //! @copydoc ::boost::intrusive::bstree::bstree(bstree &&)
   avltree_impl(avltree_impl&& x) : tree_type(std::move(x))
   {}

   //! @copydoc ::boost::intrusive::bstree::operator=(bstree &&)
   avltree_impl& operator=(avltree_impl&& x)
   {  return static_cast<avltree_impl&>(tree_type::operator=(std::move(x))); }

   #ifdef BOOST_INTRUSIVE_DOXYGEN_INVOKED

   //! @copydoc ::boost::intrusive::bstree::~bstree()
   ~avltree_impl();

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
   static avltree_impl &container_from_end_iterator(iterator end_iterator) noexcept;

   //! @copydoc ::boost::intrusive::bstree::container_from_end_iterator(const_iterator)
   static const avltree_impl &container_from_end_iterator(const_iterator end_iterator) noexcept;

   //! @copydoc ::boost::intrusive::bstree::container_from_iterator(iterator)
   static avltree_impl &container_from_iterator(iterator it) noexcept;

   //! @copydoc ::boost::intrusive::bstree::container_from_iterator(const_iterator)
   static const avltree_impl &container_from_iterator(const_iterator it) noexcept;

   //! @copydoc ::boost::intrusive::bstree::key_comp()const
   key_compare key_comp() const;

   //! @copydoc ::boost::intrusive::bstree::value_comp()const
   value_compare value_comp() const;

   //! @copydoc ::boost::intrusive::bstree::empty()const
   bool empty() const noexcept;

   //! @copydoc ::boost::intrusive::bstree::size()const
   std::size_t size() const noexcept;

   //! @copydoc ::boost::intrusive::bstree::swap
   void swap(avltree_impl& other);

   //! @copydoc ::boost::intrusive::bstree::clone_from(const bstree&,Cloner,Disposer)
   template <class Cloner, class Disposer>
   void clone_from(const avltree_impl &src, Cloner cloner, Disposer disposer);

   #else //BOOST_INTRUSIVE_DOXYGEN_INVOKED

   using tree_type::clone_from;

   #endif   //#ifdef BOOST_INTRUSIVE_DOXYGEN_INVOKED

   //! @copydoc ::boost::intrusive::bstree::clone_from(bstree&&,Cloner,Disposer)
   template <class Cloner, class Disposer>
   void clone_from(avltree_impl&& src, Cloner cloner, Disposer disposer)
   {  tree_type::clone_from(std::forward<avltree_impl>(src), cloner, disposer);  }

   #ifdef BOOST_INTRUSIVE_DOXYGEN_INVOKED

   //! @copydoc ::boost::intrusive::bstree::insert_equal(reference)
   iterator insert_equal(reference value);

   //! @copydoc ::boost::intrusive::bstree::insert_equal(const_iterator,reference)
   iterator insert_equal(const_iterator hint, reference value);

   //! @copydoc ::boost::intrusive::bstree::insert_equal(Iterator,Iterator)
   template<class Iterator>
   void insert_equal(Iterator b, Iterator e);

   //! @copydoc ::boost::intrusive::bstree::insert_unique(reference)
   std::pair<iterator, bool> insert_unique(reference value);

   //! @copydoc ::boost::intrusive::bstree::insert_unique(const_iterator,reference)
   iterator insert_unique(const_iterator hint, reference value);

   //! @copydoc ::boost::intrusive::bstree::insert_unique_check(const KeyType&,KeyTypeKeyCompare,insert_commit_data&)
   template<class KeyType, class KeyTypeKeyCompare>
   std::pair<iterator, bool> insert_unique_check
      (const KeyType &key, KeyTypeKeyCompare comp, insert_commit_data &commit_data);

   //! @copydoc ::boost::intrusive::bstree::insert_unique_check(const_iterator,const KeyType&,KeyTypeKeyCompare,insert_commit_data&)
   template<class KeyType, class KeyTypeKeyCompare>
   std::pair<iterator, bool> insert_unique_check
      (const_iterator hint, const KeyType &key
      ,KeyTypeKeyCompare comp, insert_commit_data &commit_data);

   //! @copydoc ::boost::intrusive::bstree::insert_unique_check(const key_type&,insert_commit_data&)
   std::pair<iterator, bool> insert_unique_check
      (const key_type &key, insert_commit_data &commit_data);

   //! @copydoc ::boost::intrusive::bstree::insert_unique_check(const_iterator,const key_type&,insert_commit_data&)
   std::pair<iterator, bool> insert_unique_check
      (const_iterator hint, const key_type &key, insert_commit_data &commit_data);

   //! @copydoc ::boost::intrusive::bstree::insert_unique_commit
   iterator insert_unique_commit(reference value, const insert_commit_data &commit_data) noexcept;

   //! @copydoc ::boost::intrusive::bstree::insert_unique(Iterator,Iterator)
   template<class Iterator>
   void insert_unique(Iterator b, Iterator e);

   //! @copydoc ::boost::intrusive::bstree::insert_before
   iterator insert_before(const_iterator pos, reference value) noexcept;

   //! @copydoc ::boost::intrusive::bstree::push_back
   void push_back(reference value) noexcept;

   //! @copydoc ::boost::intrusive::bstree::push_front
   void push_front(reference value) noexcept;

   //! @copydoc ::boost::intrusive::bstree::erase(const_iterator)
   iterator erase(const_iterator i) noexcept;

   //! @copydoc ::boost::intrusive::bstree::erase(const_iterator,const_iterator)
   iterator erase(const_iterator b, const_iterator e) noexcept;

   //! @copydoc ::boost::intrusive::bstree::erase(const key_type &)
   std::size_t erase(const key_type &key);

   //! @copydoc ::boost::intrusive::bstree::erase(const KeyType&,KeyTypeKeyCompare)
   template<class KeyType, class KeyTypeKeyCompare>
   std::size_t erase(const KeyType& key, KeyTypeKeyCompare comp);

   //! @copydoc ::boost::intrusive::bstree::erase_and_dispose(const_iterator,Disposer)
   template<class Disposer>
   iterator erase_and_dispose(const_iterator i, Disposer disposer) noexcept;

   //! @copydoc ::boost::intrusive::bstree::erase_and_dispose(const_iterator,const_iterator,Disposer)
   template<class Disposer>
   iterator erase_and_dispose(const_iterator b, const_iterator e, Disposer disposer) noexcept;

   //! @copydoc ::boost::intrusive::bstree::erase_and_dispose(const key_type &, Disposer)
   template<class Disposer>
   std::size_t erase_and_dispose(const key_type &key, Disposer disposer);

   //! @copydoc ::boost::intrusive::bstree::erase_and_dispose(const KeyType&,KeyTypeKeyCompare,Disposer)
   template<class KeyType, class KeyTypeKeyCompare, class Disposer>
   std::size_t erase_and_dispose(const KeyType& key, KeyTypeKeyCompare comp, Disposer disposer);

   //! @copydoc ::boost::intrusive::bstree::clear
   void clear() noexcept;

   //! @copydoc ::boost::intrusive::bstree::clear_and_dispose
   template<class Disposer>
   void clear_and_dispose(Disposer disposer) noexcept;

   //! @copydoc ::boost::intrusive::bstree::count(const key_type &ke)const
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

   //! @copydoc ::boost::intrusive::bstree::upper_bound(const key_type &key)
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
      (const key_type &lower, const key_type &upper_key, bool left_closed, bool right_closed);

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

   //! @copydoc ::boost::intrusive::bstree::merge_unique(bstree<T, Options2...>&)
   template<class T, class ...Options2>
   void merge_unique(avltree<T, Options2...> &);

   //! @copydoc ::boost::intrusive::bstree::merge_equal(bstree<T, Options2...>&)
   template<class T, class ...Options2>
   void merge_equal(avltree<T, Options2...> &);

   friend bool operator< (const avltree_impl &x, const avltree_impl &y);

   friend bool operator==(const avltree_impl &x, const avltree_impl &y);

   friend bool operator!= (const avltree_impl &x, const avltree_impl &y);

   friend bool operator>(const avltree_impl &x, const avltree_impl &y);

   friend bool operator<=(const avltree_impl &x, const avltree_impl &y);

   friend bool operator>=(const avltree_impl &x, const avltree_impl &y);

   friend void swap(avltree_impl &x, avltree_impl &y);
   #endif   //#ifdef BOOST_INTRUSIVE_DOXYGEN_INVOKED
};


//! Helper metafunction to define a \c avltree that yields to the same type when the
//! same options (either explicitly or implicitly) are used.
template<class T, class ...Options>
struct make_avltree
{
   /// @cond
   typedef typename pack_options
      < avltree_defaults,
      Options...
      >::type packed_options;

   using value_traits = detail::get_value_traits_t<T, typename packed_options::proto_value_traits>;

   typedef avltree_impl
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
class avltree
   :  public make_avltree<T,
      Options...
      >::type
{
   typedef typename make_avltree
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

   inline avltree()
      :  Base()
   {}

   inline explicit avltree( const key_compare &cmp, const value_traits &v_traits = value_traits())
      :  Base(cmp, v_traits)
   {}

   avltree(const avltree&) = delete;
   avltree& operator=(const avltree&) = delete;

   template<class Iterator>
   inline avltree( bool unique, Iterator b, Iterator e
         , const key_compare &cmp = key_compare()
         , const value_traits &v_traits = value_traits())
      :  Base(unique, b, e, cmp, v_traits)
   {}

   inline avltree(avltree&& x) : Base(std::move(x))
   {}

   inline avltree& operator=(avltree &&x)
   {  return static_cast<avltree &>(this->Base::operator=(std::move(x)));  }

   template <class Cloner, class Disposer>
   inline void clone_from(const avltree &src, Cloner cloner, Disposer disposer)
   {  Base::clone_from(src, cloner, disposer);  }

   template <class Cloner, class Disposer>
   inline void clone_from(avltree&& src, Cloner cloner, Disposer disposer)
   {  Base::clone_from(std::forward<avltree>(src), cloner, disposer);  }

   inline static avltree &container_from_end_iterator(iterator end_iterator) noexcept
   {  return static_cast<avltree &>(Base::container_from_end_iterator(end_iterator));   }

   inline static const avltree &container_from_end_iterator(const_iterator end_iterator) noexcept
   {  return static_cast<const avltree &>(Base::container_from_end_iterator(end_iterator));   }

   inline static avltree &container_from_iterator(iterator it) noexcept
   {  return static_cast<avltree &>(Base::container_from_iterator(it));   }

   inline static const avltree &container_from_iterator(const_iterator it) noexcept
   {  return static_cast<const avltree &>(Base::container_from_iterator(it));   }
};

#endif

} //namespace intrusive
} //namespace boost

#endif //BOOST_INTRUSIVE_AVLTREE_HPP
