/////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2007-2013
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/intrusive for documentation.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef BOOST_INTRUSIVE_GENERIC_HOOK_HPP
#define BOOST_INTRUSIVE_GENERIC_HOOK_HPP

#ifndef BOOST_CONFIG_HPP
#  include <boost/config.hpp>
#endif

#if defined(BOOST_HAS_PRAGMA_ONCE)
#  pragma once
#endif

#include <boost/intrusive/pointer_traits.hpp>
#include <boost/intrusive/link_mode.hpp>
#include <boost/intrusive/detail/mpl.hpp>
#include <boost/intrusive/detail/assert.hpp>
#include <boost/intrusive/detail/node_holder.hpp>
#include <boost/intrusive/detail/algo_type.hpp>

namespace boost {
namespace intrusive {

/// @cond

enum base_hook_type
{  NoBaseHookId
,  ListBaseHookId
,  SlistBaseHookId
,  RbTreeBaseHookId
,  HashBaseHookId
,  AvlTreeBaseHookId
,  BsTreeBaseHookId
,  TreapTreeBaseHookId
,  AnyBaseHookId
};


template <class HookTags, unsigned int>
struct hook_tags_definer {};

template <class HookTags>
struct hook_tags_definer<HookTags, ListBaseHookId>
{
    using default_list_hook = HookTags;
};

template <class HookTags>
struct hook_tags_definer<HookTags, SlistBaseHookId>
{
    using default_slist_hook = HookTags;
};

template <class HookTags>
struct hook_tags_definer<HookTags, RbTreeBaseHookId>
{
    using default_rbtree_hook = HookTags;
};

template <class HookTags>
struct hook_tags_definer<HookTags, HashBaseHookId>
{
    using default_hashtable_hook = HookTags;
};

template <class HookTags>
struct hook_tags_definer<HookTags, AvlTreeBaseHookId>
{
    using default_avltree_hook = HookTags;
};

template <class HookTags>
struct hook_tags_definer<HookTags, BsTreeBaseHookId>
{
    using default_bstree_hook = HookTags;
};

template <class HookTags>
struct hook_tags_definer<HookTags, AnyBaseHookId>
{
    using default_any_hook = HookTags;
};

template
   < class NodeTraits
   , class Tag
   , link_mode_type LinkMode
   , base_hook_type BaseHookType
   >
struct hooktags_impl
{
   static const link_mode_type link_mode = LinkMode;
   using tag = Tag;
   using node_traits = NodeTraits;
   static const bool is_base_hook = !std::is_same<Tag, member_tag>::value;
   static constexpr bool safemode_or_autounlink = is_safe_autounlink(link_mode);
   static const unsigned int type = BaseHookType;
};

/// @endcond

template
   < boost::intrusive::algo_types Algo
   , class NodeTraits
   , class Tag
   , link_mode_type LinkMode
   , base_hook_type BaseHookType
   >
class generic_hook
   /// @cond
   //If the hook is a base hook, derive generic hook from node_holder
   //so that a unique base class is created to convert from the node
   //to the type. This mechanism will be used by bhtraits.
   //
   //If the hook is a member hook, generic hook will directly derive
   //from the hook.
   : public std::conditional_t<
      std::is_same_v<Tag, member_tag>,
      typename NodeTraits::node, 
      node_holder<typename NodeTraits::node, Tag, BaseHookType>
     >
   //If this is the a default-tagged base hook derive from a class that
   //will define an special internal typedef. Containers will be able to detect this
   //special typedef and obtain generic_hook's internal types in order to deduce
   //value_traits for this hook.
   , public hook_tags_definer
      < generic_hook<Algo, NodeTraits, Tag, LinkMode, BaseHookType>
      , std::is_same_v<Tag, dft_tag> ? BaseHookType : NoBaseHookId>
   /// @endcond
{
   /// @cond
   using node_algorithms = get_algo_t<Algo, NodeTraits>;
   using node = typename node_algorithms::node;
   using node_ptr = typename node_algorithms::node_ptr;
   using const_node_ptr = typename node_algorithms::const_node_ptr;

   public:

   using hooktags = hooktags_impl<NodeTraits, Tag, LinkMode, BaseHookType>;

   inline node_ptr this_ptr()
   {  return std::pointer_traits<node_ptr>::pointer_to(static_cast<node&>(*this)); }

   inline const_node_ptr this_ptr() const
   {  return pointer_traits<const_node_ptr>::pointer_to(static_cast<const node&>(*this)); }

   public:
   /// @endcond

   inline generic_hook()
   {
      if constexpr (hooktags::safemode_or_autounlink) {
         node_algorithms::init(this->this_ptr());
      }
   }

   inline generic_hook(const generic_hook& )
   {
      if constexpr (hooktags::safemode_or_autounlink) {
         node_algorithms::init(this->this_ptr());
      }
   }

   inline generic_hook& operator=(const generic_hook& )
   {  return *this;  }

   inline ~generic_hook()
   {
      if constexpr (hooktags::link_mode == link_mode_type::safe_link) {
        //If this assertion raises, you might have destroyed an object
        //while it was still inserted in a container that is alive.
        //If so, remove the object from the container before destroying it.
        BOOST_INTRUSIVE_SAFE_HOOK_DESTRUCTOR_ASSERT(!is_linked());
      }
      else if constexpr (hooktags::link_mode == link_mode_type::auto_unlink) {
        unlink();
      }
   }

   inline void swap_nodes(generic_hook &other)
   {
      node_algorithms::swap_nodes
         (this->this_ptr(), other.this_ptr());
   }

   inline bool is_linked() const
   {
      //is_linked() can be only used in safe-mode or auto-unlink
      static_assert(( hooktags::safemode_or_autounlink ));
      return !node_algorithms::unique(this->this_ptr());
   }

   inline void unlink()
   {
      static_assert(hooktags::link_mode == link_mode_type::auto_unlink);
      node_ptr n(this->this_ptr());
      if(!node_algorithms::inited(n)){
         node_algorithms::unlink(n);
         node_algorithms::init(n);
      }
   }
};

} //namespace intrusive
} //namespace boost

#endif //BOOST_INTRUSIVE_GENERIC_HOOK_HPP
