#include <iostream>
#include <vector>
#include <map>
#include <stack>
#include <tuple>
#include <algorithm>
#include <chrono>
#include <thread>
#include <future>

#include <mutex>
#include "helper.hpp"

#include <boost/shared_ptr.hpp>


using namespace std;


/**
 * Implementation of Decorator Pattern in C++14
 * thanks to: N. Bonelli & A. Bigagli
 */
template<typename Decorator>
decltype(auto) decorator(Decorator d)
{
    return [d] (auto&& ...args){ // auto&& => type deduction => universal reference

        /**
         * decltype(args) is the effective lambda's parameter type (being a template parameter pack doesn't make any difference here) after template deduction and collapsing rules have been applied
         * Since auto is powered by template deduction mechanisms, as with normal functions with "universal reference" arguments, if the lambda is invoked with:
         * 1) an lvalue of type T, "auto" will be deduced as a T& and so the effective parameter type will be "T& &&" which will collapse to T& --> decltype(args) is T&
         * 2) an rvalue of type T, "auto" will be deduced as a T and so the effective parameter type will be "T&&" --> decltype(arg) is T&&
         * 
         * so, when recovering the type category through std::forward<decltype(arg)>, remembering that std::forward<U> returns a static_cast<U&&> (§20.2.3) we will be doing:
         * for lvalue (case 1): d(forward<T&>(p)...) --> static_cast<U&& &> --> static_cast<U&> i.e. lvalue-ness is preserved
         * for rvalue (case 2): d(forward<T&&>(p)...) --> static_cast<U&& &&> --> static_cast<U&&> i.e. rvalue-ness is preserved
         */

        d(forward<decltype(args)>(args)...);
    };
}


// First decorator: print an entry log when function is called and print and another log when function return

template<typename Fun>
decltype(auto) head_trail(Fun f)
{
    return decorator([f](auto&& ...args){
        cout << "*** START FUNCTION CALL ***" << endl;
        f(forward<decltype(args)>(args)...);
        cout << "*** END FUNCTION ***" << endl << endl;
    });
}


template<typename Fun>
decltype(auto) misure(Fun f)
{
    /*TODO!*/
    return decorator([f](auto&& ...args){
        //chrono t;

        f(forward<decltype(args)>(args)...);

        cout << "Function runs in: " << endl << endl;
    });
}



// TEST FUNCTIONs: both are functions template

template<typename N, typename S>
void print_name(N&& name, S&& surname) // N&& and S&& are universal reference
{
    cout << "Name: " << name << ", Surname: " << surname << endl;
}


template <typename T>
void test_unique(T&& p) // T&& is universal reference
{
    cout << "p: " << *p << endl;
}



int main()
{
    //perfect fw to an existing function!
    auto d_printname = head_trail([](auto&& name, auto&& surname){
        print_name(name, surname);
    });


    string s{"Mario"}, c{"Rossi"}; 
    d_printname("Scott", "Meyers");
    d_printname(s, c);

    const string cs{"Godel"}, cc{"Esher"};
    d_printname(cs, cc);


    unique_ptr<int> p(new int(10));
    cout << *p << endl;

    auto d_testunique = decorator([](auto&& ...args){
        cout << "calling test_unique!" << endl;
        test_unique(forward<decltype(args)>(args)...);
    });


    // now test lvalue reference of unique_ptr<int>
    d_testunique(p);

    // here we are passing a temp so => testing rvalue reference of unique_ptr<int>
    d_testunique(unique_ptr<int>(new int(42)));
}
