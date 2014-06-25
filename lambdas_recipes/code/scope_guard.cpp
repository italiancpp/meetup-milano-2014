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
#include <fstream>
#include <unistd.h>
#include "helper.hpp"

using namespace std;


template <typename F>
struct ScopeGuard
{
    ScopeGuard(F f) : active_(true), guard(move(f)) {}
    ScopeGuard(ScopeGuard&& rhs) : active_(rhs.active_), guard(move(rhs.guard)) {
        rhs.dismiss();
    }

    void dismiss() { active_ = false; }

    ~ScopeGuard() {
        if (active_) { guard(); }
    }

    ScopeGuard() = delete;
    ScopeGuard(const ScopeGuard&) = delete;
    ScopeGuard& operator=(const ScopeGuard&) = delete;
private:
    bool active_;
    F guard;
};


template <typename F>
ScopeGuard<F> scopeGuard(F&& fun)
{
    return ScopeGuard<F>(forward<F>(fun));
}



void recv_data() {throw std::exception();}
//void recv_data() {}



int main()
{
    ofstream foo("data.txt");
    auto g = scopeGuard([&](){
        foo.close();
        cout << "error!" << endl;
        unlink("data.txt");
    });

    try {
        recv_data();
        /* write data inside foo */
        g.dismiss();
        foo.close();
    }
    catch(...) {}

    return 0;
}
