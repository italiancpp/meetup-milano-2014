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


#define has_function_(fun, _class, _type) \
template <typename __Type> \
struct has_function_ ## fun \
{ \
    using yes = char[1]; \
    using no  = char[2]; \
    \
    template <typename __Fun, __Fun value> struct type_check; \
    template <typename __Type2> static yes& check(type_check<typename std::decay<decltype(_class<_type>::fun)>::type, fun> *); \
    template <typename __Type2> static no&  check(...); \
    \
    static constexpr bool value = sizeof(check<__Type>(0)) == sizeof(yes); \
};



