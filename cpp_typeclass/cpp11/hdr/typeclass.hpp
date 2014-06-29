#pragma once

#include <list>
#include <string>

#include "multiset.hpp"

template <typename ...Fs>
using typeclass = type::multiset<Fs...>;


template <template <typename> class C, typename T> struct typeclass_instance
{
    using type = typeclass<>;
};


#define has_function_(fun) \
template <typename __Type> \
struct has_function_ ## fun \
{ \
    using yes = char[1]; \
    using no  = char[2]; \
    \
    template <typename __Type2> static yes& check(typename std::decay<decltype(fun(std::declval<__Type2>()))>::type *); \
    template <typename __Type2> static no&  check(...); \
    \
    static constexpr bool value = sizeof(check<__Type>(0)) == sizeof(yes); \
};



