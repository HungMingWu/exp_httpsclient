#include "catch.hpp"
#include <boost/intrusive/list.hpp>
#include <boost/intrusive/slist.hpp>
#include <boost/intrusive/set.hpp>
#include <boost/intrusive/unordered_set.hpp>
#include <boost/intrusive/avl_set.hpp>
#include <boost/intrusive/sg_set.hpp>
#include <boost/intrusive/splay_set.hpp>
#include <boost/intrusive/bs_set.hpp>
#include <boost/intrusive/treap_set.hpp>
#include <boost/intrusive/detail/mpl.hpp>
#include <boost/intrusive/pointer_traits.hpp>
#include "smart_ptr.hpp"
#include <vector>

using namespace boost::intrusive;

struct my_tag;
struct my_tag2;
struct my_tag3;

using void_smart_ptr = void_pointer<smart_ptr<void> >;
using normal_link_mod = link_mode<link_mode_type::normal_link>;

using TreapHook = make_bs_set_base_hook_t<void_smart_ptr, normal_link_mod, tag<my_tag>>;
using SplayHook = make_bs_set_base_hook_t<void_smart_ptr, normal_link_mod, tag<my_tag2>>;
using BsHook = make_bs_set_base_hook_t<void_smart_ptr, normal_link_mod, tag<my_tag3>>;

class MyClass4
    : public make_list_base_hook_t<void_smart_ptr, normal_link_mod>
    , public make_slist_base_hook_t<void_smart_ptr, normal_link_mod>
    , public make_set_base_hook_t<void_smart_ptr, normal_link_mod>
    , public make_unordered_set_base_hook_t<void_smart_ptr, normal_link_mod>
    , public make_avl_set_base_hook_t<void_smart_ptr, normal_link_mod>
    , public make_bs_set_base_hook_t<void_smart_ptr, normal_link_mod>
    , public TreapHook
    , public SplayHook
    , public BsHook
{
public:
    int int_;
    MyClass4(int i)
        : int_(i)
    {}

    friend bool operator<(const MyClass4& l, const MyClass4& r)
    {
        return l.int_ < r.int_;
    }

    friend bool operator==(const MyClass4& l, const MyClass4& r)
    {
        return l.int_ == r.int_;
    }

    friend bool priority_order(const MyClass4& l, const MyClass4& r)
    {
        return l.int_ < r.int_;
    }
};

namespace std
{
    template<>
    struct hash<MyClass4>
    {
        size_t operator()(MyClass4 const& o) const noexcept
        {
            return std::hash<int>()(o.int_);
        }
    };
}

//Define a list that will store MyClass4 using the public base hook
typedef make_list<MyClass4>::type          List;
typedef make_slist<MyClass4>::type         Slist;
typedef make_set<MyClass4>::type           Set;
typedef make_unordered_set<MyClass4>::type USet;

typedef make_avl_set<MyClass4>::type       AvlSet;
typedef make_sg_set<MyClass4>::type        SgSet;
typedef make_treap_set<MyClass4
    , base_hook<TreapHook> >::type         TreapSet;
typedef make_splay_set<MyClass4
    , base_hook<SplayHook> >::type         SplaySet;
typedef make_bs_set<MyClass4
    , base_hook<BsHook> >::type            BsSet;

//Check defined types and implicitly defined types are equal
using void_ptr_t = void_pointer<void*>;
using safe_link_mode = link_mode<link_mode_type::safe_link>;
using normal_link_mode = link_mode<link_mode_type::normal_link>;

static_assert(std::is_same_v<make_list_base_hook_t<void_ptr_t, safe_link_mode>,
                             make_list_base_hook_t<>>);

static_assert(std::is_same_v<make_slist_base_hook_t<void_ptr_t, safe_link_mode>,
                             make_slist_base_hook_t<>>);

static_assert(std::is_same_v<make_set_base_hook_t<void_ptr_t, safe_link_mode>,
                             make_set_base_hook_t<>>);

static_assert(std::is_same_v<make_unordered_set_base_hook_t<void_ptr_t, safe_link_mode>,
                             make_unordered_set_base_hook_t<>>);

