#pragma once

#include <list>
#include <string>

#include "multiset.hpp"

template <typename ...Fs>
using typeclass = type::multiset<Fs...>;

template <template <typename> class Class, typename Ty, typename ...Fs>
struct __typeclass_instance
{
    static_assert( !(sizeof...(Fs) <  Class<Ty>::type::size ), "instance declaration: incomplete interface");
    static_assert( !(sizeof...(Fs) >  Class<Ty>::type::size ), "instance declaration: too many method");

    static_assert( type::equal_set< type::multiset<Fs...>, typename Class<Ty>::type >(), "instance declaration: function(s) mismatch");

    using type = typeclass<Fs...>;
};


template <template <typename> class Class, typename Ty, typename ...Fs>
using typeclass_instance = typename __typeclass_instance<Class, Ty, Fs...>::type;


template <template <typename> class C, typename T> struct global_instance
{
    using type = typeclass<>;
};


#define has_function_(fun, _class) \
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



