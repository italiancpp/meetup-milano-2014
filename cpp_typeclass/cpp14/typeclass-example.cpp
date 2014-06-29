#include <list>
#include <string>
#include <iostream>

#include <typeclass.hpp>


///////////////// Show typeclass


template <typename T>
struct Show
{
    static std::string show(T const &);
    static std::string showList(std::list<T> const &);

    using type = typeclass
                 <
                   symbol(show),
                   symbol(showList)
                 >;
};


namespace details
{
    has_function_(show);
    has_function_(showList);
}


template <typename Ty>
constexpr bool ShowInstance()
{
    return  type::equal_set< typename typeclass_instance<Show,Ty>::type, typename Show<Ty>::type>() &&

            details::has_function_show<Ty>::value &&
            details::has_function_showList<std::list<Ty>>::value;
};


///////////////// User defined type:


struct Test { };

std::string show(Test const &)
{
    return "Test";
}

std::string showList(std::list<Test> const &)
{
    return "[Test]";
}


///////////////// Instance declaration


template <>
struct typeclass_instance<Show, Test>
{
    using type =

        typeclass
        <
            symbol(show),
            symbol(showList)
        >;
};


///////////////// Constraint with Concept Lite:

#if 0
template <ShowInstance T>
void print(const T &type)
{
    std::cout << show (type) << std::endl;
}
#endif


int
main(int, char *[])
{
     static_assert(ShowInstance<Test>(), "Show instance error");
     static_assert(!ShowInstance<int>(), "Show internal error");

     std::cout << show(Test{}) << std::endl;
     std::cout << showList(std::list<Test>{}) << std::endl;

     return 0;
}

