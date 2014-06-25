#pragma once

#include <list>
#include <string>

#include "type-multiset.hpp"

template <typename ...Fs>
using typeclass = type::multiset<Fs...>;

template <template <typename> class Cl, typename Ty, typename ...Fs>
struct __typeclass_instance
{
    static_assert( type::equal_set< type::multiset<Fs...>, typename Cl<Ty>::type >(), "TypeClass: instance error");

    using type = typeclass<Fs...>;
};

template <template <typename> class Cl, typename Ty, typename ...Fs>
using typeclass_instance = typename __typeclass_instance<Cl, Ty, Fs...>::type;

template <template <typename> class Cl, typename Ty, typename ...Fs>
struct is_instance_of : std::integral_constant<bool, true>
{
    using type = typename Cl<Ty>::type;
};

#define has_function_(fun) \
template <typename Ty> \
struct has_function_ ## fun \
{ \
    using yes = char[1]; \
    using no  = char[2]; \
    \
    template <typename V, V value> struct type_check; \
    template <typename U> static yes& sfinae(type_check<typename std::decay<decltype(fun)>::type, fun> *); \
    template <typename U> static no&  sfinae(...); \
    \
    static constexpr bool value = sizeof(sfinae<Ty>(0)) == sizeof(yes); \
};



