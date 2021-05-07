/////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Olaf Krzikalla 2004-2006.
// (C) Copyright Ion Gaztanaga  2006-2013.
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/intrusive for documentation.
//
/////////////////////////////////////////////////////////////////////////////
#include "common_functors.hpp"
#include <boost/intrusive/options.hpp>
#include <boost/intrusive/detail/mpl.hpp>
#include "test_macros.hpp"
#include "test_container.hpp"

namespace boost{
namespace intrusive{
namespace test{

template <typename T>
concept has_splay = requires(T t) {
    { t.splay() };
};

template <typename T>
concept has_rebalance = requires(T t) {
    { t.rebalance() };
};

template <typename T>
concept has_insert_before = requires(T t) {
    { t.insert_before() };
};

template <typename T>
concept is_treap = requires(T t) {
    { t.priority_comp() };
};

template <typename ContainerDefiner>
struct test_generic_assoc
{
   using value_cont_type = typename ContainerDefiner::value_cont_type;

   static void test_all(value_cont_type&);
   template <typename assoc_type>
   static void test_root(value_cont_type&);
   template <typename assoc_type>
   static void test_clone(value_cont_type&);
   template <typename assoc_type>
   static void test_insert_erase_burst();
   template <typename assoc_type>
   static void test_container_from_end(value_cont_type&);
   template <typename assoc_type>
   static void test_splay_up(value_cont_type&);
   template <typename assoc_type>
   static void test_splay_down(value_cont_type&);
   template <typename assoc_type>
   static void test_rebalance(value_cont_type&);
   template <typename assoc_type>
   static void test_insert_before(value_cont_type&);
   template <typename assoc_type>
   static void test_container_from_iterator(value_cont_type&);
};

template <typename ContainerDefiner>
template <typename assoc_type>
void test_generic_assoc<ContainerDefiner>::
   test_container_from_iterator(value_cont_type& values)
{
   assoc_type testset(values.begin(), values.end());
   auto sz = testset.size();
   for (auto b(testset.begin()), e(testset.end()); b != e; ++b)
   {
      assoc_type &s = assoc_type::container_from_iterator(b);
      const assoc_type &cs = assoc_type::container_from_iterator(b);
      REQUIRE(&s == &cs);
      REQUIRE(&s == &testset);
      s.erase(b);
      REQUIRE(testset.size() == (sz-1));
      s.insert(*b);
      REQUIRE(testset.size() == sz);
   }
}

template <typename ContainerDefiner>
template <typename assoc_type>
void test_generic_assoc<ContainerDefiner>::test_insert_erase_burst()
{
   //value_cont_type values;
   const std::size_t MaxValues = 200;
   value_cont_type values(MaxValues);
   for(std::size_t i = 0; i != MaxValues; ++i){
      (&values[i])->value_ = i;
   }

   {  //Ordered insertion + erasure
      assoc_type testset (values.begin(), values.begin() + values.size());
      TEST_INTRUSIVE_SEQUENCE_EXPECTED(testset, testset.begin());
      testset.check();
      auto it(testset.begin()), itend(testset.end());
      for(std::size_t i = 0; it != itend; ++i){
         REQUIRE(&*it == &values[i]);
         it = testset.erase(it);
         testset.check();
      }
      REQUIRE(testset.empty());
   }

   {  //Now random insertions + erasure
      assoc_type testset;
      std::vector<decltype(values.begin())> it_vector;
      //Random insertion
      for (auto it(values.begin()), itend(values.end())
         ; it != itend
         ; ++it){
         it_vector.push_back(it);
      }
      for(std::size_t i = 0; i != MaxValues; ++i){
         testset.insert(*it_vector[i]);
         testset.check();
      }
      TEST_INTRUSIVE_SEQUENCE_EXPECTED(testset, testset.begin());
      //Random erasure
      std::random_device rng;
      std::mt19937 urng(rng());
      std::shuffle(it_vector.begin(), it_vector.end(), urng);
      for(std::size_t i = 0; i != MaxValues; ++i){
         testset.erase(testset.iterator_to(*it_vector[i]));
         testset.check();
      }
      REQUIRE(testset.empty());
   }
}

template <typename ContainerDefiner>
void test_generic_assoc<ContainerDefiner>::test_all(value_cont_type& values)
{
   using assoc_type = typename ContainerDefiner::template container<>::type;
   test_root<assoc_type>(values);
   test_clone<assoc_type>(values);
   if constexpr (assoc_type::has_container_from_iterator) test_container_from_end<assoc_type>(values);
   if constexpr (has_splay<assoc_type>) {
       test_splay_up<assoc_type>(values);
       test_splay_down<assoc_type>(values);
   }
   if constexpr (has_rebalance<assoc_type>) test_rebalance<assoc_type>(values);
   if constexpr (has_insert_before<assoc_type>) test_insert_before(values);
   test_insert_erase_burst<assoc_type>();
   if constexpr (assoc_type::has_container_from_iterator) test_container_from_iterator<assoc_type>(values);
}

template <typename ContainerDefiner>
template <typename assoc_type>
void test_generic_assoc<ContainerDefiner>::test_root(value_cont_type& values)
{
   using iterator = typename assoc_type::iterator;
   using const_iterator = typename assoc_type::const_iterator;

   assoc_type testset1;
   const assoc_type &ctestset1 = testset1;;

   REQUIRE( testset1.root()  ==  testset1.end());
   REQUIRE(ctestset1.root()  == ctestset1.cend());
   REQUIRE( testset1.croot() == ctestset1.cend());


   testset1.insert(values.begin(), values.begin() + values.size());

   iterator i = testset1.root();
   iterator i2(i);
   REQUIRE( i.go_parent().go_parent() == i2);

   const_iterator ci = ctestset1.root();
   const_iterator ci2(ci);
   REQUIRE( ci.go_parent().go_parent() == ci2);

   ci = testset1.croot();
   ci2 = ci;
   REQUIRE( ci.go_parent().go_parent() == ci2);
}

template <typename ContainerDefiner>
template <typename assoc_type>
void test_generic_assoc<ContainerDefiner>::test_clone(value_cont_type& values)
{
    using value_type = typename assoc_type::value_type;

	assoc_type testset1(values.begin(), values.begin() + values.size());
	assoc_type testset2;


	const auto testset1_oldsize = testset1.size();
	testset2.clone_from(testset1, test::new_cloner<value_type>(), test::delete_disposer<value_type>());
	REQUIRE(testset1.size() == testset1_oldsize);
	REQUIRE(testset2 == testset1);
	testset2.clear_and_dispose(test::delete_disposer<value_type>());
	REQUIRE(testset2.empty());

	//Now test move clone
	testset2.clone_from(std::move(testset1), test::new_nonconst_cloner<value_type>(), test::delete_disposer<value_type>());
	REQUIRE(testset2 == testset1);
	testset2.clear_and_dispose(test::delete_disposer<value_type>());
	REQUIRE(testset2.empty());
}

template <typename ContainerDefiner>
template <typename assoc_type>
void test_generic_assoc<ContainerDefiner>
   ::test_container_from_end(value_cont_type& values)
{
    assoc_type testset(values.begin(), values.begin() + values.size());
    REQUIRE(testset == assoc_type::container_from_end_iterator(testset.end()));
    REQUIRE(testset == assoc_type::container_from_end_iterator(testset.cend()));
}

template <typename ContainerDefiner>
template <typename assoc_type>
void test_generic_assoc<ContainerDefiner>::test_splay_up(value_cont_type& values)
{
   std::size_t num_values;
   auto original_testset = [&] {
      assoc_type testset (values.begin(), values.end());
      num_values = testset.size();
      return value_cont_type(testset.begin(), testset.end());
   }();

   for(std::size_t i = 0; i != num_values; ++i){
      assoc_type testset (values.begin(), values.end());
      {
         auto it = testset.begin();
         for(std::size_t j = 0; j != i; ++j, ++it){}
         testset.splay_up(it);
      }
      REQUIRE (testset.size() == num_values);
      auto it = testset.begin();
      for( auto origit    = original_testset.begin()
         , origitend = original_testset.end()
         ; origit != origitend
         ; ++origit, ++it){
         REQUIRE(*origit == *it);
      }
   }
}

template <typename ContainerDefiner>
template <typename assoc_type>
void test_generic_assoc<ContainerDefiner>::test_splay_down(value_cont_type& values)
{
   std::size_t num_values;
   auto original_testset = [&] {
      assoc_type testset (values.begin(), values.end());
      num_values = testset.size();
      return value_cont_type(testset.begin(), testset.end());
   }();

   for(std::size_t i = 0; i != num_values; ++i){
      assoc_type testset (values.begin(), values.end());
      REQUIRE(testset.size() == num_values);
      {
         auto it = testset.begin();
         for(std::size_t j = 0; j != i; ++j, ++it){}
         REQUIRE(*it == *testset.splay_down(*it));
      }
      REQUIRE (testset.size() == num_values);
      auto it = testset.begin();
      for (auto origit = original_testset.begin()
         , origitend = original_testset.end()
         ; origit != origitend
         ; ++origit, ++it){
         REQUIRE(*origit == *it);
      }
   }
}

template <typename ContainerDefiner>
template <typename assoc_type>
void test_generic_assoc<ContainerDefiner>::test_rebalance(value_cont_type& values)
{
   auto original_testset = [&] {
      assoc_type testset (values.begin(), values.end());
      return value_cont_type(testset.begin(), testset.end());
   }();
   {
      assoc_type testset(values.begin(), values.end());
      testset.rebalance();
      TEST_INTRUSIVE_SEQUENCE_EXPECTED(original_testset, testset.begin());
   }

   {
      std::size_t numdata;
      {
         assoc_type testset(values.begin(), values.end());
         numdata = testset.size();
      }

      for(int i = 0; i != (int)numdata; ++i){
         assoc_type testset(values.begin(), values.end());
         typename assoc_type::iterator it = testset.begin();
         for(int j = 0; j  != i; ++j)  ++it;
         testset.rebalance_subtree(it);
         TEST_INTRUSIVE_SEQUENCE_EXPECTED(original_testset, testset.begin());
      }
   }
}

template <typename ContainerDefiner>
template <typename assoc_type>
void test_generic_assoc<ContainerDefiner>::test_insert_before(value_cont_type& values)
{
   {
      assoc_type testset;
      for (auto it(values.begin()), itend(values.end())
         ; it != itend
         ; ++it){
         testset.push_back(*it);
      }
      REQUIRE(testset.size() == values.size());
      TEST_INTRUSIVE_SEQUENCE_EXPECTED(values, testset.begin());
   }
   {
      assoc_type testset;

      for (auto it(--values.end()); true; --it){
         testset.push_front(*it);
       if(it == values.begin()){
            break;
       }
      }
      REQUIRE(testset.size() == values.size());
      TEST_INTRUSIVE_SEQUENCE_EXPECTED(values, testset.begin());
   }
   {
      assoc_type testset;
      auto it_pos = testset.insert_before(testset.end(), *values.rbegin());
      testset.insert_before(testset.begin(), *values.begin());
      for (auto it(++values.begin()), itend(--values.end())
         ; it != itend
         ; ++it){
         testset.insert_before(it_pos, *it);
      }
      REQUIRE(testset.size() == values.size());
      TEST_INTRUSIVE_SEQUENCE_EXPECTED(values, testset.begin());
   }
}

}}}   //namespace boost::intrusive::test
