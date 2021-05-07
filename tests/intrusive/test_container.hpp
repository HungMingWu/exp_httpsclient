/////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga  2007-2013
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/intrusive for documentation.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef BOOST_INTRUSIVE_TEST_CONTAINER_HPP
#define BOOST_INTRUSIVE_TEST_CONTAINER_HPP

#include <boost/intrusive/detail/mpl.hpp>
#include <boost/intrusive/detail/simple_disposers.hpp>
#include <boost/intrusive/detail/iterator.hpp>
#include <boost/intrusive/detail/mpl.hpp>
#include "iterator_test.hpp"
#include <cstdlib>

namespace boost {
namespace intrusive {
namespace test {

template <typename T, typename value_type>
concept is_unordered = requires {
    { typename T::hasher()(std::declval<value_type>()) } -> std::same_as<std::size_t>;
};

template<class Container>
struct test_container_typedefs
{
   typedef typename Container::value_type       value_type;
   typedef typename Container::iterator         iterator;
   typedef typename Container::const_iterator   const_iterator;
   typedef typename Container::reference        reference;
   typedef typename Container::const_reference  const_reference;
   typedef typename Container::pointer          pointer;
   typedef typename Container::const_pointer    const_pointer;
   typedef typename Container::difference_type  difference_type;
   typedef typename Container::value_traits     value_traits;
};

template< class Container >
void test_container( Container & c )
{
   typedef typename Container::const_iterator   const_iterator;
   typedef typename Container::iterator         iterator;

   {test_container_typedefs<Container> dummy;  (void)dummy;}
   const std::size_t num_elem = c.size();
   REQUIRE( c.empty() == (num_elem == 0) );
   {
      iterator it(c.begin()), itend(c.end());
      std::size_t i;
      for(i = 0; i < num_elem; ++i){
         ++it;
      }
      REQUIRE( it == itend );
      REQUIRE( c.size() == i );
   }

   //Check iterator conversion
   REQUIRE(const_iterator(c.begin()) == c.cbegin() );
   {
      const_iterator it(c.cbegin()), itend(c.cend());
      std::size_t i;
      for(i = 0; i < num_elem; ++i){
         ++it;
      }
      REQUIRE( it == itend );
      REQUIRE( c.size() == i );
   }
   static_cast<const Container&>(c).check();
   //Very basic test for comparisons
   {
      REQUIRE(c == c);
      REQUIRE(!(c != c));
      REQUIRE(!(c < c));
      REQUIRE(c <= c);
      REQUIRE(!(c > c));
      REQUIRE(c >= c);
   }
}


template< class Container, class Data >
void test_sequence_container(Container & c, Data & d)
{
   assert( d.size() > 2 );
   {
      c.clear();

      REQUIRE( c.size() == 0 );
      REQUIRE( c.empty() );


      {
      typename Data::iterator i = d.begin();
      c.insert( c.begin(), *i );
      REQUIRE( &*c.iterator_to(*c.begin())  == &*i );
      REQUIRE( &*c.iterator_to(*c.cbegin()) == &*i );
      REQUIRE( &*Container::s_iterator_to(*c.begin())  == &*i );
      REQUIRE( &*Container::s_iterator_to(*c.cbegin()) == &*i );
      c.insert( c.end(), *(++i) );
      }
      REQUIRE( c.size() == 2 );
      REQUIRE( !c.empty() );

      typename Container::iterator i;
      i = c.erase( c.begin() );

      REQUIRE( c.size() == 1 );
      REQUIRE( !c.empty() );

      i = c.erase( c.begin() );

      REQUIRE( c.size() == 0 );
      REQUIRE( c.empty() );

      c.insert( c.begin(), *d.begin() );

      REQUIRE( c.size() == 1 );
      REQUIRE( !c.empty() );

      {
      typename Data::iterator di = d.begin();
      ++++di;
      c.insert( c.begin(), *(di) );
      }

      i = c.erase( c.begin(), c.end() );
      REQUIRE( i == c.end() );

      REQUIRE( c.empty() );

      c.insert( c.begin(), *d.begin() );

      REQUIRE( c.size() == 1 );

      REQUIRE( c.begin() != c.end() );

      i = c.erase_and_dispose( c.begin(), detail::null_disposer() );
      REQUIRE( i == c.begin() );

      c.assign(d.begin(), d.end());

      REQUIRE( c.size() == d.size() );

      c.clear();

      REQUIRE( c.size() == 0 );
      REQUIRE( c.empty() );
   }
   {
      c.clear();
      c.insert( c.begin(), d.begin(), d.end() );
      Container move_c(std::move(c));
      REQUIRE( move_c.size() == d.size() );
      REQUIRE( c.empty());

      c = std::move(move_c);
      REQUIRE( c.size() == d.size() );
      REQUIRE( move_c.empty());
   }
}

template< class Container, class Data >
void test_common_unordered_and_associative_container(Container & c, Data & d, std::true_type unordered)
{
   (void)unordered;
   typedef typename Container::key_of_value     key_of_value;
   typedef typename Container::iterator         iterator;
   typedef typename Container::const_iterator   const_iterator;

   assert( d.size() > 2 );

   c.clear();
   c.insert(d.begin(), d.end());

   {
      Container const &cc = c;
      for( typename Data::const_iterator di = d.begin(), de = d.end();
         di != de; ++di )
      {
         REQUIRE( cc.find(key_of_value()(*di), c.hash_function(), c.key_eq()) != cc.end() );
         std::pair<const_iterator, const_iterator> rdi = cc.equal_range(key_of_value()(*di), c.hash_function(), c.key_eq());
         REQUIRE(rdi.first != rdi.second);
         REQUIRE(std::size_t(boost::intrusive::iterator_distance(rdi.first, rdi.second)) == cc.count(key_of_value()(*di), c.hash_function(), c.key_eq()));
      }

      for( iterator ci = c.begin(), ce = c.end(); ci != ce; )
      {
         REQUIRE( c.find(key_of_value()(*ci)) != c.end() );
         std::pair<iterator, iterator> rci = c.equal_range(key_of_value()(*ci), c.hash_function(), c.key_eq());
         REQUIRE(rci.first != rci.second);
         std::size_t const sc = c.count(key_of_value()(*ci), c.hash_function(), c.key_eq());
         REQUIRE(std::size_t(boost::intrusive::iterator_distance(rci.first, rci.second)) == sc);
         REQUIRE(sc == c.erase(key_of_value()(*ci), c.hash_function(), c.key_eq()));
         ci = rci.second;
      }
      REQUIRE(c.empty());
   }

   c.clear();
   c.insert(d.begin(), d.end());

   typename Data::const_iterator db = d.begin();
   typename Data::const_iterator da = db++;

   std::size_t old_size = c.size();

   c.erase(key_of_value()(*da), c.hash_function(), c.key_eq());
   REQUIRE( c.size() == old_size-1 );
   //This should not eras anyone
   std::size_t second_erase = c.erase_and_dispose
      ( key_of_value()(*da), c.hash_function(), c.key_eq(), detail::null_disposer() );
   REQUIRE( second_erase == 0 );

   REQUIRE( c.count(key_of_value()(*da), c.hash_function(), c.key_eq()) == 0 );
   REQUIRE( c.count(key_of_value()(*db), c.hash_function(), c.key_eq()) != 0 );

   REQUIRE( c.find(key_of_value()(*da), c.hash_function(), c.key_eq()) == c.end() );
   REQUIRE( c.find(key_of_value()(*db), c.hash_function(), c.key_eq()) != c.end() );

   REQUIRE( c.equal_range(key_of_value()(*db), c.hash_function(), c.key_eq()).first != c.end() );

   c.clear();

   REQUIRE( c.equal_range(key_of_value()(*da), c.hash_function(), c.key_eq()).first == c.end() );

   //
   //suggested_upper_bucket_count
   //
   //Maximum fallbacks to the highest possible value
   std::size_t sz = Container::suggested_upper_bucket_count(std::size_t(-1));
   REQUIRE( sz > std::size_t(-1)/2 );
   //In the rest of cases the upper bound is returned
   sz = Container::suggested_upper_bucket_count(std::size_t(-1)/2);
   REQUIRE( sz >= std::size_t(-1)/2 );
   sz = Container::suggested_upper_bucket_count(std::size_t(-1)/4);
   REQUIRE( sz >= std::size_t(-1)/4 );
   sz = Container::suggested_upper_bucket_count(0);
   REQUIRE( sz > 0 );
   //
   //suggested_lower_bucket_count
   //
   sz = Container::suggested_lower_bucket_count(std::size_t(-1));
   REQUIRE( sz <= std::size_t(-1) );
   //In the rest of cases the lower bound is returned
   sz = Container::suggested_lower_bucket_count(std::size_t(-1)/2);
   REQUIRE( sz <= std::size_t(-1)/2 );
   sz = Container::suggested_lower_bucket_count(std::size_t(-1)/4);
   REQUIRE( sz <= std::size_t(-1)/4 );
   //Minimum fallbacks to the lowest possible value
   sz = Container::suggested_upper_bucket_count(0);
   REQUIRE( sz > 0 );
}

template< class Container, class Data >
void test_common_unordered_and_associative_container(Container & c, Data & d, std::false_type unordered)
{
   (void)unordered;
   typedef typename Container::key_of_value     key_of_value;
   typedef typename Container::iterator         iterator;
   typedef typename Container::const_iterator   const_iterator;

   assert( d.size() > 2 );

   c.clear();
   typename Container::reference r = *d.begin();
   c.insert(d.begin(), ++d.begin());
   REQUIRE( &*Container::s_iterator_to(*c.begin())  == &r );
   REQUIRE( &*Container::s_iterator_to(*c.cbegin()) == &r );

   c.clear();
   c.insert(d.begin(), d.end());
   {
      Container const &cc = c;
      for( typename Data::const_iterator di = d.begin(), de = d.end();
         di != de; ++di )
      {
         REQUIRE( cc.find(key_of_value()(*di), c.key_comp()) != cc.end() );
         std::pair<const_iterator, const_iterator> rdi = cc.equal_range(key_of_value()(*di), c.key_comp());
         REQUIRE(rdi.first != rdi.second);
         REQUIRE(std::size_t(boost::intrusive::iterator_distance(rdi.first, rdi.second)) == cc.count(key_of_value()(*di), c.key_comp()));
      }

      for( iterator ci = c.begin(), ce = c.end(); ci != ce; )
      {
         REQUIRE( c.find(key_of_value()(*ci)) != c.end() );
         std::pair<iterator, iterator> rci = c.equal_range(key_of_value()(*ci), c.key_comp());
         REQUIRE(rci.first != rci.second);
         std::size_t const sc = c.count(key_of_value()(*ci), c.key_comp());
         REQUIRE(std::size_t(boost::intrusive::iterator_distance(rci.first, rci.second)) == sc);
         REQUIRE(sc == c.erase(key_of_value()(*ci), c.key_comp()));
         ci = rci.second;
      }
      REQUIRE(c.empty());
   }

   c.clear();
   c.insert(d.begin(), d.end());

   typename Data::const_iterator db = d.begin();
   typename Data::const_iterator da = db++;

   std::size_t old_size = c.size();

   c.erase(key_of_value()(*da), c.key_comp());
   REQUIRE( c.size() == old_size-1 );
   //This should not erase any
   std::size_t second_erase = c.erase_and_dispose( key_of_value()(*da), c.key_comp(), detail::null_disposer() );
   REQUIRE( second_erase == 0 );

   REQUIRE( c.count(key_of_value()(*da), c.key_comp()) == 0 );
   REQUIRE( c.count(key_of_value()(*db), c.key_comp()) != 0 );
   REQUIRE( c.find(key_of_value()(*da), c.key_comp()) == c.end() );
   REQUIRE( c.find(key_of_value()(*db), c.key_comp()) != c.end() );
   REQUIRE( c.equal_range(key_of_value()(*db), c.key_comp()).first != c.end() );
   c.clear();
   REQUIRE( c.equal_range(key_of_value()(*da), c.key_comp()).first == c.end() );
}

template< class Container, class Data >
void test_common_unordered_and_associative_container(Container & c, Data & d)
{
   typedef typename Container::key_of_value     key_of_value;
   typedef typename Container::iterator         iterator;
   typedef typename Container::const_iterator   const_iterator;

   {
      assert( d.size() > 2 );

      c.clear();
      typename Container::reference r = *d.begin();
      c.insert(d.begin(), ++d.begin());
      REQUIRE( &*c.iterator_to(*c.begin())  == &r );
      REQUIRE( &*c.iterator_to(*c.cbegin()) == &r );

      c.clear();
      c.insert(d.begin(), d.end());

      Container const &cc = c;
      for( typename Data::const_iterator di = d.begin(), de = d.end();
         di != de; ++di )
      {
         REQUIRE( cc.find(key_of_value()(*di)) != cc.end() );
         std::pair<const_iterator, const_iterator> rdi = cc.equal_range(key_of_value()(*di));
         REQUIRE(rdi.first != rdi.second);
         REQUIRE(std::size_t(boost::intrusive::iterator_distance(rdi.first, rdi.second)) == cc.count(key_of_value()(*di)));
      }

      for( iterator ci = c.begin(), ce = c.end(); ci != ce; )
      {
         REQUIRE( c.find(key_of_value()(*ci)) != c.end() );
         std::pair<iterator, iterator> rci = c.equal_range(key_of_value()(*ci));
         REQUIRE(rci.first != rci.second);
         std::size_t const sc = c.count(key_of_value()(*ci));
         REQUIRE(std::size_t(boost::intrusive::iterator_distance(rci.first, rci.second)) == sc);
         REQUIRE(sc == c.erase(key_of_value()(*ci)));
         ci = rci.second;
      }
      REQUIRE(c.empty());
   }
   {
      c.clear();
      c.insert(d.begin(), d.end());

      typename Data::const_iterator db = d.begin();
      typename Data::const_iterator da = db++;

      std::size_t old_size = c.size();

      c.erase(key_of_value()(*da));
      REQUIRE( c.size() == old_size-1 );
      //This should erase nothing
      std::size_t second_erase = c.erase_and_dispose( key_of_value()(*da), detail::null_disposer() );
      REQUIRE( second_erase == 0 );

      REQUIRE( c.count(key_of_value()(*da)) == 0 );
      REQUIRE( c.count(key_of_value()(*db)) != 0 );

      REQUIRE( c.find(key_of_value()(*da)) == c.end() );
      REQUIRE( c.find(key_of_value()(*db)) != c.end() );

      REQUIRE( c.equal_range(key_of_value()(*db)).first != c.end() );
      REQUIRE( c.equal_range(key_of_value()(*da)).first == c.equal_range(key_of_value()(*da)).second );
   }
   {
      c.clear();
      c.insert( d.begin(), d.end() );
      std::size_t orig_size = c.size();
      Container move_c(std::move(c));
      REQUIRE(orig_size == move_c.size());
      REQUIRE( c.empty());
      for( typename Data::const_iterator di = d.begin(), de = d.end();
         di != de; ++di )
      {  REQUIRE( move_c.find(key_of_value()(*di)) != move_c.end() );   }

      c = std::move(move_c);
      for( typename Data::const_iterator di = d.begin(), de = d.end();
         di != de; ++di )
      {  REQUIRE( c.find(key_of_value()(*di)) != c.end() );   }
      REQUIRE( move_c.empty());
   }
   using enabler = std::bool_constant<is_unordered<Container, typename Container::key_type>>;
   test_common_unordered_and_associative_container(c, d, enabler());
}

template< class Container, class Data >
void test_associative_container_invariants(Container & c, Data & d)
{
   typedef typename Container::const_iterator   const_iterator;
   typedef typename Container::key_of_value     key_of_value;
   for( typename Data::const_iterator di = d.begin(), de = d.end();
      di != de; ++di)
   {
      const_iterator ci = c.find(key_of_value()(*di));
      REQUIRE( ci != c.end() );
      REQUIRE( ! c.value_comp()(*ci, *di) );
      REQUIRE( ! c.key_comp()(key_of_value()(*ci), key_of_value()(*di)) );
      const_iterator cil = c.lower_bound(key_of_value()(*di));
      const_iterator ciu = c.upper_bound(key_of_value()(*di));
      std::pair<const_iterator, const_iterator> er = c.equal_range(key_of_value()(*di));
      REQUIRE( cil == er.first );
      REQUIRE( ciu == er.second );
      if(ciu != c.end()){
         REQUIRE( c.value_comp()(*cil, *ciu) );
         REQUIRE( c.key_comp()(key_of_value()(*cil), key_of_value()(*ciu)) );
      }
      if(c.count(key_of_value()(*di)) > 1){
         const_iterator ci_next = cil; ++ci_next;
         for( ; ci_next != ciu; ++cil, ++ci_next){
            REQUIRE( !c.value_comp()(*ci_next, *cil) );
            REQUIRE( !c.key_comp()(key_of_value()(*ci_next), key_of_value()(*cil)) );
         }
      }
   }
}

template< class Container, class Data >
void test_associative_container(Container & c, Data & d)
{
   assert( d.size() > 2 );

   c.clear();
   c.insert(d.begin(),d.end());

   test_associative_container_invariants(c, d);

   const Container & cr = c;

   test_associative_container_invariants(cr, d);
}

template< class Container, class Data >
void test_unordered_associative_container_invariants(Container & c, Data & d)
{
   typedef typename Container::const_iterator   const_iterator;
   typedef typename Container::key_of_value     key_of_value;

   for( typename Data::const_iterator di = d.begin(), de = d.end() ;
      di != de ; ++di ){
      const_iterator i = c.find(key_of_value()(*di));
      std::size_t nb = c.bucket(key_of_value()(*i));
      std::size_t bucket_elem = boost::intrusive::iterator_distance(c.begin(nb), c.end(nb));
      REQUIRE( bucket_elem ==  c.bucket_size(nb) );
      REQUIRE( &*c.local_iterator_to(*c.find(key_of_value()(*di))) == &*i );
      REQUIRE( &*c.local_iterator_to(*const_cast<const Container &>(c).find(key_of_value()(*di))) == &*i );
      REQUIRE( &*Container::s_local_iterator_to(*c.find(key_of_value()(*di))) == &*i );
      REQUIRE( &*Container::s_local_iterator_to(*const_cast<const Container &>(c).find(key_of_value()(*di))) == &*i );
      std::pair<const_iterator, const_iterator> er = c.equal_range(key_of_value()(*di));
      std::size_t cnt = boost::intrusive::iterator_distance(er.first, er.second);
      REQUIRE( cnt == c.count(key_of_value()(*di)));
      if(cnt > 1){
         const_iterator n = er.first;
         i = n++;
         const_iterator e = er.second;
         for(; n != e; ++i, ++n){
            REQUIRE( c.key_eq()(key_of_value()(*i), key_of_value()(*n)) );
            REQUIRE( (c.hash_function()(key_of_value()(*i))) == (c.hash_function()(key_of_value()(*n))) );
         }
      }
   }

   std::size_t blen = c.bucket_count();
   std::size_t total_objects = 0;
   for(std::size_t i = 0; i < blen; ++i){
      total_objects += c.bucket_size(i);
   }
   REQUIRE( total_objects ==  c.size() );
}

template< class Container, class Data >
void test_unordered_associative_container(Container & c, Data & d)
{
   c.clear();
   c.insert( d.begin(), d.end() );

   test_unordered_associative_container_invariants(c, d);

   const Container & cr = c;

   test_unordered_associative_container_invariants(cr, d);
}

template< class Container, class Data >
void test_unique_container(Container & c, Data & d)
{
   //typedef typename Container::value_type value_type;
   c.clear();
   c.insert(d.begin(),d.end());
   std::size_t old_size = c.size();
   //value_type v(*d.begin());
   //c.insert(v);
   Data d2(1);
   (&d2.front())->value_ = (&d.front())->value_;
   c.insert(d2.front());
   REQUIRE( c.size() == old_size );
   c.clear();
}

template< class Container, class Data >
void test_non_unique_container(Container & c, Data & d)
{
   //typedef typename Container::value_type value_type;
   c.clear();
   c.insert(d.begin(),d.end());
   std::size_t old_size = c.size();
   //value_type v(*d.begin());
   //c.insert(v);
   Data d2(1);
   (&d2.front())->value_ = (&d.front())->value_;
   c.insert(d2.front());
   REQUIRE( c.size() == (old_size+1) );
   c.clear();
}

template< class Container, class Data >
void test_maybe_unique_container(Container & c, Data & d, std::false_type)//is_unique
{  test_unique_container(c, d);  }

template< class Container, class Data >
void test_maybe_unique_container(Container & c, Data & d, std::true_type)//!is_unique
{  test_non_unique_container(c, d);  }

}}}

#endif   //#ifndef BOOST_INTRUSIVE_TEST_CONTAINER_HPP
