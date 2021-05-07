#include "catch.hpp"
#include <cstddef>

#include <boost/intrusive/list.hpp>
#include <boost/intrusive/slist.hpp>
#include <boost/intrusive/set.hpp>
#include <boost/intrusive/avl_set.hpp>
#include <boost/intrusive/bs_set.hpp>
#include <boost/intrusive/sg_set.hpp>
#include <boost/intrusive/splay_set.hpp>
#include <boost/intrusive/treap_set.hpp>
#include <boost/intrusive/unordered_set.hpp>
#include <boost/static_assert.hpp>
#include "itestvalue.hpp"

using namespace boost::intrusive;

template<class A, class B>
struct pow2_and_equal_sizes
{
    static const std::size_t a_size = sizeof(A);
    static const std::size_t b_size = sizeof(B);
    static const bool a_b_sizes_equal = a_size == b_size;
    static const bool value = !(a_size & (a_size - 1u));
};

template<class Hook>
struct node : Hook
{};

//Avoid testing for uncommon architectures

template <typename T>
using type_reverse_iterator_t = typename T::reverse_iterator;

template <typename T>
using type_const_reverse_iterator_t = typename T::const_reverse_iterator;

template<class C>
void test_iterator_sizes(std::size_t size)
{
    typedef typename C::iterator                       iterator;
    typedef typename C::const_iterator                 const_iterator;
    using reverse_iterator = detail::default_action_t<C, ::type_reverse_iterator_t, iterator>;
    using const_reverse_iterator = detail::default_action_t<C, ::type_const_reverse_iterator_t, iterator>;
    REQUIRE(sizeof(iterator) == size);
    REQUIRE(sizeof(const_iterator) == size);
    REQUIRE(sizeof(iterator) == sizeof(reverse_iterator));
    REQUIRE(sizeof(const_iterator) == size);
    REQUIRE(sizeof(const_iterator) == sizeof(const_reverse_iterator));
}

