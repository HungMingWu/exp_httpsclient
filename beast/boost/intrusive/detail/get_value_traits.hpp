/////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga  2014-2014
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/intrusive for documentation.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef BOOST_INTRUSIVE_DETAIL_GET_VALUE_TRAITS_HPP
#define BOOST_INTRUSIVE_DETAIL_GET_VALUE_TRAITS_HPP

#ifndef BOOST_CONFIG_HPP
#  include <boost/config.hpp>
#endif

#if defined(BOOST_HAS_PRAGMA_ONCE)
#  pragma once
#endif

#include <boost/intrusive/detail/mpl.hpp>
#include <boost/intrusive/detail/hook_traits.hpp>

namespace boost {
namespace intrusive {

#ifndef BOOST_INTRUSIVE_DOXYGEN_INVOKED

template<class SupposedValueTraits>
struct is_default_hook_tag
{  static const bool value = false;  };

namespace detail{

template <typename T, typename BaseHook>
struct concrete_hook_base_value_traits
{
   using tags = typename BaseHook::hooktags;
   using type = bhtraits<T, 
                         typename tags::node_traits,
                         tags::link_mode,
                         typename tags::tag,
                         tags::type>;
};

template <typename BaseHook>
struct concrete_hook_base_value_traits<void, BaseHook>
{
   using type = typename BaseHook::hooktags;
};

template <typename T, typename BaseHook>
using concrete_hook_base_value_traits_t = typename concrete_hook_base_value_traits<T, BaseHook>::type;

template <typename T, typename AnyToSomeHook_ProtoValueTraits>
struct any_hook_base_value_traits
{
   //AnyToSomeHook value_traits derive from a generic_hook
   //The generic_hook is configured with any_node_traits
   //and AnyToSomeHook::value_traits with the correct
   //node traits for the container, so use node_traits
   //from AnyToSomeHook_ProtoValueTraits and the rest of
   //elements from the hooktags member of the generic_hook

   using basic_hook_t = typename AnyToSomeHook_ProtoValueTraits::basic_hook_t;
   using void_pointer = pointer_rebind_t<typename basic_hook_t::hooktags::node_traits::node_ptr, void>;
   using node_traits = typename AnyToSomeHook_ProtoValueTraits::template
                       node_traits_from_voidptr<void_pointer>::type;

   using type = bhtraits<T,
                         node_traits,
                         basic_hook_t::hooktags::link_mode,
                         typename basic_hook_t::hooktags::tag,
                         basic_hook_t::hooktags::type>;
};

template <typename AnyToSomeHook_ProtoValueTraits>
struct any_hook_base_value_traits<void, AnyToSomeHook_ProtoValueTraits>
{
   using basic_hook_t = typename AnyToSomeHook_ProtoValueTraits::basic_hook_t;
   using void_pointer = pointer_rebind_t<typename basic_hook_t::hooktags::node_traits::node_ptr, void>;

