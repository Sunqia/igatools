//-+--------------------------------------------------------------------
// This file is part of igatools, a general purpose Isogeometric analysis
// library. It was copied from the web-page
// http://www.cse.unsw.edu.au/~jarrydb/thenewcpp/mpl.hpp
// where it is licensed under the Boost Software License, Version 1.0 (see below).
// It has been modified by the igatools authors to fit the igatools framework.
//-+--------------------------------------------------------------------

/* Meta-programming utility.
   Copyright (C) 2013 Jarryd Beck

Distributed under the Boost Software License, Version 1.0

Permission is hereby granted, free of charge, to any person or organization
obtaining a copy of the software and accompanying documentation covered by
this license (the "Software") to use, reproduce, display, distribute,
execute, and transmit the Software, and to prepare derivative works of the
Software, and to permit third-parties to whom the Software is furnished to
do so, all subject to the following:

  The copyright notices in the Software and this entire statement, including
  the above license grant, this restriction and the following disclaimer,
  must be included in all copies of the Software, in whole or in part, and
  all derivative works of the Software, unless such copies or derivative
  works are solely in the form of machine-executable object code generated by
  a source language processor.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
  SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
  FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
  ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
  DEALINGS IN THE SOFTWARE.
*/

#ifndef MPL_UTIL_H_
#define MPL_UTIL_H_

#include <type_traits>
#include <cstdlib>

namespace detail
{
template< template <typename> class Size, typename SoFar,typename... Args>
struct max_storage_helper;

template<template <typename> class Size, typename SoFar>
struct max_storage_helper<Size, SoFar>
{
    static constexpr decltype(Size<SoFar>::value) value = Size<SoFar>::value;
    using type = SoFar;
};

template<template <typename> class Size,typename SoFar,typename Next,typename... Args>
struct max_storage_helper<Size, SoFar, Next, Args...>
{
private:
    using m_next =  typename std::conditional<
                    (Size<Next>::value > Size<SoFar>::value),
                    max_storage_helper<Size, Next, Args...>,
                    max_storage_helper<Size, SoFar, Args...> >::type;

public:
    static constexpr decltype(Size<SoFar>::value) value = m_next::value;

    using type = typename m_next::type;
};
}

template <template <typename> class Size, typename... Args>
struct max_storage;

template<template <typename> class Size,typename First,typename... Args>
struct max_storage<Size, First, Args...>
{
private:
    using m_size_type = decltype(Size<First>::value);
    using m_helper = typename detail::max_storage_helper<Size,First,Args...>;

public:
    static constexpr m_size_type value = m_helper::value;
    using type = typename m_helper::type;
};

#endif // #ifndef MPL_UTIL_H_