//Test sizes for common 32 and 64 bit architectures
void test_sizes(std::true_type, std::size_t wordsize)
{
    {  //list
        typedef list<node<list_base_hook<> > > c;
        REQUIRE(sizeof(c) == wordsize * 3);
        test_iterator_sizes<c>(wordsize);
    }
    {
        typedef list<node<list_base_hook<> >, constant_time_size<false> > c;
        REQUIRE(sizeof(c) == wordsize * 2);
        test_iterator_sizes<c>(wordsize);
    }
    {
        typedef list< node< list_base_hook<> >, header_holder_type< heap_node_holder< list_node<void*>* > > > c;
        REQUIRE(sizeof(c) == wordsize * 2);
        test_iterator_sizes<c>(wordsize);
    }
    {
        typedef list< node< list_base_hook<> >, constant_time_size<false>, header_holder_type< heap_node_holder< list_node<void*>* > > > c;
        REQUIRE(sizeof(c) == wordsize * 1);
        test_iterator_sizes<c>(wordsize);
    }
    {  //slist
        typedef slist<node< slist_base_hook<> > > c;
        REQUIRE(sizeof(c) == wordsize * 2);
        test_iterator_sizes<c>(wordsize);
    }
    {
        typedef slist<node< slist_base_hook<> >, constant_time_size<false> > c;
        REQUIRE(sizeof(c) == wordsize * 1);
        test_iterator_sizes<c>(wordsize);
    }
    {
        typedef slist<node< slist_base_hook<> >, cache_last<true> > c;
        REQUIRE(sizeof(c) == wordsize * 3);
        test_iterator_sizes<c>(wordsize);
    }
    {  //set
        typedef set<node< set_base_hook<> > > c;
#if defined (_WIN32) // lack [[no_unique_address]]
        REQUIRE(sizeof(c) == wordsize * 6);
#else
        REQUIRE(sizeof(c) == wordsize * 5);
#endif
        test_iterator_sizes<c>(wordsize);
    }
    {
        typedef set<node< set_base_hook<> >, constant_time_size<false> > c;
#if defined (_WIN32) // lack [[no_unique_address]]
        REQUIRE(sizeof(c) == wordsize * 6);
#else
        REQUIRE(sizeof(c) == wordsize * 4);
#endif
        test_iterator_sizes<c>(wordsize);
    }
    {
        typedef set<node< set_base_hook<optimize_size<true> > >, constant_time_size<false> > c;
#if defined (_WIN32) // lack [[no_unique_address]]
        REQUIRE(sizeof(c) == wordsize * 5);
#else
        REQUIRE(sizeof(c) == wordsize * 3);
#endif
        test_iterator_sizes<c>(wordsize);
    }
    {
        typedef set< node< set_base_hook<> >, header_holder_type< heap_node_holder< rbtree_node<void*>* > > > c;
#if defined (_WIN32) // lack [[no_unique_address]]
        REQUIRE(sizeof(c) == wordsize * 3);
#else
        REQUIRE(sizeof(c) == wordsize * 2);
#endif
        test_iterator_sizes<c>(wordsize);
    }
    {
        typedef set< node< set_base_hook<> >, constant_time_size<false>, header_holder_type< heap_node_holder< rbtree_node<void*>* > > > c;
#if defined (_WIN32) // lack [[no_unique_address]]
        REQUIRE(sizeof(c) == wordsize * 3);
#else
        REQUIRE(sizeof(c) == wordsize * 1);
#endif
        test_iterator_sizes<c>(wordsize);
    }
    {  //avl
        typedef avl_set<node< avl_set_base_hook<> > > c;
#if defined (_WIN32) // lack [[no_unique_address]]
        REQUIRE(sizeof(c) == wordsize * 6);
#else
        REQUIRE(sizeof(c) == wordsize * 5);
#endif
        test_iterator_sizes<c>(wordsize);
    }
    {
        typedef avl_set<node< avl_set_base_hook<> >, constant_time_size<false> > c;
#if defined (_WIN32) // lack [[no_unique_address]]
        REQUIRE(sizeof(c) == wordsize * 6);
#else
        REQUIRE(sizeof(c) == wordsize * 4);
#endif
        test_iterator_sizes<c>(wordsize);
    }
    {
        typedef avl_set<node< avl_set_base_hook<optimize_size<true> > >, constant_time_size<false> > c;
#if defined (_WIN32) // lack [[no_unique_address]]
        REQUIRE(sizeof(c) == wordsize * 5);
#else
        REQUIRE(sizeof(c) == wordsize * 3);
#endif
        test_iterator_sizes<c>(wordsize);
    }
    {
        typedef avl_set< node< avl_set_base_hook<> >, header_holder_type< heap_node_holder< avltree_node<void*>* > > > c;
#if defined (_WIN32) // lack [[no_unique_address]]
        REQUIRE(sizeof(c) == wordsize * 3);
#else
        REQUIRE(sizeof(c) == wordsize * 2);
#endif
        test_iterator_sizes<c>(wordsize);
    }
    {
        typedef avl_set< node< avl_set_base_hook<> >, constant_time_size<false>, header_holder_type< heap_node_holder< avltree_node<void*>* > > > c;
#if defined (_WIN32) // lack [[no_unique_address]]
        REQUIRE(sizeof(c) == wordsize * 3);
#else
        REQUIRE(sizeof(c) == wordsize * 1);
#endif
        test_iterator_sizes<c>(wordsize);
    }
    {  //bs
        typedef bs_set<node< bs_set_base_hook<> > > c;
#if defined (_WIN32) // lack [[no_unique_address]]
        REQUIRE(sizeof(c) == wordsize * 5);
#else
        REQUIRE(sizeof(c) == wordsize * 4);
#endif
        test_iterator_sizes<c>(wordsize);
    }
    {
        typedef bs_set<node< bs_set_base_hook<> >, constant_time_size<false> > c;
#if defined (_WIN32) // lack [[no_unique_address]]
        REQUIRE(sizeof(c) == wordsize * 5);
#else
        REQUIRE(sizeof(c) == wordsize * 3);
#endif
        test_iterator_sizes<c>(wordsize);
    }
    {  //splay
        typedef splay_set<node< bs_set_base_hook<> > > c;
#if defined (_WIN32) // lack [[no_unique_address]]
        REQUIRE(sizeof(c) == wordsize * 5);
#else
        REQUIRE(sizeof(c) == wordsize * 4);
#endif
        test_iterator_sizes<c>(wordsize);
    }
    {
        typedef splay_set<node< bs_set_base_hook<> >, constant_time_size<false> > c;
#if defined (_WIN32) // lack [[no_unique_address]]
        REQUIRE(sizeof(c) == wordsize * 5);
#else
        REQUIRE(sizeof(c) == wordsize * 3);
#endif
        test_iterator_sizes<c>(wordsize);
    }
    {  //scapegoat
        typedef sg_set<node< bs_set_base_hook<> > > c;
#if defined (_WIN32) // lack [[no_unique_address]]
        REQUIRE(sizeof(c) == (wordsize * 6 + sizeof(float) * 2));
#else
        REQUIRE(sizeof(c) == (wordsize * 5 + sizeof(float) * 2));
#endif
        test_iterator_sizes<c>(wordsize);
    }
    {  //treap
        typedef treap_set<node< bs_set_base_hook<> > > c;
#if defined (_WIN32) // lack [[no_unique_address]]
        REQUIRE(sizeof(c) == wordsize * 6);
#else
        REQUIRE(sizeof(c) == wordsize * 4);
#endif
        test_iterator_sizes<c>(wordsize);
    }
    {
        typedef treap_set<node< bs_set_base_hook<> >, constant_time_size<false> > c;
#if defined (_WIN32) // lack [[no_unique_address]]
        REQUIRE(sizeof(c) == wordsize * 6);
#else
        REQUIRE(sizeof(c) == wordsize * 3);
#endif
        test_iterator_sizes<c>(wordsize);
    }
    {  //unordered
        typedef unordered_set<node< unordered_set_base_hook<> > > c;
#if defined (_WIN32) // lack [[no_unique_address]]
        REQUIRE(sizeof(c) == wordsize * 6);
#else
        REQUIRE(sizeof(c) == wordsize * 3);
#endif
        test_iterator_sizes<c>(wordsize * 2);
    }
    {
        typedef unordered_set<node< unordered_set_base_hook<> >, power_2_buckets<true>  > c;
#if defined (_WIN32) // lack [[no_unique_address]]
        REQUIRE(sizeof(c) == wordsize * 6);
#else
        REQUIRE(sizeof(c) == wordsize * 3);
#endif
        test_iterator_sizes<c>(wordsize * 2);
    }
    {
        typedef unordered_set<node< unordered_set_base_hook<> >, constant_time_size<false> > c;
#if defined (_WIN32) // lack [[no_unique_address]]
        REQUIRE(sizeof(c) == wordsize * 6);
#else
        REQUIRE(sizeof(c) == wordsize * 2);
 #endif
        test_iterator_sizes<c>(wordsize * 2);
    }
    {
        typedef unordered_set<node< unordered_set_base_hook< optimize_multikey<true> > >, constant_time_size<false> > c;
#if defined (_WIN32) // lack [[no_unique_address]]
        REQUIRE(sizeof(c) == wordsize * 6);
#else
        REQUIRE(sizeof(c) == wordsize * 2);
#endif
        test_iterator_sizes<c>(wordsize * 2);
    }
    {
        typedef unordered_set<node< unordered_set_base_hook< optimize_multikey<true> > >, incremental<true> > c;
#if defined (_WIN32) // lack [[no_unique_address]]
        REQUIRE(sizeof(c) == wordsize * 6);
#else
        REQUIRE(sizeof(c) == wordsize * 4);
#endif
        test_iterator_sizes<c>(wordsize * 2);
    }
}

TEST_CASE("container_size_test test_size", "container_size_test") {
    test_sizes(std::bool_constant<pow2_and_equal_sizes<std::size_t, void*>::value>(), sizeof(std::size_t));
}