   struct type
   {
       using node_traits = typename AnyToSomeHook_ProtoValueTraits::template
                           node_traits_from_voidptr<void_pointer>::type;
   };
};

template <typename T, typename AnyToSomeHook_ProtoValueTraits>
using any_hook_base_value_traits_t = typename any_hook_base_value_traits<T, AnyToSomeHook_ProtoValueTraits>::type;

template <typename MemberHook>
using get_member_value_traits_t = typename MemberHook::member_value_traits;

template <typename T>
concept internal_base_hook = std::is_convertible_v<decltype(T::hooktags::is_base_hook), bool> && T::hooktags::is_base_hook;

template <typename T>
concept internal_any_hook = std::is_convertible_v<decltype(T::is_any_hook), bool> && T::is_any_hook;

template <typename T>
using member_value_traits_t = typename T::member_value_traits;

template<class SupposedValueTraits, class T, bool = is_default_hook_tag<SupposedValueTraits>::value>
struct supposed_value_traits;

template<class T, class BaseHook, bool = internal_any_hook<BaseHook>>
struct get_base_value_traits;

template<class SupposedValueTraits, class T, bool value>
struct supposed_base_value_traits1;

template <class SupposedValueTraits, class T, bool value = internal_base_hook<SupposedValueTraits>>
using supposed_base_value_traits_t = typename supposed_base_value_traits1<SupposedValueTraits, T, value>::type;

template<class SupposedValueTraits, bool = check_action<member_value_traits_t, SupposedValueTraits>>
struct supposed_member_value_traits;

template <typename SupposedValueTraits>
using supposed_member_value_traits_t = typename supposed_member_value_traits<SupposedValueTraits>::type;

template<class SupposedValueTraits, bool = internal_any_hook<SupposedValueTraits>>
struct any_or_concrete_value_traits;

//Base any hook
template <typename T, typename BaseHook>
struct get_base_value_traits<T, BaseHook, true>
{
    using type = any_hook_base_value_traits_t<T, BaseHook>;
};

//Non-any base hook
template <typename T, typename BaseHook>
struct get_base_value_traits<T, BaseHook, false>
{
    using type = concrete_hook_base_value_traits_t<T, BaseHook>;
};

template <typename T, typename BaseHook>
using get_base_value_traits_t = typename get_base_value_traits<T, BaseHook>::type;

//...It's a default hook
template <typename SupposedValueTraits, typename T>
struct supposed_value_traits<SupposedValueTraits, T, true>
{  
    using type = typename SupposedValueTraits::template apply<T>::type;
};

//...Not  a default hook
template <typename SupposedValueTraits, typename T>
struct supposed_value_traits<SupposedValueTraits, T, false>
{ 
    using type = SupposedValueTraits;
};

template <typename SupposedValueTraits, typename T>
using supposed_value_traits_t = typename supposed_value_traits<SupposedValueTraits, T>::type;

//...It's a base hook
template<class BaseHook, class T>
struct supposed_base_value_traits1<BaseHook, T, true>
{
    using type = get_base_value_traits_t<T, BaseHook>;
};

//...Not a base hook, try if it's a member or value_traits
template<class SupposedValueTraits, class T>
struct supposed_base_value_traits1<SupposedValueTraits, T, false>
{
    using type = typename supposed_member_value_traits<SupposedValueTraits>::type;
};

//...It's a member hook
template<class MemberHook>
struct supposed_member_value_traits<MemberHook, true>
{
    using type = get_member_value_traits_t<MemberHook>;
};

//...Not a member hook
template<class SupposedValueTraits>
struct supposed_member_value_traits<SupposedValueTraits, false>
{
    using type = typename any_or_concrete_value_traits<SupposedValueTraits>::type;
};

template<class AnyToSomeHook_ProtoValueTraits>
struct any_or_concrete_value_traits<AnyToSomeHook_ProtoValueTraits, true>
{
   //A hook node (non-base, e.g.: member or other value traits
   using basic_hook_t = typename AnyToSomeHook_ProtoValueTraits::basic_hook_t;
   using void_pointer = pointer_rebind_t<typename basic_hook_t::node_ptr, void>;
   using any_node_traits = typename AnyToSomeHook_ProtoValueTraits::template
       node_traits_from_voidptr<void_pointer>::type;

   struct type : basic_hook_t
   {
      using node_traits = any_node_traits;
   };
};

template<class SupposedValueTraits>
struct any_or_concrete_value_traits<SupposedValueTraits, false>
{
   using type = SupposedValueTraits;
};

////////////////////////////////////////
//  get_value_traits / get_node_traits
////////////////////////////////////////

template <typename T, typename SupposedValueTraits>
using get_value_traits_t = supposed_base_value_traits_t<supposed_value_traits_t<SupposedValueTraits, T>, T>;

template<class SupposedValueTraits>
struct get_node_traits
{
   using type = typename get_value_traits_t<void, SupposedValueTraits>::node_traits;
};

}  //namespace detail{

#endif   //BOOST_INTRUSIVE_DOXYGEN_INVOKED

}  //namespace intrusive {
}  //namespace boost {

#endif   //#ifndef BOOST_INTRUSIVE_DETAIL_GET_VALUE_TRAITS_HPP
