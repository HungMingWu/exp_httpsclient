/////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Joaquin M Lopez Munoz  2006-2013
// (C) Copyright Ion Gaztanaga          2014-2014
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/intrusive for documentation.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef BOOST_INTRUSIVE_DETAIL_EBO_HOLDER_HPP
#define BOOST_INTRUSIVE_DETAIL_EBO_HOLDER_HPP

#ifndef BOOST_CONFIG_HPP
#  include <boost/config.hpp>
#endif

#if defined(BOOST_HAS_PRAGMA_ONCE)
#  pragma once
#endif

namespace boost {
namespace intrusive {
namespace detail {

#if defined(BOOST_MSVC) || defined(__BORLANDC_)
#define BOOST_INTRUSIVE_TT_DECL __cdecl
#else
#define BOOST_INTRUSIVE_TT_DECL
#endif

#if defined(_MSC_EXTENSIONS) && !defined(__BORLAND__) && !defined(_WIN64) && !defined(_M_ARM) && !defined(_M_ARM64) && !defined(UNDER_CE)
#define BOOST_INTRUSIVE_TT_TEST_MSC_FUNC_SIGS
#endif

template <typename T>
struct is_unary_or_binary_function_impl
{  static const bool value = false; };

// see boost ticket #4094
// avoid duplicate definitions of is_unary_or_binary_function_impl
#ifndef BOOST_INTRUSIVE_TT_TEST_MSC_FUNC_SIGS

template <typename R>
struct is_unary_or_binary_function_impl<R (*)()>
{  static const bool value = true;  };

template <typename R>
struct is_unary_or_binary_function_impl<R (*)(...)>
{  static const bool value = true;  };

#else // BOOST_INTRUSIVE_TT_TEST_MSC_FUNC_SIGS

template <typename R>
struct is_unary_or_binary_function_impl<R (__stdcall*)()>
{  static const bool value = true;  };

#ifndef _MANAGED

template <typename R>
struct is_unary_or_binary_function_impl<R (__fastcall*)()>
{  static const bool value = true;  };

#endif

template <typename R>
struct is_unary_or_binary_function_impl<R (__cdecl*)()>
{  static const bool value = true;  };

template <typename R>
struct is_unary_or_binary_function_impl<R (__cdecl*)(...)>
{  static const bool value = true;  };

#endif

// see boost ticket #4094
// avoid duplicate definitions of is_unary_or_binary_function_impl
#ifndef BOOST_INTRUSIVE_TT_TEST_MSC_FUNC_SIGS

template <typename R, class T0>
struct is_unary_or_binary_function_impl<R (*)(T0)>
{  static const bool value = true;  };

template <typename R, class T0>
struct is_unary_or_binary_function_impl<R (*)(T0...)>
{  static const bool value = true;  };

#else // BOOST_INTRUSIVE_TT_TEST_MSC_FUNC_SIGS

template <typename R, class T0>
struct is_unary_or_binary_function_impl<R (__stdcall*)(T0)>
{  static const bool value = true;  };

#ifndef _MANAGED

template <typename R, class T0>
struct is_unary_or_binary_function_impl<R (__fastcall*)(T0)>
{  static const bool value = true;  };

#endif

template <typename R, class T0>
struct is_unary_or_binary_function_impl<R (__cdecl*)(T0)>
{  static const bool value = true;  };

template <typename R, class T0>
struct is_unary_or_binary_function_impl<R (__cdecl*)(T0...)>
{  static const bool value = true;  };

#endif

// see boost ticket #4094
// avoid duplicate definitions of is_unary_or_binary_function_impl
#ifndef BOOST_INTRUSIVE_TT_TEST_MSC_FUNC_SIGS

template <typename R, class T0, class T1>
struct is_unary_or_binary_function_impl<R (*)(T0, T1)>
{  static const bool value = true;  };

template <typename R, class T0, class T1>
struct is_unary_or_binary_function_impl<R (*)(T0, T1...)>
{  static const bool value = true;  };

#else // BOOST_INTRUSIVE_TT_TEST_MSC_FUNC_SIGS

template <typename R, class T0, class T1>
struct is_unary_or_binary_function_impl<R (__stdcall*)(T0, T1)>
{  static const bool value = true;  };

#ifndef _MANAGED

template <typename R, class T0, class T1>
struct is_unary_or_binary_function_impl<R (__fastcall*)(T0, T1)>
{  static const bool value = true;  };

#endif

template <typename R, class T0, class T1>
struct is_unary_or_binary_function_impl<R (__cdecl*)(T0, T1)>
{  static const bool value = true;  };

template <typename R, class T0, class T1>
struct is_unary_or_binary_function_impl<R (__cdecl*)(T0, T1...)>
{  static const bool value = true;  };
#endif

template <typename T>
struct is_unary_or_binary_function_impl<T&>
{  static const bool value = false; };

template<typename T>
struct is_unary_or_binary_function : is_unary_or_binary_function_impl<T>
{};

template<typename T, typename Tag = void, bool = is_unary_or_binary_function<T>::value>
class ebo_functor_holder
{
    public:
   typedef T functor_type;

