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

    has_function_(show);
    has_function_(showList);
};


template <typename Ty>
constexpr bool ShowInstance()
{
    return  Show<Ty>::template has_function_show<Ty>::value &&
            Show<Ty>::template has_function_showList<std::list<Ty>>::value;
};


///////////////// User defined type:


struct Test { };

std::string show(Test const &)
{
    return "test";
}

std::string showList(std::list<Test> const &)
{
    return "[test]";
}


///////////////// Instance declaration


using Show_Test_instance =

    typeclass_instance
    <
        Show, Test,
        decltype(showList),
        decltype(show)
    >;


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

     std::cout << show(Test{}) << std::endl;
     std::cout << showList(std::list<Test>{}) << std::endl;

     return 0;
}

