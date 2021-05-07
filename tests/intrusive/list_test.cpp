#include "catch.hpp"
#include <boost/intrusive/list.hpp>
#include <boost/intrusive/pointer_traits.hpp>
#include "itestvalue.hpp"
#include "bptr_value.hpp"
#include "smart_ptr.hpp"
#include "common_functors.hpp"
#include <vector>
#include "test_macros.hpp"
#include "test_container.hpp"
#include <typeinfo>

using namespace boost::intrusive;

template <typename VoidPointer>
struct hooks
{
    using unlink_mode = link_mode<link_mode_type::auto_unlink>;
    using void_ptr_t = void_pointer<VoidPointer>;
    using tag_void_t = tag<void>;
    using base_hook_type = list_base_hook<void_ptr_t>;
    using auto_base_hook_type = list_base_hook<unlink_mode, void_ptr_t, tag_void_t>;
    using member_hook_type = list_member_hook<void_ptr_t, tag_void_t>;
    using auto_member_hook_type = list_member_hook<unlink_mode, void_ptr_t>;
    using nonhook_node_member_type = nonhook_node_member<list_node_traits<VoidPointer>, 
                                                         circular_list_algorithms>;
};


template <typename ListType, typename ValueContainer>
struct test_list
{
    using value_traits = typename ListType::value_traits;
    using value_type = typename value_traits::value_type;
    using node_algorithms = typename ListType::node_algorithms;

    static void test_all(ValueContainer&);
    static void test_front_back(ValueContainer&);
    static void test_sort(ValueContainer&);
    static void test_merge(ValueContainer&);
    static void test_remove_unique(ValueContainer&);
    static void test_insert(ValueContainer&);
    static void test_shift(ValueContainer&);
    static void test_swap(ValueContainer&);
    static void test_clone(ValueContainer&);
    static void test_container_from_end(ValueContainer&);
};

template <typename ListType, typename ValueContainer>
void test_list<ListType, ValueContainer>::test_all(ValueContainer& values)
{
    {
        ListType list(values.begin(), values.end());
        test::test_container(list);
        list.clear();
        list.insert(list.end(), values.begin(), values.end());
        test::test_sequence_container(list, values);
    }
    {
        ListType list(values.begin(), values.end());
        test::test_iterator_bidirectional(list);
    }

    test_front_back(values);
    test_sort(values);
    test_merge(values);
    test_remove_unique(values);
    test_insert(values);
    test_shift(values);
    test_swap(values);
    test_clone(values);
    if constexpr (ListType::has_container_from_iterator) test_container_from_end(values);
}

//test: push_front, pop_front, push_back, pop_back, front, back, size, empty:
template < class ListType, typename ValueContainer >
void test_list<ListType, ValueContainer>
::test_front_back(ValueContainer& values)
{
    ListType testlist;
    REQUIRE(testlist.empty());

    testlist.push_back(values[0]);
    REQUIRE(testlist.size() == 1);
    REQUIRE(&testlist.front() == &values[0]);
    REQUIRE(&testlist.back() == &values[0]);

    testlist.push_front(values[1]);
    REQUIRE(testlist.size() == 2);
    REQUIRE(&testlist.front() == &values[1]);
    REQUIRE(&testlist.back() == &values[0]);

    testlist.pop_back();
    REQUIRE(testlist.size() == 1);
    const auto& const_testlist = testlist;
    REQUIRE(&const_testlist.front() == &values[1]);
    REQUIRE(&const_testlist.back() == &values[1]);

    testlist.pop_front();
    REQUIRE(testlist.empty());
}

//test: constructor, iterator, reverse_iterator, sort, reverse:
template < class ListType, typename ValueContainer >
void test_list<ListType, ValueContainer>
::test_sort(ValueContainer& values)
{
    ListType testlist(values.begin(), values.end());

    {  int init_values[] = { 1, 2, 3, 4, 5 };
    TEST_INTRUSIVE_SEQUENCE(init_values, testlist.begin());  }

    testlist.sort(even_odd());
    {  int init_values[] = { 5, 3, 1, 4, 2 };
    TEST_INTRUSIVE_SEQUENCE(init_values, testlist.rbegin());  }

    testlist.reverse();
    {  int init_values[] = { 5, 3, 1, 4, 2 };
    TEST_INTRUSIVE_SEQUENCE(init_values, testlist.begin());  }
}