   inline ebo_functor_holder()
      : t_()
   {}

   inline explicit ebo_functor_holder(const T &t)
      : t_(t)
   {}

   inline explicit ebo_functor_holder(T&& t)
      : t_(std::move(t))
   {}

   template<class Arg1, class Arg2>
   inline ebo_functor_holder(Arg1&& arg1, Arg2&& arg2)
      : t_(std::forward<Arg1>(arg1), std::forward<Arg2>(arg2))
   {}

   inline ebo_functor_holder(const ebo_functor_holder &x)
      : t_(x.t_)
   {}

   inline ebo_functor_holder(ebo_functor_holder&& x) : t_(x.t_) {}

   inline ebo_functor_holder& operator=(const ebo_functor_holder& x)
   {
      this->get() = x.get();
      return *this;
   }

   inline ebo_functor_holder& operator=(ebo_functor_holder&& x)
   {
      this->get() = std::move(x.get());
      return *this;
   }

   inline ebo_functor_holder& operator=(const T &x)
   {
      this->get() = x;
      return *this;
   }

   inline ebo_functor_holder& operator=(T&& x)
   {
      this->get() = std::move(x);
      return *this;
   }

   inline T&       get(){return t_;}
   inline const T& get()const{return t_;}

   private:
   T t_;
};

template<typename T, typename Tag>
class ebo_functor_holder<T, Tag, false>
   :  public T
{
   public:
   typedef T functor_type;

   inline ebo_functor_holder()
      : T()
   {}

   inline explicit ebo_functor_holder(const T &t)
      : T(t)
   {}

   inline explicit ebo_functor_holder(T&& t)
      : T(std::move(t))
   {}

   template<class Arg1, class Arg2>
   inline ebo_functor_holder(Arg1&& arg1, Arg2&& arg2)
      : T(std::forward<Arg1>(arg1), std::forward<Arg2>(arg2))
   {}

   inline ebo_functor_holder(const ebo_functor_holder &x)
      : T(static_cast<const T&>(x))
   {}

   inline ebo_functor_holder(ebo_functor_holder&& x)
      : T(std::move(x))
   {}

   inline ebo_functor_holder& operator=(const ebo_functor_holder &x)
   {
      const ebo_functor_holder&r = x;
      this->get() = r;
      return *this;
   }

   inline ebo_functor_holder& operator=(ebo_functor_holder&& x)
   {
      this->get() = std::move(x.get());
      return *this;
   }

   inline ebo_functor_holder& operator=(const T &x)
   {
      this->get() = x;
      return *this;
   }

   inline ebo_functor_holder& operator=(T&& x)
   {
      this->get() = std::move(x);
      return *this;
   }

   inline T&       get(){return *this;}
   inline const T& get()const{return *this;}
};

}  //namespace detail {
}  //namespace intrusive {
}  //namespace boost {

#endif   //#ifndef BOOST_INTRUSIVE_DETAIL_EBO_HOLDER_HPP
