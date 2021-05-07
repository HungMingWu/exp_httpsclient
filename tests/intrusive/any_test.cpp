#include "catch.hpp"
#include <boost/intrusive/any_hook.hpp>
#include <boost/intrusive/slist.hpp>
#include <boost/intrusive/rbtree.hpp>
#include <boost/intrusive/list.hpp>
#include <boost/intrusive/avltree.hpp>
#include <boost/intrusive/sgtree.hpp>
#include <boost/intrusive/splaytree.hpp>
#include <boost/intrusive/treap.hpp>
#include <boost/intrusive/hashtable.hpp>
#include <vector>    //std::vector

using namespace boost::intrusive;

class MyClass : public any_base_hook<>
{
    int int_;

public:
    //This is a member hook
    any_member_hook<> member_hook_;

    MyClass(int i = 0)
        : int_(i)
    {}

    int get() const
    {
        return this->int_;
    }

    friend bool operator  < (const MyClass& l, const MyClass& r)
    {
        return l.int_ < r.int_;
    }

    friend bool operator == (const MyClass& l, const MyClass& r)
    {
        return l.int_ == r.int_;
    }

    friend bool priority_order(const MyClass& a, const MyClass& b)
    {
        return a.int_ < b.int_;
    }
};

namespace std
{
    template<>
    struct hash<MyClass>
    {
        size_t operator()(MyClass const& o) const noexcept
        {
            return std::hash<int>()(o.get());
        }
    };
}

TEST_CASE("simple_slist_test", "any_hook") {
    //Define an slist that will store MyClass using the public base hook
    using BaseOption = any_to_slist_hook<base_hook<any_base_hook<>>>;
    using BaseList = slist<MyClass, BaseOption, constant_time_size<false>>;

    //Define an slist that will store MyClass using the public member hook
    using MemberOption = any_to_slist_hook<member_hook<MyClass, any_member_hook<>, &MyClass::member_hook_>>;
    using MemberList = slist<MyClass, MemberOption>;

    //Create several MyClass objects, each one with a different value
    std::vector<MyClass> values;
    for (int i = 0; i < 100; ++i)  values.push_back(MyClass(i));

    BaseList baselist;
    MemberList memberlist;

    //Now insert them in the reverse order in the base hook list
    for (auto it(values.begin()), itend(values.end()); it != itend; ++it)
        baselist.push_front(*it);

    //Now insert them in the same order as in vector in the member hook list
    for (BaseList::iterator it(baselist.begin()), itend(baselist.end())
        ; it != itend; ++it) {
        memberlist.push_front(*it);
    }

    //Now test lists
    {
        BaseList::iterator bit(baselist.begin());
        MemberList::iterator mit(memberlist.begin());
        auto rit(values.rbegin()), ritend(values.rend());
        auto  it(values.begin()), itend(values.end());

        //Test the objects inserted in the base hook list
        for (; rit != ritend; ++rit, ++bit)
            REQUIRE(&*bit == &*rit);

        //Test the objects inserted in the member hook list
        for (; it != itend; ++it, ++mit)
            REQUIRE(&*mit == &*it);
    }
}

TEST_CASE("instantiation_test", "any_hook") {
    using MemberHook = member_hook<MyClass, any_member_hook<>, &MyClass::member_hook_>;
    using BaseHook = base_hook<any_base_hook<>>;

    MyClass myclass;
    {
        slist<MyClass, any_to_slist_hook<BaseHook>> slist_base;
        slist_base.push_front(myclass);
    }
    {
        slist<MyClass, any_to_slist_hook<MemberHook>> slist_member;
        slist_member.push_front(myclass);
    }
    {
        list<MyClass, any_to_list_hook<BaseHook>> list_base;
        list_base.push_front(myclass);
    }
    {
        list<MyClass, any_to_list_hook<MemberHook>> list_member;
        list_member.push_front(myclass);
    }
    {
        rbtree<MyClass, any_to_set_hook<BaseHook>>  rbtree_base;
        rbtree_base.insert_unique(myclass);
    }
    {
        rbtree<MyClass, any_to_set_hook<MemberHook>> rbtree_member;
        rbtree_member.insert_unique(myclass);
    }
    {
        avltree<MyClass, any_to_avl_set_hook<BaseHook>> avltree_base;
        avltree_base.insert_unique(myclass);
    }
    {
        avltree<MyClass, any_to_avl_set_hook<MemberHook>> avltree_member;
        avltree_member.insert_unique(myclass);
    }
    {
        sgtree<MyClass, any_to_bs_set_hook<BaseHook>> sgtree_base;
        sgtree_base.insert_unique(myclass);
    }
    {
        sgtree<MyClass, any_to_bs_set_hook<MemberHook>> sgtree_member;
        sgtree_member.insert_unique(myclass);
    }
    {
        treap<MyClass, any_to_bs_set_hook<BaseHook>> treap_base;
        treap_base.insert_unique(myclass);
    }
    {
        treap<MyClass, any_to_bs_set_hook<MemberHook>> treap_member;
        treap_member.insert_unique(myclass);
    }
    {
        splaytree<MyClass, any_to_bs_set_hook<BaseHook>> splaytree_base;
        splaytree_base.insert_unique(myclass);
    }
    {
        splaytree<MyClass, any_to_bs_set_hook<MemberHook>> splaytree_member;
        splaytree_member.insert_unique(myclass);
    }
    using bucket_type = unordered_bucket<any_to_unordered_set_hook<BaseHook>>::type;
    using bucket_traits = unordered_default_bucket_traits<any_to_unordered_set_hook<BaseHook>>::type;
    bucket_type buckets[2];
    {
        hashtable<MyClass, any_to_unordered_set_hook<BaseHook>>
            hashtable_base(bucket_traits(&buckets[0], 1));
        hashtable_base.insert_unique(myclass);
    }
    {
        hashtable<MyClass, any_to_unordered_set_hook<MemberHook>>
            hashtable_member(bucket_traits(&buckets[1], 1));
        hashtable_member.insert_unique(myclass);
    }
}
