#include "common_functors.hpp"
#include <boost/intrusive/options.hpp>
#include <boost/intrusive/detail/iterator.hpp>
#include <boost/intrusive/detail/mpl.hpp>
#include "test_macros.hpp"
#include "test_container.hpp"
#include "generic_assoc_test.hpp"
#include <typeinfo>
#include <boost/intrusive/priority_compare.hpp>

namespace boost{
namespace intrusive{
namespace test{

template<class ContainerDefiner>
struct test_generic_set
{
   static void test_all();
   private:
   typedef typename ContainerDefiner::value_cont_type    value_cont_type;
   static void test_sort(value_cont_type&);
   static void test_insert(value_cont_type&);
   static void test_insert_advanced(value_cont_type&, std::true_type);
   static void test_insert_advanced(value_cont_type&, std::false_type);
   static void test_swap(value_cont_type&);
   static void test_merge(value_cont_type&);
   static void test_find(value_cont_type&);
   static void test_impl();
};


template<class ContainerDefiner>
void test_generic_set<ContainerDefiner>::test_all()
{
   typedef typename ContainerDefiner::template container
      <>::type set_type;
   {
      static const int random_init[6] = { 3, 2, 4, 1, 5, 2 };
      value_cont_type values(6);
      for (int i = 0; i < 6; ++i)
         (&values[i])->value_ = random_init[i];

      {
         set_type testset(values.begin(), values.end());
         test::test_container(testset);
         testset.clear();
         testset.insert(values.begin(), values.end());
         test::test_common_unordered_and_associative_container(testset, values);
         testset.clear();
         testset.insert(values.begin(), values.end());
         test::test_associative_container(testset, values);
         testset.clear();
         testset.insert(values.begin(), values.end());
         test::test_unique_container(testset, values);
      }

      test_sort(values);
      test_insert(values);
      test_insert_advanced(values, std::bool_constant< is_treap< set_type > >());
      test_swap(values);
      test_merge(values);
      test_find(values);
      test_impl();
      test_generic_assoc<ContainerDefiner>::test_all(values);
   }
   {
      value_cont_type values(6);
      for (int i = 0; i < 6; ++i)
         (&values[i])->value_ = i+1;
      set_type testset(values.begin(), values.end());
      test::test_iterator_bidirectional(testset);
   }
}

//test case due to an error in tree implementation:
template<class ContainerDefiner>
void test_generic_set<ContainerDefiner>::test_impl()
{
   value_cont_type values (5);
   for (int i = 0; i < 5; ++i)
      (&values[i])->value_ = i;

   typedef typename ContainerDefiner::template container
      <>::type set_type;
   set_type testset;
   for (int i = 0; i < 5; ++i)
      testset.insert (values[i]);

   testset.erase (testset.iterator_to (values[0]));
   testset.erase (testset.iterator_to (values[1]));
   testset.insert (values[1]);

   testset.erase (testset.iterator_to (values[2]));
   testset.erase (testset.iterator_to (values[3]));
}

//test: constructor, iterator, clear, reverse_iterator, front, back, size:
template<class ContainerDefiner>
void test_generic_set<ContainerDefiner>::test_sort(value_cont_type& values)
{
   typedef typename ContainerDefiner::template container
      <>::type set_type;

   set_type testset1 (values.begin(), values.end());
   {  int init_values [] = { 1, 2, 3, 4, 5 };
      TEST_INTRUSIVE_SEQUENCE( init_values, testset1.begin() );  }

   testset1.clear();
   REQUIRE (testset1.empty());

   typedef typename ContainerDefiner::template container
      < compare<even_odd> >::type set_type2;

   set_type2 testset2 (values.begin(), values.begin() + 6);
   {  int init_values [] = { 5, 3, 1, 4, 2 };
      TEST_INTRUSIVE_SEQUENCE( init_values, testset2.rbegin() );  }
   REQUIRE (testset2.begin()->value_ == 2);
   REQUIRE (testset2.rbegin()->value_ == 5);
}

//test: insert, const_iterator, const_reverse_iterator, erase, s_iterator_to:
template<class ContainerDefiner>
void test_generic_set<ContainerDefiner>::test_insert(value_cont_type& values)
{
   typedef typename ContainerDefiner::template container
      <>::type set_type;
   {
      set_type testset;
      testset.insert(values.begin() + 2, values.begin() + 5);
      testset.check();

      const set_type& const_testset = testset;
      {  int init_values [] = { 1, 4, 5 };
         TEST_INTRUSIVE_SEQUENCE( init_values, const_testset.begin() );  }

      typename set_type::iterator i = testset.begin();
      REQUIRE (i->value_ == 1);

      i = testset.insert (i, values[0]);
      testset.check();
      REQUIRE (&*i == &values[0]);

      {  int init_values [] = { 5, 4, 3, 1 };
         TEST_INTRUSIVE_SEQUENCE( init_values, testset.rbegin() );  }

      i = testset.iterator_to (values[2]);
      REQUIRE (&*i == &values[2]);

      i = set_type::s_iterator_to(values[2]);
      REQUIRE (&*i == &values[2]);

      typedef typename value_cont_type::const_reference const_reference;
      typename set_type::const_iterator ic;
      ic = testset.iterator_to (static_cast< const_reference >(values[2]));
      REQUIRE (&*ic == &values[2]);
      ic = set_type::s_iterator_to (static_cast< const_reference >(values[2]));
      REQUIRE (&*ic == &values[2]);

      testset.erase (i);
      testset.check();
      {  int init_values [] = { 1, 3, 5 };
         TEST_INTRUSIVE_SEQUENCE( init_values, testset.begin() );  }
   }
}

// treap version
template<class ValueType, class KeyType>
struct prio_comp
   : priority_compare<int>
{
   bool operator()(const ValueType &v, const KeyType &k) const
   {  return this->priority_compare<int>::operator()(v.int_value(), k.int_value());  }

   bool operator()(const KeyType &k, const ValueType &v) const
   {  return this->priority_compare<int>::operator()(k.int_value(), v.int_value());  }
};

template<class ContainerDefiner>
void test_generic_set<ContainerDefiner>::test_insert_advanced
(value_cont_type& values, std::true_type)
{
   typedef typename ContainerDefiner::template container
      <>::type set_type;
   typedef typename set_type::key_of_value key_of_value;
   typedef typename set_type::priority_of_value priority_of_value;
   typedef typename set_type::value_type  value_type;
   typedef priority_compare<> prio_comp_t;
   {
      set_type testset;
      testset.insert(values.begin(), values.begin() + values.size());
      testset.check();
      value_type v(1);
      typename set_type::insert_commit_data data;
      REQUIRE ((!testset.insert_check(1, any_less(), 1, prio_comp_t(), data).second));
      REQUIRE ((!testset.insert_check(testset.begin(), 1, any_less(), 1, prio_comp_t(), data).second));
      REQUIRE ((!testset.insert_check(key_of_value()(v), priority_of_value()(v), data).second));
      REQUIRE ((!testset.insert_check(testset.begin(), key_of_value()(v), priority_of_value()(v), data).second));
   }
}

//test: insert, const_iterator, const_reverse_iterator, erase, s_iterator_to:
template<class ContainerDefiner>
void test_generic_set<ContainerDefiner>::test_insert_advanced
(value_cont_type& values, std::false_type)
{
   typedef typename ContainerDefiner::template container
      <>::type set_type;
   typedef typename set_type::key_of_value   key_of_value;
   typedef typename set_type::value_type     value_type;
   {
      set_type testset;
      testset.insert(values.begin(), values.begin() + values.size());
      testset.check();
      value_type v(1);
      typename set_type::insert_commit_data data;
      REQUIRE ((!testset.insert_check(1, any_less(), data).second));
      REQUIRE ((!testset.insert_check(key_of_value()(v), data).second));
      REQUIRE ((!testset.insert_check(testset.begin(), 1, any_less(), data).second));
      REQUIRE ((!testset.insert_check(testset.begin(), key_of_value()(v), data).second));
   }
}

//test: insert (seq-version), swap, erase (seq-version), size:
template<class ContainerDefiner>
void test_generic_set<ContainerDefiner>::test_swap(value_cont_type& values)
{
   typedef typename ContainerDefiner::template container
      <>::type set_type;
   set_type testset1 (values.begin(), values.begin() + 2);
   set_type testset2;
   testset2.insert (values.begin() + 2, values.begin() + 6);
   testset1.swap (testset2);

   {  int init_values [] = { 1, 2, 4, 5 };
      TEST_INTRUSIVE_SEQUENCE( init_values, testset1.begin() );  }

   {  int init_values [] = { 2, 3 };
      TEST_INTRUSIVE_SEQUENCE( init_values, testset2.begin() );  }

   testset1.erase (testset1.iterator_to(values[5]), testset1.end());
   REQUIRE (testset1.size() == 1);
   //  REQUIRE (&testset1.front() == &values[3]);
   REQUIRE (&*testset1.begin() == &values[3]);
}

template<class ContainerDefiner>
void test_generic_set<ContainerDefiner>::test_merge(value_cont_type& values)
{
   typedef typename ContainerDefiner::template container
      <>::type set_type;
   typedef typename set_type::key_type key_type;

   typedef typename ContainerDefiner::template container
      < compare< std::greater<key_type> > >::type set_greater_type;

   //2,3
   set_type testset1 (values.begin(), values.begin() + 2);
   //5, 4, 2, 1
   set_greater_type testset2;
   testset2.insert (values.begin() + 2, values.begin() + 6);

   testset2.merge(testset1);
   testset1.check();
   testset2.check();

   REQUIRE (testset1.size() == 1);
   {  int init_values [] = { 2 };
      TEST_INTRUSIVE_SEQUENCE( init_values, testset1.begin() );  }
   REQUIRE (&*testset1.begin() == &values[1]);

   REQUIRE (testset2.size() == 5);
   {  int init_values [] = { 5, 4, 3, 2, 1 };
      TEST_INTRUSIVE_SEQUENCE( init_values, testset2.begin() );  }

   testset1.merge(testset2);
   testset1.check();
   testset2.check();

   REQUIRE (testset1.size() == 5);
   {  int init_values [] = { 1, 2, 3, 4, 5 };
      TEST_INTRUSIVE_SEQUENCE( init_values, testset1.begin() );  }

   REQUIRE (testset2.size() == 1);
   {  int init_values [] = { 2 };
      TEST_INTRUSIVE_SEQUENCE( init_values, testset2.begin() );  }
   REQUIRE (&*testset2.begin() == &values[5]);
}

//test: find, equal_range (lower_bound, upper_bound), bounded_range:
template<class ContainerDefiner>
void test_generic_set<ContainerDefiner>::test_find(value_cont_type& values)
{
   typedef typename ContainerDefiner::template container
      <>::type set_type;
   set_type testset (values.begin(), values.end());
   typedef typename set_type::iterator       iterator;
   typedef typename set_type::const_iterator const_iterator;
   typedef typename set_type::key_of_value   key_of_value;
   typedef typename value_cont_type::reference reference;

   {
      //value_type cmp_val;
      value_cont_type cmp_val_cont(1);
      reference cmp_val = cmp_val_cont.front();
      (&cmp_val)->value_ = 2;
      iterator i = testset.find(key_of_value()(cmp_val));
      REQUIRE (i == testset.find(2, any_less()));
      REQUIRE (i->value_ == 2);
      REQUIRE ((++i)->value_ != 2);

      std::pair<iterator,iterator> range = testset.equal_range (key_of_value()(cmp_val));
      REQUIRE(range == testset.equal_range (2, any_less()));

      REQUIRE (range.first->value_ == 2);
      REQUIRE (range.second->value_ == 3);
      REQUIRE (boost::intrusive::iterator_distance (range.first, range.second) == 1);

      (&cmp_val)->value_ = 7;
      REQUIRE (testset.find (key_of_value()(cmp_val)) == testset.end());
      REQUIRE (testset.find (7, any_less()) == testset.end());
   }

   {
      const set_type &const_testset = testset;
      std::pair<iterator,iterator> range;
      std::pair<const_iterator, const_iterator> const_range;
      //value_type cmp_val_lower, cmp_val_upper;
      value_cont_type cmp_val_cont(2);
      reference cmp_val_lower = cmp_val_cont.front();
      reference cmp_val_upper = cmp_val_cont.back();
      {
      (&cmp_val_lower)->value_ = 1;
      (&cmp_val_upper)->value_ = 2;
      //left-closed, right-closed
      range = testset.bounded_range (key_of_value()(cmp_val_lower), key_of_value()(cmp_val_upper), true, true);
      REQUIRE (range == testset.bounded_range (1, 2, any_less(), true, true));
      REQUIRE (range.first->value_ == 1);
      REQUIRE (range.second->value_ == 3);
      REQUIRE (boost::intrusive::iterator_distance (range.first, range.second) == 2);
      }
      {
      (&cmp_val_lower)->value_ = 1;
      (&cmp_val_upper)->value_ = 2;
      const_range = const_testset.bounded_range (key_of_value()(cmp_val_lower), key_of_value()(cmp_val_upper), true, false);
      REQUIRE (const_range == const_testset.bounded_range (1, 2, any_less(), true, false));
      REQUIRE (const_range.first->value_ == 1);
      REQUIRE (const_range.second->value_ == 2);
      REQUIRE (boost::intrusive::iterator_distance (const_range.first, const_range.second) == 1);

      (&cmp_val_lower)->value_ = 1;
      (&cmp_val_upper)->value_ = 3;
      range = testset.bounded_range (key_of_value()(cmp_val_lower), key_of_value()(cmp_val_upper), true, false);
      REQUIRE (range == testset.bounded_range (1, 3, any_less(), true, false));
      REQUIRE (range.first->value_ == 1);
      REQUIRE (range.second->value_ == 3);
      REQUIRE (boost::intrusive::iterator_distance (range.first, range.second) == 2);
      }
      {
      (&cmp_val_lower)->value_ = 1;
      (&cmp_val_upper)->value_ = 2;
      const_range = const_testset.bounded_range (key_of_value()(cmp_val_lower), key_of_value()(cmp_val_upper), false, true);
      REQUIRE (const_range == const_testset.bounded_range (1, 2, any_less(), false, true));
      REQUIRE (const_range.first->value_ == 2);
      REQUIRE (const_range.second->value_ == 3);
      REQUIRE (boost::intrusive::iterator_distance (const_range.first, const_range.second) == 1);
      }
      {
      (&cmp_val_lower)->value_ = 1;
      (&cmp_val_upper)->value_ = 2;
      range = testset.bounded_range (key_of_value()(cmp_val_lower), key_of_value()(cmp_val_upper), false, false);
      REQUIRE (range == testset.bounded_range (1, 2, any_less(), false, false));
      REQUIRE (range.first->value_ == 2);
      REQUIRE (range.second->value_ == 2);
      REQUIRE (boost::intrusive::iterator_distance (range.first, range.second) == 0);
      }
      {
      (&cmp_val_lower)->value_ = 5;
      (&cmp_val_upper)->value_ = 6;
      const_range = const_testset.bounded_range (key_of_value()(cmp_val_lower), key_of_value()(cmp_val_upper), true, false);
      REQUIRE (const_range == const_testset.bounded_range (5, 6, any_less(), true, false));
      REQUIRE (const_range.first->value_ == 5);
      REQUIRE (const_range.second == const_testset.end());
      REQUIRE (boost::intrusive::iterator_distance (const_range.first, const_range.second) == 1);
      }
   }
}

}}}   //namespace boost::intrusive::test

