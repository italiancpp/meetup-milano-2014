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

using namespace std;


bool getData(string k, int& d) {
    cout << "calling for key: " << k << endl;
    const map<string, bool> m {
        make_pair ("fiat", true),
        make_pair ("bmw", false),
        make_pair ("mercedes", false)
    };
    
    d = k.length();
    return m.at(k);
}


/*
 * Imagine that you have this function to lookup (using a key) some values from a container.
 * If this key is present you are able to write the associated value on screen otherwise you must exit from the function.
 * A possibile solution could be somenthing like that:
 */
void unit_03()
{
    bool b; int d;

    b = getData("fiat", d);
    if (!b)
    {
        return;
    }
    else
    {
        cout << "fiat: " << d << endl;
    }

    b = getData("bmw", d);
    if (!b)
    {
        return;
    }
    else
    {
        cout << "bmw: " << d << endl;
    }

    b = getData("mercedes", d);
    if (!b)
    {
        return;
    }
    else
    {
        cout << "mercedes: " << d << endl;
    }
}


/*
    In C++11 we can simplify our problems using lambda and variadic template
*/
template <typename F>
void run_if(F f)
{
    f();
}

template <typename F, typename ...Funs>
void run_if(F f, Funs ...funs)
{
    if (f())
        run_if(forward<Funs>(funs)...);
}


void unit_11()
{
    // we define a lamda function as base block of computation
    auto f = [&](string k) -> bool {
        // look for data
        int d; auto b = getData(k, d);
        if (!b)
        {
            // if no value is present stop!
            return false;
        }
        else
        {
            //ok I found a good value so I can carry on
            cout << k << ": " << d << endl;
            return true;
        }
    };


    // Now we use a "glue" function. This function take an arbitrary numbers of lambda and execute it.
    // For every function we check the result of execution and if it's true we continue to execute the chain of lambda.

    run_if(
        [&] { return f("fiat"); },
        [&] { return f("bmw"); },
        [&] { return f("mercedes"); }
    );
}


int main()
{
    unit_03();
    cout << endl;
    unit_11();
}