//test: merge due to error in merge implementation:
template < class ListType, typename ValueContainer >
void test_list<ListType, ValueContainer>
::test_remove_unique(ValueContainer& values)
{
    {
        ListType list(values.begin(), values.end());
        list.remove_if(is_even());
        int init_values[] = { 1, 3, 5 };
        TEST_INTRUSIVE_SEQUENCE(init_values, list.begin());
    }
    {
        ListType list(values.begin(), values.end());
        list.remove_if(is_odd());
        int init_values[] = { 2, 4 };
        TEST_INTRUSIVE_SEQUENCE(init_values, list.begin());
    }
    {
        ListType list(values.begin(), values.end());
        list.remove_and_dispose_if(is_even(), test::empty_disposer());
        int init_values[] = { 1, 3, 5 };
        TEST_INTRUSIVE_SEQUENCE(init_values, list.begin());
    }
    {
        ListType list(values.begin(), values.end());
        list.remove_and_dispose_if(is_odd(), test::empty_disposer());
        int init_values[] = { 2, 4 };
        TEST_INTRUSIVE_SEQUENCE(init_values, list.begin());
    }
    {
        ValueContainer values2(values);
        ListType list(values.begin(), values.end());
        list.insert(list.end(), values2.begin(), values2.end());
        list.sort();
        int init_values[] = { 1, 1, 2, 2, 3, 3, 4, 4, 5, 5 };
        TEST_INTRUSIVE_SEQUENCE(init_values, list.begin());
        list.unique();
        int init_values2[] = { 1, 2, 3, 4, 5 };
        TEST_INTRUSIVE_SEQUENCE(init_values2, list.begin());
    }
}

//test: merge due to error in merge implementation:
template < class ListType, typename ValueContainer >
void test_list<ListType, ValueContainer>
::test_merge(ValueContainer& values)
{
    ListType testlist1, testlist2;
    testlist1.push_front(values[0]);
    testlist2.push_front(values[4]);
    testlist2.push_front(values[3]);
    testlist2.push_front(values[2]);
    testlist1.merge(testlist2);

    int init_values[] = { 1, 3, 4, 5 };
    TEST_INTRUSIVE_SEQUENCE(init_values, testlist1.begin());
}

//test: assign, insert, const_iterator, const_reverse_iterator, erase, s_iterator_to:
template < class ListType, typename ValueContainer >
void test_list<ListType, ValueContainer>
::test_insert(ValueContainer& values)
{
    ListType testlist;
    testlist.assign(values.begin() + 2, values.begin() + 5);

    const auto& const_testlist = testlist;
    {  int init_values[] = { 3, 4, 5 };
    TEST_INTRUSIVE_SEQUENCE(init_values, const_testlist.begin());  }

    auto i = ++testlist.begin();
    REQUIRE(i->value_ == 4);

    {
        typename ListType::const_iterator ci = typename ListType::iterator();
        (void)ci;
    }

    testlist.insert(i, values[0]);
    {  int init_values[] = { 5, 4, 1, 3 };
    TEST_INTRUSIVE_SEQUENCE(init_values, const_testlist.rbegin());  }

    i = testlist.iterator_to(values[4]);
    REQUIRE(&*i == &values[4]);

    i = ListType::s_iterator_to(values[4]);
    REQUIRE(&*i == &values[4]);

    typename ListType::const_iterator ic;
    ic = testlist.iterator_to(static_cast<typename ListType::const_reference>(values[4]));
    REQUIRE(&*ic == &values[4]);

    ic = ListType::s_iterator_to(static_cast<typename ListType::const_reference>(values[4]));
    REQUIRE(&*ic == &values[4]);

    i = testlist.erase(i);
    REQUIRE(i == testlist.end());

    {  int init_values[] = { 3, 1, 4 };
    TEST_INTRUSIVE_SEQUENCE(init_values, const_testlist.begin());  }
}

template < class ListType, typename ValueContainer >
void test_list<ListType, ValueContainer>
::test_shift(ValueContainer& values)
{
    ListType testlist;
    const int num_values = (int)values.size();
    std::vector<int> expected_values(num_values);

    for (int s = 1; s <= num_values; ++s) {
        expected_values.resize(s);
        //Shift forward all possible positions 3 times
        for (int i = 0; i < s * 3; ++i) {
            testlist.insert(testlist.begin(), values.begin(), values.begin() + s);
            testlist.shift_forward(i);
            for (int j = 0; j < s; ++j) {
                expected_values[(j + s - i % s) % s] = (j + 1);
            }
            TEST_INTRUSIVE_SEQUENCE_EXPECTED(expected_values, testlist.begin());
            testlist.clear();
        }

        //Shift backwards all possible positions
        for (int i = 0; i < s * 3; ++i) {
            testlist.insert(testlist.begin(), values.begin(), values.begin() + s);
            testlist.shift_backwards(i);
            for (int j = 0; j < s; ++j) {
                expected_values[(j + i) % s] = (j + 1);
            }
            TEST_INTRUSIVE_SEQUENCE_EXPECTED(expected_values, testlist.begin());
            testlist.clear();
        }
    }
}

