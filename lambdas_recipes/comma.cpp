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


template <typename T>
void print_vector(const vector<T>& v)
{
    typename vector<T>::const_iterator it;
    for (it = v.begin(); it != v.end(); ++it)
    {
        cout << *it;
        if (it != v.end() - 1)
            cout << ", ";
    }
    cout << endl;
}


template <typename T>
void print_vector11(const vector<T>& v)
{
    auto it = begin(v);
    while ([&](){
        if (it == begin(v)) { return true; }
        else if (it == end(v)) {cout << endl; return false; }
        else { cout << ", "; return true; }
    }())
    {
        cout << *it; ++it;
    }
}


template <typename T>
void print_vector11_2(const vector<T>& v)
{
    for_each_middle(begin(v), end(v), [](auto e) { cout << e; }, [] { cout<< ", "; });
}



int main()
{
    vector<int> v{1, 2, 3, 4, 5, 8};

    cout << "Print vector C++03" << endl;
    print_vector(v);

    cout << "Print vector C++11" << endl;
    print_vector11(v);

    cout << "Print vector C++11 2nd version" << endl;
    print_vector11_2(v);
} 
