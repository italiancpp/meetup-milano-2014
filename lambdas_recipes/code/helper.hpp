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


template <typename C, typename B>
void while_do(C cond, B body)
{
    while(cond())
    {
        body();
    }
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


template <typename Ib, typename Ie, typename Fun, typename Middle>
void for_each_middle(Ib it_b, Ie it_e, Fun fun, Middle middle)
{
    auto it = it_b;
    while ([&](){
        if (it == it_b) { return true; }
        else if (it == it_e) {cout << endl; return false; }
        else { middle(); return true; }
    }())
    {
        fun(*it);
        ++it;
    }
}