static_assert(std::is_same_v<make_avl_set_base_hook_t<void_ptr_t, safe_link_mode>,
                             make_avl_set_base_hook_t<>>);

static_assert(std::is_same_v<make_bs_set_base_hook_t<void_ptr_t, safe_link_mode>,
                             make_bs_set_base_hook_t<>>);

//Check defined types and implicitly defined types are unequal
static_assert(!std::is_same_v<make_list_base_hook_t<void_ptr_t, normal_link_mode>,
                              make_list_base_hook_t<>>);

static_assert(!std::is_same_v<make_slist_base_hook_t<void_ptr_t, normal_link_mode>,
                              make_slist_base_hook_t<>>);

static_assert(!std::is_same_v<make_set_base_hook_t<void_ptr_t, normal_link_mode>,
                              make_set_base_hook_t<>>);

static_assert(!std::is_same_v<make_unordered_set_base_hook_t<void_ptr_t, normal_link_mode>,
                              make_unordered_set_base_hook_t<>>);

static_assert(!std::is_same_v<make_avl_set_base_hook_t<void_ptr_t, normal_link_mode>,
                              make_avl_set_base_hook_t<>>);

static_assert(!std::is_same_v<make_bs_set_base_hook_t<void_ptr_t, normal_link_mode>, 
                              make_bs_set_base_hook_t<>
>);

TEST_CASE("make_functions_test", "make_functions_test") {
    typedef std::vector<MyClass4>::iterator VectIt;
    typedef std::vector<MyClass4>::reverse_iterator VectRit;

    //Create several MyClass4 objects, each one with a different value
    std::vector<MyClass4> values;
    for (int i = 0; i < 100; ++i)  values.push_back(MyClass4(i));

    USet::bucket_type buckets[100];

    List  my_list;
    Slist my_slist;
    Set   my_set;
    USet  my_uset(USet::bucket_traits
    (pointer_traits<USet::bucket_ptr>::pointer_to(buckets[0]), 100));

    AvlSet      my_avlset;
    SplaySet    my_splayset;
    BsSet       my_bsset;
    SgSet       my_sgset;
    TreapSet    my_treapset;

    //Now insert them in containers
    for (VectIt it(values.begin()), itend(values.end()); it != itend; ++it) {
        my_list.push_front(*it);
        my_slist.push_front(*it);
        my_set.insert(*it);
        my_uset.insert(*it);
        my_avlset.insert(*it);
        my_splayset.insert(*it);
        my_bsset.insert(*it);
        my_sgset.insert(*it);
        my_treapset.insert(*it);
    }

    //Now test lists
    {
        List::const_iterator  list_it(my_list.cbegin());
        Slist::const_iterator slist_it(my_slist.cbegin());
        Set::const_reverse_iterator set_rit(my_set.crbegin());

        AvlSet::const_reverse_iterator avlset_rit(my_avlset.crbegin());
        SplaySet::const_reverse_iterator splayset_rit(my_splayset.crbegin());
        BsSet::const_reverse_iterator bsset_rit(my_bsset.crbegin());
        SgSet::const_reverse_iterator sgset_rit(my_sgset.crbegin());
        TreapSet::const_reverse_iterator treapset_rit(my_treapset.crbegin());

        VectRit vect_it(values.rbegin()), vect_itend(values.rend());

        //Test the objects inserted in the base hook list
        for (; vect_it != vect_itend
            ; ++vect_it, ++list_it, ++slist_it, ++set_rit
            , ++avlset_rit, ++splayset_rit, ++bsset_rit, ++sgset_rit, ++treapset_rit
            ) {
            REQUIRE(&*list_it == &*vect_it);
            REQUIRE(&*slist_it == &*vect_it);
            REQUIRE(&*set_rit == &*vect_it);
            REQUIRE(my_uset.find(*set_rit) != my_uset.cend());
            REQUIRE(&*avlset_rit == &*vect_it);
            REQUIRE(&*splayset_rit == &*vect_it);
            REQUIRE(&*bsset_rit == &*vect_it);
            REQUIRE(&*sgset_rit == &*vect_it);
            REQUIRE(&*treapset_rit == &*vect_it);
        }
    }
}