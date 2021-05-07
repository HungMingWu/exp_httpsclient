#include "catch.hpp"
#include <boost/intrusive/unordered_set.hpp>
#include <boost/intrusive/detail/mpl.hpp>
#include <vector>

using namespace boost::intrusive;

class MyClass1 : public unordered_set_base_hook<>
{
public:
    int int_;
    MyClass1(int i = 0) : int_(i)
    {}
    unordered_set_member_hook<> member_hook_;

    friend bool operator==(const MyClass1& l, const MyClass1& r)
    {
        return l.int_ == r.int_;
    }
};

namespace std
{
    template<>
    struct hash<MyClass1>
    {
        size_t operator()(MyClass1 const& o) const noexcept
        {
            return std::hash<int>()(o.int_);
        }
    };
}

struct uset_value_traits
{
    typedef slist_node_traits<void*>          node_traits;
    typedef node_traits::node_ptr             node_ptr;
    typedef node_traits::const_node_ptr       const_node_ptr;
    typedef MyClass1                           value_type;
    typedef MyClass1* pointer;
    typedef const MyClass1* const_pointer;
    static const link_mode_type link_mode = link_mode_type::normal_link;

    static node_ptr to_node_ptr(value_type& value)
    {
        return node_ptr(&value);
    }
    static const_node_ptr to_node_ptr(const value_type& value)
    {
        return const_node_ptr(&value);
    }
    static pointer to_value_ptr(node_ptr n)
    {
        return static_cast<value_type*>(n);
    }
    static const_pointer to_value_ptr(const_node_ptr n)
    {
        return static_cast<const value_type*>(n);
    }
};

//Base
typedef base_hook< unordered_set_base_hook<> >  BaseHook;
typedef unordered_bucket<BaseHook>::type        BaseBucketType;
typedef unordered_bucket_ptr<BaseHook>::type    BaseBucketPtrType;
typedef unordered_set<MyClass1, BaseHook>        BaseUset;
//Member
typedef member_hook
< MyClass1, unordered_set_member_hook<>
    , &MyClass1::member_hook_ >                   MemberHook;
typedef unordered_bucket<MemberHook>::type      MemberBucketType;
typedef unordered_bucket_ptr<MemberHook>::type  MemberBucketPtrType;
typedef unordered_set<MyClass1, MemberHook>      MemberUset;
//Explicit
typedef value_traits< uset_value_traits >       Traits;
typedef unordered_bucket<Traits>::type          TraitsBucketType;
typedef unordered_bucket_ptr<Traits>::type      TraitsBucketPtrType;
typedef unordered_set<MyClass1, Traits>          TraitsUset;

struct uset_bucket_traits
{
    //Power of two bucket length
    static const std::size_t NumBuckets = 128;

    uset_bucket_traits(BaseBucketType* buckets)
        : buckets_(buckets)
    {}

    uset_bucket_traits(const uset_bucket_traits& other)
        : buckets_(other.buckets_)
    {}

    BaseBucketType* bucket_begin() const
    {
        return buckets_;
    }

    std::size_t bucket_count() const
    {
        return NumBuckets;
    }

    BaseBucketType* buckets_;
};

typedef unordered_set
<MyClass1, bucket_traits<uset_bucket_traits>, power_2_buckets<true> >
BucketTraitsUset;

static_assert(std::is_same<BaseUset::bucket_type, BaseBucketType>::value);
static_assert(std::is_same<MemberUset::bucket_type, MemberBucketType>::value);
static_assert(std::is_same<TraitsUset::bucket_type, TraitsBucketType>::value);
static_assert(std::is_same<BaseBucketType, MemberBucketType>::value);
static_assert(std::is_same<BaseBucketType, TraitsBucketType>::value);
static_assert(std::is_same<BaseBucketPtrType, TraitsBucketPtrType>::value);
static_assert(std::is_same<BaseBucketPtrType, MemberBucketPtrType>::value);
static_assert(std::is_same<BaseBucketPtrType, BaseBucketType*>::value);

TEST_CASE("custom_bucket_traits_test", "custom_bucket_traits_test") {
    typedef std::vector<MyClass1>::iterator VectIt;
    typedef std::vector<MyClass1>::reverse_iterator VectRit;
    std::vector<MyClass1> values;

    for (int i = 0; i < 100; ++i)  values.push_back(MyClass1(i));

    BaseBucketType buckets[uset_bucket_traits::NumBuckets];
    uset_bucket_traits btraits(buckets);
    BucketTraitsUset uset(btraits);

    for (VectIt it(values.begin()), itend(values.end()); it != itend; ++it)
        uset.insert(*it);

    for (VectRit it(values.rbegin()), itend(values.rend()); it != itend; ++it) {
        REQUIRE(uset.find(*it) != uset.cend());
    }
}