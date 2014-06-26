#include <list>
#include <string>
#include <iostream>

#include "hdr/typeclass.hpp"


///////////////// Show typeclass (simplified)


template <typename T>
struct Show
{
    static std::string show(T const &);
    static std::string showList(std::list<T> const &);

    using type = typeclass
                 <
                   decltype(show),
                   decltype(showList)
                 >;
};


namespace details
{
    has_function_(show, Show);
    has_function_(showList, Show);
}


template <typename Ty>
constexpr bool ShowInstance()
{
    return  global_instance<Show,Ty>::type::size != 0 &&
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
struct global_instance<Show, Test>
{
    using type =

        typeclass_instance
        <
            Show, Test,
            decltype(show),
            decltype(showList)
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

