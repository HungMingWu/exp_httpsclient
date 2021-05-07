/////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga  2013-2013
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/intrusive for documentation.
//
/////////////////////////////////////////////////////////////////////////////
#include <boost/intrusive/list.hpp>
#include <boost/intrusive/slist.hpp>
#include <boost/intrusive/set.hpp>
#include <boost/intrusive/unordered_set.hpp>
#include <boost/intrusive/avl_set.hpp>
#include <boost/intrusive/sg_set.hpp>
#include <boost/intrusive/bs_set.hpp>
#include <boost/intrusive/splay_set.hpp>
#include <boost/intrusive/treap_set.hpp>
#include <boost/intrusive/detail/mpl.hpp>
#include <boost/static_assert.hpp>
#include "smart_ptr.hpp"
#include <functional>   //std::greater/std::less

using namespace boost::intrusive;
struct my_tag;

template<class VoidPtr = void*, link_mode_type mode = link_mode_type::normal_link>
struct MyClass5
    : public make_list_base_hook_t<void_pointer<VoidPtr>, link_mode<mode>>
    , public make_slist_base_hook_t<void_pointer<VoidPtr>, link_mode<mode>>
    , public make_set_base_hook_t<void_pointer<VoidPtr>, link_mode<mode>>
    , public make_unordered_set_base_hook_t<void_pointer<VoidPtr>, link_mode<mode>>
    , public make_avl_set_base_hook_t<void_pointer<VoidPtr>, link_mode<mode>>
    , public make_bs_set_base_hook_t<void_pointer<VoidPtr>, link_mode<mode>>
{
    int int_;

    MyClass5(int i)
        : int_(i)
    {}

    friend bool operator<(const MyClass5& l, const MyClass5& r)
    {
        return l.int_ < r.int_;
    }

    friend bool operator==(const MyClass5& l, const MyClass5& r)
    {
        return l.int_ == r.int_;
    }

    friend bool priority_order(const MyClass5& l, const MyClass5& r)
    {
        return l.int_ < r.int_;
    }
};

namespace std
{
    template <typename VoidPtr, link_mode_type mode>
    struct hash<MyClass5<VoidPtr, mode>>
    {
        size_t operator()(MyClass5<VoidPtr, mode> const& o) const noexcept
        {
            return static_cast<std::size_t>(o.int_);
        }
    };
}

template<class T>
struct inverse_priority
{
    bool operator()(const T& l, const T& r)
    {
        return l.int_ > r.int_;
    }
};

template<class T>
struct inverse_hash
{
    bool operator()(const T& l)
    {
        return hash_value(l);
    }
};

template<class T>
struct alternative_equal
{
    bool operator()(const T& l, const T& r)
    {
        return l.int_ == r.int_;
    }
};

template <typename T, typename ...Ts>
using list_t = list<T, Ts...>;

template <typename T = MyClass5<>, typename ...Ts>
using list_iterator_t = typename list<T, Ts...>::iterator;

template <typename T = MyClass5<>, typename ...Ts>
using list_const_iterator_t = typename list<T, Ts...>::const_iterator;

template <typename T, typename ...Ts>
using slist_t = slist<T, Ts...>;

template <typename T = MyClass5<>, typename ...Ts>
using slist_iterator_t = typename slist<T, Ts...>::iterator;

template <typename T = MyClass5<>, typename ...Ts>
using slist_const_iterator_t = typename slist<T, Ts...>::const_iterator;

template <typename T, typename ...Ts>
using set_t = set<T, Ts...>;

template <typename T = MyClass5<>, typename ...Ts>
using set_iterator_t = typename set<T, Ts...>::iterator;

template <typename T = MyClass5<>, typename ...Ts>
using set_const_iterator_t = typename set<T, Ts...>::const_iterator;

template <typename T, typename ...Ts>
using avlset_t = avl_set<T, Ts...>;

template <typename T = MyClass5<>, typename ...Ts>
using avlset_iterator_t = typename avlset_t<T, Ts...>::iterator;

template <typename T = MyClass5<>, typename ...Ts>
using avlset_const_iterator_t = typename avlset_t<T, Ts...>::const_iterator;

template <typename T, typename ...Ts>
using sgset_t = sg_set<T, Ts...>;

