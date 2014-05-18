#pragma once

#include <iostream>
using namespace std;

template <typename B, typename C>
void do_while(B body, C cond)
{
    do
    {
        body();
    }
    while(cond());
}


template <typename C, typename S, typename F>
decltype(auto) ternary(C cond, S succ, F fail)
{
    return cond() ? succ() : fail();
}


template <typename S, typename F>
decltype(auto) ternary(bool cond, S succ, F fail)
{
    return cond ? succ() : fail();
}


template <typename T, typename O=decltype(cout)>
void print_comma(const T& v, O& out_stream = cout)
{
    auto last = [](auto it, auto const& v) { return it == end(v) - 1; };
    auto it = begin(v);
    do_while(
        [&]{ out_stream << *it; },
        [&]{
            if (last(it, v)) {
                return false;
            }
            else {
                out_stream << ", ";
                ++it;
                return true;
            }
        }
    );
    out_stream << endl;
}
