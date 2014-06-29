#pragma once

#include <string>

#include "typeset.hpp"


/// template string (c++14 required):


template <char ...cs>
struct tstring { };

template <typename CharT, CharT ...str>
tstring<str...> operator""_tstr()
{
    return tstring<str...>{};
}


/// template symbol:


template <typename Tstring, typename Fun> struct tsymbol { };


#define symbol(fun) tsymbol<decltype(#fun ## _tstr), decltype(fun)>


/// typeclass:


template <typename ...Fs>
using typeclass = type::multiset<Fs...>;



/// typeclass_instance:


template <template <typename> class C, typename T> struct typeclass_instance
{
    using type = typeclass<>;
};


/// SFINAE utility macro:


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