template <typename T = MyClass5<>, typename ...Ts>
using sgset_iterator_t = typename sgset_t<T, Ts...>::iterator;

template <typename T = MyClass5<>, typename ...Ts>
using sgset_const_iterator_t = typename sgset_t<T, Ts...>::const_iterator;

template <typename T, typename ...Ts>
using bsset_t = bs_set<T, Ts...>;

template <typename T = MyClass5<>, typename ...Ts>
using bsset_iterator_t = typename bsset_t<T, Ts...>::iterator;

template <typename T = MyClass5<>, typename ...Ts>
using bsset_const_iterator_t = typename bsset_t<T, Ts...>::const_iterator;

template <typename T, typename ...Ts>
using splayset_t = splay_set<T, Ts...>;

template <typename T = MyClass5<>, typename ...Ts>
using splayset_iterator_t = typename splayset_t<T, Ts...>::iterator;

template <typename T = MyClass5<>, typename ...Ts>
using splayset_const_iterator_t = typename splayset_t<T, Ts...>::const_iterator;

template <typename T, typename ...Ts>
using treapset_t = treap_set<T, Ts...>;

template <typename T = MyClass5<>, typename ...Ts>
using treapset_iterator_t = typename treapset_t<T, Ts...>::iterator;

template <typename T = MyClass5<>, typename ...Ts>
using treapset_const_iterator_t = typename treapset_t<T, Ts...>::const_iterator;

template <typename T, typename ...Ts>
using unorderedset_t = unordered_set<T, Ts...>;

template <typename T = MyClass5<>, typename ...Ts>
using unorderedset_iterator_t = typename unorderedset_t<T, Ts...>::iterator;

template <typename T = MyClass5<>, typename ...Ts>
using unorderedset_const_iterator_t = typename unorderedset_t<T, Ts...>::const_iterator;

static_assert(!std::is_same_v<list_iterator_t<>, list_const_iterator_t<>>);
static_assert(std::is_same_v<list_iterator_t<MyClass5<>, constant_time_size<true>>,
                             list_iterator_t<MyClass5<>, constant_time_size<false>>>);
static_assert(!std::is_same_v<list_iterator_t<>, list_iterator_t<MyClass5<smart_ptr<void>>>>);
static_assert(!std::is_same_v<slist_iterator_t<>, slist_const_iterator_t<>>);
static_assert(std::is_same_v<slist_iterator_t<MyClass5<>, constant_time_size<true>>,
                             slist_iterator_t<MyClass5<>, constant_time_size<false>>>);
static_assert(!std::is_same_v<slist_iterator_t<>, slist_iterator_t<MyClass5<smart_ptr<void>>>>);
static_assert(std::is_same_v<slist_iterator_t<MyClass5<>, cache_last<false>>, 
                             slist_iterator_t<MyClass5<>, cache_last<true>>>);
static_assert(std::is_same_v<slist_iterator_t<MyClass5<>, linear<false>>, slist_iterator_t<MyClass5<>, linear<true>>>);
static_assert(!std::is_same_v<set_iterator_t<>, set_const_iterator_t<>>);
static_assert(std::is_same_v<set_iterator_t<MyClass5<>, constant_time_size<true>>,
                             set_iterator_t<MyClass5<>, constant_time_size<false>>>);
static_assert(!std::is_same_v<set_iterator_t<>, set_iterator_t<MyClass5<smart_ptr<void>>>>);
static_assert(std::is_same_v<set_iterator_t<MyClass5<>, compare<std::greater<MyClass5<>>>>,
                             set_iterator_t<MyClass5<>, compare<std::less<MyClass5<>>>>>);
static_assert(!std::is_same_v<avlset_iterator_t<>, avlset_const_iterator_t<>>);
static_assert(std::is_same_v<avlset_iterator_t<MyClass5<>, constant_time_size<true>>,
                             avlset_iterator_t<MyClass5<>, constant_time_size<false>>>);
static_assert(!std::is_same_v<avlset_iterator_t<>, avlset_iterator_t<MyClass5<smart_ptr<void>>>>);
static_assert(std::is_same_v<avlset_iterator_t<MyClass5<>, compare<std::greater<MyClass5<>>>>,
                             avlset_iterator_t<MyClass5<>, compare<std::less<MyClass5<>>>>>);