//test: insert (seq-version), swap, splice, erase (seq-version):
template < class ListType, typename ValueContainer >
void test_list<ListType, ValueContainer>
::test_swap(ValueContainer& values)
{
    {
        ListType testlist1(values.begin(), values.begin() + 2);
        ListType testlist2;
        testlist2.insert(testlist2.end(), values.begin() + 2, values.begin() + 5);
        testlist1.swap(testlist2);

        {  int init_values[] = { 3, 4, 5 };
        TEST_INTRUSIVE_SEQUENCE(init_values, testlist1.begin());  }
        {  int init_values[] = { 1, 2 };
        TEST_INTRUSIVE_SEQUENCE(init_values, testlist2.begin());  }

        testlist2.splice(++testlist2.begin(), testlist1);
        {  int init_values[] = { 1, 3, 4, 5, 2 };
        TEST_INTRUSIVE_SEQUENCE(init_values, testlist2.begin());  }

        REQUIRE(testlist1.empty());

        testlist1.splice(testlist1.end(), testlist2, ++(++testlist2.begin()));
        {  int init_values[] = { 4 };
        TEST_INTRUSIVE_SEQUENCE(init_values, testlist1.begin());  }

        {  int init_values[] = { 1, 3, 5, 2 };
        TEST_INTRUSIVE_SEQUENCE(init_values, testlist2.begin());  }

        testlist1.splice(testlist1.end(), testlist2,
            testlist2.begin(), ----testlist2.end());
        {  int init_values[] = { 4, 1, 3 };
        TEST_INTRUSIVE_SEQUENCE(init_values, testlist1.begin());  }
        {  int init_values[] = { 5, 2 };
        TEST_INTRUSIVE_SEQUENCE(init_values, testlist2.begin());  }

        testlist1.erase(testlist1.iterator_to(values[0]), testlist1.end());
        REQUIRE(testlist1.size() == 1);
        REQUIRE(&testlist1.front() == &values[3]);
    }
    {
        ListType testlist1(values.begin(), values.begin() + 2);
        ListType testlist2(values.begin() + 3, values.begin() + 5);

        swap_nodes< node_algorithms >(values[0], values[2]);
        {  int init_values[] = { 3, 2 };
        TEST_INTRUSIVE_SEQUENCE(init_values, testlist1.begin());  }

        swap_nodes< node_algorithms >(values[2], values[4]);
        {  int init_values[] = { 5, 2 };
        TEST_INTRUSIVE_SEQUENCE(init_values, testlist1.begin());  }
        {  int init_values[] = { 4, 3 };
        TEST_INTRUSIVE_SEQUENCE(init_values, testlist2.begin());  }
    }
    {
        ListType testlist1(values.begin(), values.begin() + 1);

        {  int init_values[] = { 1 };
        TEST_INTRUSIVE_SEQUENCE(init_values, testlist1.begin());  }

        swap_nodes< node_algorithms >(values[1], values[2]);

        {  int init_values[] = { 1 };
        TEST_INTRUSIVE_SEQUENCE(init_values, testlist1.begin());  }

        swap_nodes< node_algorithms >(values[0], values[2]);

        {  int init_values[] = { 3 };
        TEST_INTRUSIVE_SEQUENCE(init_values, testlist1.begin());  }

        swap_nodes< node_algorithms >(values[0], values[2]);

        {  int init_values[] = { 1 };
        TEST_INTRUSIVE_SEQUENCE(init_values, testlist1.begin());  }
    }
}

template < class ListType, typename ValueContainer >
void test_list<ListType, ValueContainer>
::test_container_from_end(ValueContainer& values)
{
    ListType testlist1(values.begin(), values.begin() + values.size());
    REQUIRE(testlist1 == ListType::container_from_end_iterator(testlist1.end()));
    REQUIRE(testlist1 == ListType::container_from_end_iterator(testlist1.cend()));
}

template < class ListType, typename ValueContainer >
void test_list<ListType, ValueContainer>
::test_clone(ValueContainer& values)
{
    ListType testlist1(values.begin(), values.begin() + values.size());
    ListType testlist2;

    testlist2.clone_from(testlist1, test::new_cloner<value_type>(), test::delete_disposer<value_type>());
    REQUIRE(testlist2 == testlist1);
    testlist2.clear_and_dispose(test::delete_disposer<value_type>());
    REQUIRE(testlist2.empty());
}

