#include "catch.hpp"
#include <boost/intrusive/parent_from_member.hpp>
#include <boost/intrusive/list.hpp>
#include <boost/intrusive/slist.hpp>
#include <boost/intrusive/set.hpp>
#include <boost/intrusive/unordered_set.hpp>

using namespace boost::intrusive;

struct MyClass3
{
    MyClass3() : order(0) {}
    int order;

    //This internal type has two hooks
    struct InnerNode : public list_base_hook<>, public slist_base_hook<>
        , public set_base_hook<>, public unordered_set_base_hook<>
    {
        list_member_hook<>            listhook;
        slist_member_hook<>           slisthook;
        set_member_hook<>             sethook;
        unordered_set_member_hook<>   usethook;
    } inner;

    friend bool operator < (const MyClass3& l, const MyClass3& r)
    {
        return l.order < r.order;
    }
    friend bool operator == (const MyClass3& l, const MyClass3& r)
    {
        return l.order == r.order;
    }
};

namespace std
{
    template<>
    struct hash<MyClass3>
    {
        size_t operator()(MyClass3 const& o) const noexcept
        {
            return std::hash<int>()(o.order);
        }
    };
}

//This functor converts between MyClass3 and the InnerNode member hook
#define InnerMemberHook(TAG, HOOKTYPE, MEMBERNAME)\
   struct InnerMemberHookFunctor##TAG \
   {\
      typedef HOOKTYPE              hook_type;\
      typedef hook_type*            hook_ptr;\
      typedef const hook_type*      const_hook_ptr;\
      typedef MyClass3               value_type;\
      typedef value_type*           pointer;\
      typedef const value_type*     const_pointer;\
                                                \
      static hook_ptr to_hook_ptr (value_type &value)\
         {  return &value.inner.MEMBERNAME; }\
      static const_hook_ptr to_hook_ptr(const value_type &value)\
         {  return &value.inner.MEMBERNAME; }\
      static pointer to_value_ptr(hook_ptr n)\
      {\
         return get_parent_from_member<MyClass3>\
            (get_parent_from_member<MyClass3::InnerNode>(n, &MyClass3::InnerNode::MEMBERNAME)\
            ,&MyClass3::inner\
         );\
      }\
      static const_pointer to_value_ptr(const_hook_ptr n)\
      {\
         return get_parent_from_member<MyClass3>\
            (get_parent_from_member<MyClass3::InnerNode>(n, &MyClass3::InnerNode::MEMBERNAME)\
            ,&MyClass3::inner\
         );\
      }\
   };\
//


//This functor converts between MyClass3 and the InnerNode base hook
#define InnerBaseHook(TAG, HOOKTYPE)\
   struct InnerBaseHookFunctor##TAG \
   {\
      typedef HOOKTYPE              hook_type;\
      typedef hook_type*            hook_ptr;\
      typedef const hook_type*      const_hook_ptr;\
      typedef MyClass3               value_type;\
      typedef value_type*           pointer;\
      typedef const value_type*     const_pointer;\
                                                \
      static hook_ptr to_hook_ptr (value_type &value)\
         {  return &value.inner; }\
      static const_hook_ptr to_hook_ptr(const value_type &value)\
         {  return &value.inner; }\
      static pointer to_value_ptr(hook_ptr n)\
      {\
         return get_parent_from_member<MyClass3>(static_cast<MyClass3::InnerNode*>(n),&MyClass3::inner);\
      }\
      static const_pointer to_value_ptr(const_hook_ptr n)\
      {\
         return get_parent_from_member<MyClass3>(static_cast<const MyClass3::InnerNode*>(n),&MyClass3::inner);\
      }\
   };\
//

//List
InnerMemberHook(List, list_member_hook<>, listhook)
InnerBaseHook(List, list_base_hook<>)
//Slist
InnerMemberHook(Slist, slist_member_hook<>, slisthook)
InnerBaseHook(Slist, slist_base_hook<>)
//Set
InnerMemberHook(Set, set_member_hook<>, sethook)
InnerBaseHook(Set, set_base_hook<>)
//Unordered Set
InnerMemberHook(USet, unordered_set_member_hook<>, usethook)
InnerBaseHook(USet, unordered_set_base_hook<>)

//Define containers
typedef list < MyClass3, function_hook< InnerMemberHookFunctorList> >         CustomListMember;
typedef list < MyClass3, function_hook< InnerBaseHookFunctorList  > >         CustomListBase;
typedef slist< MyClass3, function_hook< InnerMemberHookFunctorSlist> >        CustomSlistMember;
typedef slist< MyClass3, function_hook< InnerBaseHookFunctorSlist  > >        CustomSlistBase;
typedef set  < MyClass3, function_hook< InnerMemberHookFunctorSet> >          CustomSetMember;
typedef set  < MyClass3, function_hook< InnerBaseHookFunctorSet  > >          CustomSetBase;
typedef unordered_set< MyClass3, function_hook< InnerMemberHookFunctorUSet> > CustomUSetMember;
typedef unordered_set< MyClass3, function_hook< InnerBaseHookFunctorUSet  > > CustomUSetBase;

TEST_CASE("function_hook_test", "function_hook_test") {
    MyClass3 n;
    CustomListBase    listbase;
    CustomListMember  listmember;
    CustomSlistBase   slistbase;
    CustomSlistMember slistmember;
    CustomSetBase     setbase;
    CustomSetMember   setmember;
    typename CustomUSetBase::bucket_type buckets_uset[1];
    CustomUSetBase    usetbase(CustomUSetBase::bucket_traits(buckets_uset, 1));
    typename CustomUSetBase::bucket_type buckets_umultiset[1];
    CustomUSetMember  usetmember(CustomUSetMember::bucket_traits(buckets_umultiset, 1));

    listbase.insert(listbase.begin(), n);
    listmember.insert(listmember.begin(), n);
    slistbase.insert(slistbase.begin(), n);
    slistmember.insert(slistmember.begin(), n);
    setbase.insert(n);
    setmember.insert(n);
    usetbase.insert(n);
    usetmember.insert(n);
}