static_assert(!std::is_same_v<sgset_iterator_t<MyClass5<>>, sgset_const_iterator_t<>>);
static_assert(!std::is_same_v<sgset_iterator_t<>, sgset_iterator_t<MyClass5<smart_ptr<void>>>>);
static_assert(std::is_same_v<sgset_iterator_t<MyClass5<>, compare<std::greater<MyClass5<>>>>,
                             sgset_iterator_t<MyClass5<>, compare<std::less<MyClass5<>>>>>);
static_assert(!std::is_same_v<bsset_iterator_t<>, bsset_const_iterator_t<>>);
static_assert(std::is_same_v<bsset_iterator_t<MyClass5<>, constant_time_size<true>>,
                             bsset_iterator_t<MyClass5<>, constant_time_size<false>>>);
static_assert(!std::is_same_v<bsset_iterator_t<>, bsset_iterator_t<MyClass5<smart_ptr<void>>>>);
static_assert(std::is_same_v<bsset_iterator_t<MyClass5<>, compare<std::greater<MyClass5<>>>>,
                             bsset_iterator_t<MyClass5<>, compare<std::less<MyClass5<>>>>>);
static_assert(!std::is_same_v<splayset_iterator_t<>, splayset_const_iterator_t<>>);
static_assert(std::is_same_v<splayset_iterator_t<MyClass5<>, constant_time_size<true>>,
                             splayset_iterator_t<MyClass5<>, constant_time_size<false>>>);
static_assert(!std::is_same_v<splayset_iterator_t<>, splayset_iterator_t<MyClass5<smart_ptr<void>>>>);
static_assert(std::is_same_v<splayset_iterator_t<MyClass5<>, compare<std::greater<MyClass5<>>>>,
                             splayset_iterator_t<MyClass5<>, compare<std::less<MyClass5<>>>>>);
static_assert(!std::is_same_v<treapset_iterator_t<>, treapset_const_iterator_t<>>);
static_assert(std::is_same_v<treapset_iterator_t<MyClass5<>, constant_time_size<true>>,
                             treapset_iterator_t<MyClass5<>, constant_time_size<false>>>);
static_assert(!std::is_same_v<treapset_iterator_t<>, treapset_iterator_t<MyClass5<smart_ptr<void>>>>);
static_assert(std::is_same_v<treapset_iterator_t<MyClass5<>, compare<std::greater<MyClass5<>>>>,
                             treapset_iterator_t<MyClass5<>, compare<std::less<MyClass5<>>>>>);
static_assert(std::is_same_v<treapset_iterator_t<>,
                             treapset_iterator_t<MyClass5<>, priority<inverse_priority<MyClass5<>>>>>);
static_assert(std::is_same_v<bsset_iterator_t<>, sgset_iterator_t<>>);
static_assert(std::is_same_v<bsset_iterator_t<>, treapset_iterator_t<>>);
static_assert(std::is_same_v<bsset_iterator_t<>, splayset_iterator_t<>>);
static_assert(!std::is_same_v<unorderedset_iterator_t<>, unorderedset_const_iterator_t<>>);
static_assert(std::is_same_v<unorderedset_iterator_t<MyClass5<>, constant_time_size<true>>,
                             unorderedset_iterator_t<MyClass5<>, constant_time_size<false>>>);
static_assert(!std::is_same_v<unorderedset_iterator_t<>, unorderedset_iterator_t<MyClass5<smart_ptr<void>>>>);
static_assert(std::is_same_v<unorderedset_iterator_t<>, 
                             unorderedset_iterator_t<MyClass5<>, hash<inverse_hash<MyClass5<>>>>>);
static_assert(std::is_same_v<unorderedset_iterator_t<>,
                             unorderedset_iterator_t<MyClass5<>, equal<alternative_equal<MyClass5<>>>>>);
static_assert(std::is_same_v<unorderedset_iterator_t<>,
                             unorderedset_iterator_t<MyClass5<>, power_2_buckets<true>>>);
static_assert(std::is_same_v<unorderedset_iterator_t<>,
                             unorderedset_iterator_t<MyClass5<>, cache_begin<true>>>);