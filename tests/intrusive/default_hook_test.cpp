#include "catch.hpp"
#include <boost/intrusive/list.hpp>
#include <boost/intrusive/slist.hpp>
#include <boost/intrusive/set.hpp>
#include <boost/intrusive/unordered_set.hpp>
#include <boost/intrusive/splay_set.hpp>
#include <boost/intrusive/avl_set.hpp>
#include <boost/intrusive/sg_set.hpp>
#include <boost/intrusive/treap_set.hpp>
#include <boost/intrusive/bs_set.hpp>
#include <boost/intrusive/pointer_traits.hpp>
#include "smart_ptr.hpp"
#include <vector>

using namespace boost::intrusive;

using safe_link_mode = link_mode<link_mode_type::safe_link>;
using void_smart_ptr_t = void_pointer<smart_ptr<void>>;
class MyClass2 : 
    public list_base_hook<void_smart_ptr_t, safe_link_mode>, 
    public slist_base_hook<void_smart_ptr_t, safe_link_mode>,
    public set_base_hook<void_smart_ptr_t, safe_link_mode>,
    public unordered_set_base_hook<void_smart_ptr_t, safe_link_mode>,
    public avl_set_base_hook<void_smart_ptr_t, safe_link_mode>,
    public bs_set_base_hook<void_smart_ptr_t, safe_link_mode>
{
public:
    int int_;
    MyClass2(int i)
        : int_(i)
    {}

    friend bool operator<(const MyClass2& l, const MyClass2& r)
    {
        return l.int_ < r.int_;
    }

    friend bool operator==(const MyClass2& l, const MyClass2& r)
    {
        return l.int_ == r.int_;
    }

    friend bool priority_order(const MyClass2& l, const MyClass2& r)
    {
        return l.int_ < r.int_;
    }
};

namespace std
{
    template<>
    struct hash<MyClass2>
    {
        size_t operator()(MyClass2 const& o) const noexcept
        {
            return std::hash<int>()(o.int_);
        }
    };
}

//Define a list that will store MyClass2 using the public base hook
using List = list<MyClass2>;
using Slist = slist<MyClass2>;
using Set = set<MyClass2>;
using USet = unordered_set<MyClass2>;
using AvlSet = avl_set<MyClass2>;
using SplaySet = splay_set<MyClass2>;
using TreapSet = treap_set<MyClass2>;
using SgSet = sg_set<MyClass2>;
using BsSet = bs_set<MyClass2>;

TEST_CASE("default_hook_test test_size", "default_hook_test") {
    //Create several MyClass2 objects, each one with a different value
    std::vector<MyClass2> values;
    for (int i = 0; i < 100; ++i)  values.push_back(MyClass2(i));

    {
        List  my_list;
        Slist my_slist;
        Set   my_set;
        USet::bucket_type buckets[100];
        USet  my_uset(USet::bucket_traits(pointer_traits<USet::bucket_ptr>::pointer_to(*buckets), 100));
        AvlSet   my_avlset;
        SplaySet my_splayset;

        //Now insert them in the reverse order
        //in the base hook intrusive list
        for (auto it(values.begin()), itend(values.end()); it != itend; ++it) {
            my_list.push_front(*it);
            my_slist.push_front(*it);
            my_set.insert(*it);
            my_uset.insert(*it);
            my_avlset.insert(*it);
            my_splayset.insert(*it);
        }

        //Now test lists
        {
            auto list_it(my_list.cbegin());
            auto slist_it(my_slist.cbegin());
            auto set_rit(my_set.crbegin());
            auto avl_set_rit(my_avlset.crbegin());
            auto splay_set_rit(my_splayset.crbegin());

            auto vect_it(values.rbegin()), vect_itend(values.rend());

            //Test the objects inserted in the base hook list
            for (; vect_it != vect_itend
                ; ++vect_it, ++list_it
                , ++slist_it, ++set_rit
                , ++avl_set_rit
                , ++splay_set_rit
                ) {
                REQUIRE(&*list_it == &*vect_it);
                REQUIRE(&*slist_it == &*vect_it);
                REQUIRE(&*set_rit == &*vect_it);
                REQUIRE(&*avl_set_rit == &*vect_it);
                REQUIRE(&*splay_set_rit == &*vect_it);
                REQUIRE(my_uset.find(*set_rit) != my_uset.cend());
            }
        }
    }
    //Since treap_set, sg_set & bs_set reuse the hook, treat them apart
    {
        TreapSet my_treapset;
        for (auto it(values.begin()), itend(values.end()); it != itend; ++it) {
            my_treapset.insert(*it);
        }

        auto treap_set_rit(my_treapset.crbegin());
        auto vect_it(values.rbegin()), vect_itend(values.rend());
        for (; vect_it != vect_itend; ++vect_it, ++treap_set_rit) {
            REQUIRE(&*treap_set_rit == &*vect_it);
        }
    }
    {
        SgSet    my_sgset;
        for (auto it(values.begin()), itend(values.end()); it != itend; ++it) {
            my_sgset.insert(*it);
        }

        auto sg_set_rit(my_sgset.crbegin());
        auto vect_it(values.rbegin()), vect_itend(values.rend());
        for (; vect_it != vect_itend; ++vect_it, ++sg_set_rit) {
            REQUIRE(&*sg_set_rit == &*vect_it);
        }
    }
    {
        BsSet    my_bsset;
        for (auto it(values.begin()), itend(values.end()); it != itend; ++it) {
            my_bsset.insert(*it);
        }

        auto bs_set_rit(my_bsset.crbegin());
        auto vect_it(values.rbegin()), vect_itend(values.rend());
        for (; vect_it != vect_itend; ++vect_it, ++bs_set_rit) {
            REQUIRE(&*bs_set_rit == &*vect_it);
        }
    }
}