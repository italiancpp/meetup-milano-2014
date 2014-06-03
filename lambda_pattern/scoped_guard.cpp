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
struct ScopedGuard
{
    ScopedGuard(F f) : active_(true), guard(move(f)) {}
    ScopedGuard(ScopedGuard&& rhs) : active_(rhs.active_), guard(move(rhs.guard)) {
        rhs.dismiss();
    }

    void dismiss() { active_ = false; }

    ~ScopedGuard() {
        if (active_) { guard(); }
    }

    ScopedGuard() = delete;
    ScopedGuard(const ScopedGuard&) = delete;
    ScopedGuard& operator=(const ScopedGuard&) = delete;
private:
    bool active_;
    F guard;
};


template <typename F>
ScopedGuard<F> scopedGuard(F&& fun)
{
    return ScopedGuard<F>(forward<F>(fun));
}

void recv_data() {throw std::exception();}
//void recv_data() {}




int main()
{
    ofstream foo("data.txt");
    auto g = scopedGuard([&](){
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
