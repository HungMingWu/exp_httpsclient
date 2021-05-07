#include "catch.hpp"
#include <boost/intrusive/sg_set.hpp>
#include "itestvalue.hpp"
#include "bptr_value.hpp"
#include "smart_ptr.hpp"
#include "bs_test_common.hpp"
#include "generic_multiset_test.hpp"

using namespace boost::intrusive;

template < class ValueTraits, bool DefaultHolder, bool Map >
struct rebinder
{
    typedef tree_rebinder_common<ValueTraits, DefaultHolder, Map> common_t;
    typedef typename ValueContainer< typename ValueTraits::value_type >::type value_cont_type;

    template < class Option1 = void
        , class Option2 = void
    >
        struct container
    {
        typedef sg_multiset
            < typename common_t::value_type
            , value_traits<ValueTraits>
            , typename common_t::holder_opt
            , typename common_t::key_of_value_opt
            , Option1
            , Option2
            > type;
        static_assert(key_type_tester<typename common_t::key_of_value_opt, type>::value);
    };
};

enum HookType
{
    Base,
    Member,
    NonMember
};

template<class VoidPointer, bool DefaultHolder, bool Map, HookType Type>
class test_main_template;

template<class VoidPointer, bool DefaultHolder, bool Map>
class test_main_template<VoidPointer, DefaultHolder, Map, Base>
{
public:
    static void execute()
    {
        typedef testvalue_traits< bs_hooks<VoidPointer> > testval_traits_t;
        //base
        typedef typename testval_traits_t::base_value_traits  base_hook_t;
        test::test_generic_multiset
            < rebinder<base_hook_t, DefaultHolder, Map>
            >::test_all();
    }
};

template<class VoidPointer, bool DefaultHolder, bool Map>
class test_main_template<VoidPointer, DefaultHolder, Map, Member>
{
public:
    static void execute()
    {
        typedef testvalue_traits< bs_hooks<VoidPointer> > testval_traits_t;
        //member
        typedef typename testval_traits_t::member_value_traits member_hook_t;
        test::test_generic_multiset
            < rebinder<member_hook_t, DefaultHolder, Map>
            >::test_all();
    }
};

template<class VoidPointer, bool DefaultHolder, bool Map>
class test_main_template<VoidPointer, DefaultHolder, Map, NonMember>
{
public:
    static void execute()
    {
        typedef testvalue_traits< bs_hooks<VoidPointer> > testval_traits_t;
        //nonmember
        test::test_generic_multiset
            < rebinder<typename testval_traits_t::nonhook_value_traits, DefaultHolder, Map>
            >::test_all();
    }
};


template < bool FloatingPoint, bool Map >
struct test_main_template_bptr
{
    static void execute()
    {
        typedef BPtr_Value_Traits< Tree_BPtr_Node_Traits > value_traits;
        typedef bounded_allocator< BPtr_Value >            allocator_type;

        bounded_allocator_scope<allocator_type> bounded_scope; (void)bounded_scope;
        test::test_generic_multiset
            < rebinder< value_traits, true, Map>
            >::test_all();
    }
};

TEST_CASE("sg_multiset_test void pointer 1", "sg_multiset_test") {
    test_main_template<void*, false, false, Base>::execute();
}

TEST_CASE("sg_multiset_test void pointer 2", "sg_multiset_test") {
    test_main_template<void*, true, false, Member>::execute();
}

TEST_CASE("sg_multiset_test void pointer 3", "sg_multiset_test") {
    test_main_template<void*, false, false, Base>::execute();
}

TEST_CASE("sg_multiset_test void pointer 4", "sg_multiset_test") {
    test_main_template<void*, true, false, Member>::execute();
}

TEST_CASE("sg_multiset_test void pointer 5", "sg_multiset_test") {
    test_main_template<void*, true, true, NonMember>::execute();
}

TEST_CASE("sg_multiset_test smart_ptr 1", "sg_multiset_test") {
    test_main_template<smart_ptr<void>, false, true, Base>::execute();
}

TEST_CASE("sg_multiset_test smart_ptr 2", "sg_multiset_test") {
    test_main_template<smart_ptr<void>, true, true, Member>::execute();
}

TEST_CASE("sg_multiset_test smart_ptr 3", "sg_multiset_test") {
    test_main_template<smart_ptr<void>, false, true, NonMember>::execute();
}

TEST_CASE("sg_multiset_test bounded_ptr 1", "sg_multiset_test") {
    test_main_template_bptr< false, false >::execute();
}

TEST_CASE("sg_multiset_test bounded_ptr 2", "sg_multiset_test") {
    test_main_template_bptr<  true, true >::execute();
}