template < typename ValueTraits, bool ConstantTimeSize, bool Default_Holder, typename ValueContainer >
struct make_and_test_list
    : test_list< list< typename ValueTraits::value_type,
    value_traits< ValueTraits >,
    constant_time_size< ConstantTimeSize >
    >,
    ValueContainer
    >
{};

template < typename ValueTraits, bool ConstantTimeSize, typename ValueContainer >
struct make_and_test_list< ValueTraits, ConstantTimeSize, false, ValueContainer >
    : test_list< list< typename ValueTraits::value_type,
    value_traits< ValueTraits >,
    constant_time_size< ConstantTimeSize >,
    header_holder_type< heap_node_holder< typename ValueTraits::pointer > >
    >,
    ValueContainer
    >
{};


template < class VoidPointer, bool ConstantTimeSize, bool Default_Holder >
class test_main_template
{
public:
    int operator()()
    {
        typedef testvalue< hooks<VoidPointer> > value_type;
        std::vector<value_type> data(5);
        for (int i = 0; i < 5; ++i)
            data[i].value_ = i + 1;

        make_and_test_list <detail::get_base_value_traits_t<
            value_type,
            typename hooks<VoidPointer>::base_hook_type
        >,
            ConstantTimeSize,
            Default_Holder,
            std::vector< value_type >
        >::test_all(data);
        make_and_test_list < detail::get_member_value_traits_t <
            member_hook< value_type, typename hooks<VoidPointer>::member_hook_type, &value_type::node_>
        >,
            ConstantTimeSize,
            Default_Holder,
            std::vector< value_type >
        >::test_all(data);
        make_and_test_list< nonhook_node_member_value_traits <
            value_type,
            typename hooks<VoidPointer>::nonhook_node_member_type,
            &value_type::nhn_member_,
            link_mode_type::safe_link
        >,
            ConstantTimeSize,
            Default_Holder,
            std::vector< value_type >
        >::test_all(data);

        return 0;
    }
};

template < class VoidPointer, bool Default_Holder >
class test_main_template< VoidPointer, false, Default_Holder >
{
public:
    int operator()()
    {
        using value_type = testvalue<hooks<VoidPointer>>;
        std::vector<value_type> data(5);
        for (int i = 0; i < 5; ++i)
            data[i].value_ = i + 1;

        make_and_test_list <detail::get_base_value_traits_t<
            value_type,
            typename hooks<VoidPointer>::auto_base_hook_type
        >,
            false,
            Default_Holder,
            std::vector< value_type >
        >::test_all(data);
        make_and_test_list < detail::get_member_value_traits_t<
            member_hook< value_type, typename hooks<VoidPointer>::auto_member_hook_type, &value_type::auto_node_>
        >,
            false,
            Default_Holder,
            std::vector< value_type >
        >::test_all(data);

        return 0;
    }
};

template < bool ConstantTimeSize >
struct test_main_template_bptr
{
    int operator()()
    {
        typedef BPtr_Value value_type;
        typedef BPtr_Value_Traits< List_BPtr_Node_Traits > list_value_traits;
        typedef typename list_value_traits::node_ptr node_ptr;
        typedef bounded_allocator< value_type > allocator_type;

        bounded_allocator_scope<allocator_type> bounded_scope; (void)bounded_scope;

        allocator_type allocator;

        {
            bounded_reference_cont< value_type > ref_cont;
            for (int i = 0; i < 5; ++i)
            {
                node_ptr tmp = allocator.allocate(1);
                new (tmp.raw()) value_type(i + 1);
                ref_cont.push_back(*tmp);
            }

            using list_type = list<value_type,
                value_traits<list_value_traits>,
                constant_time_size<ConstantTimeSize>,
                header_holder_type<bounded_pointer_holder<value_type>>>;
            test_list < list_type, bounded_reference_cont<value_type>>::test_all(ref_cont);
        }

        return 0;
    }
};

TEST_CASE("list_test void pointer 1", "list_test") {
    test_main_template<void*, false, true>()();
}

TEST_CASE("list_test smart_ptr 1", "list_test") {
    test_main_template<boost::intrusive::smart_ptr<void>, false, true>()();
}

TEST_CASE("list_test void pointer 2", "list_test") {
    test_main_template<void*, true, true>()();
}

TEST_CASE("list_test smart_ptr 2", "list_test") {
    test_main_template<boost::intrusive::smart_ptr<void>, true, true>()();
}

#if 0
TEST_CASE("list_test bounded_ptr 1", "list_test") {
    test_main_template_bptr< true >()();
}

TEST_CASE("list_test bounded_ptr 2", "list_test") {
    test_main_template_bptr< false >()();
}
#endif
