#include <iostream>
#include <vector>
#include <stack>
#include <tuple>
#include <algorithm>
#include <chrono>
#include <thread>
#include <future>
#include <mutex>
#include <fstream>
#include "helper.hpp"

using namespace std;

template <typename Stack, typename Mutex, typename F>
void unroll_stack(Stack& s, Mutex&& m, F fun)
{
    while ([&](){
        m.lock();
        if (s.empty())
        {
            m.unlock();
            return false;
        }
        else
            return true;
    }())
    {
        auto e = s.top();
        s.pop();
        m.unlock();

        fun(e);
    }
}


int main()
{
    vector<int> v = {1, 2, 3, 6, 7, 3};
    auto last = [](auto it, auto const& v) { return it == end(v) - 1; };
    auto it = begin(v);

    // print elements of a vector using a comma as separator
    do {
        cout << *it;
    }
    while([&]() {
        if (last(it, v)) {
            return false;
        }
        else {
            cout << ", ";
            ++it;
            return true;
        }
    }());
    cout << endl;
    //output: 1, 2, 3, 6, 7, 3


    // do the same thiink with a do_while function
    it = begin(v);
    do_while(
        [&]{ cout << *it; },
        [&]{
            if (last(it, v)) {
                return false;
            }
            else {
                cout << ", ";
                ++it;
                return true;
            }
        }
    );
    cout << endl;
    //output: 1, 2, 3, 6, 7, 3


    //unroll_stack is a pattern to consume a stack in a mt enviroment
    //example:
    stack<int> s;
    s.push(1); s.push(2); s.push(3);
    unroll_stack(s, mutex(), [](auto e){ cout << e << " "; });
    cout << endl;
    // output: 3 2 1
}
