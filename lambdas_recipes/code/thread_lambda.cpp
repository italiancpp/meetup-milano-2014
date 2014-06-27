#include <iostream>
#include <vector>
#include <map>
#include <deque>
#include <tuple>
#include <algorithm>
#include <chrono>
#include <thread>
#include <future>
#include <mutex>
#include "helper.hpp"

using namespace std;


template <typename F>
void just_lock(std::mutex& l, F fun)
{
    lock_guard<std::mutex> g(l);
    fun();
}



class Worker
{
public:
    Worker() : stopped{false}
    {
        wth = thread([&] {
            while (!stopped)
            {
                m_.lock();
                if (!funs.empty())
                {
                    auto f = move(funs.front());
                    funs.pop_front();
                    m_.unlock();
                    f(); //execute
                }
                else
                {
                    m_.unlock();
                    this_thread::sleep_for (chrono::milliseconds(50));
                }
            }
            cout << "Exit from worker!" << endl;
        });
    }

    void stop() { send_log([this]{ stopped = true; }); }

    template <typename F>
    void send_log(F&& f)
    {
        just_lock(m_, [&]{ funs.push_back(forward<F>(f)); });
    }
    
    ~Worker() { stop(); wth.join(); }

    Worker(const Worker& w) =delete;
    Worker& operator=(const Worker& w) =delete;

private:
    deque<function<void(void)>> funs;
    mutable mutex m_;
    bool stopped;
    thread wth;
};


class Logger
{
public:
    Logger() : wth{} {}

    void info(string msg)  { wth.send_log([=]{ cout << "[ INFO  ] "  <<  msg << endl; }); }
    void debug(string msg) { wth.send_log([=]{ cout << "[ DEBUG ] "  <<  msg << endl; }); }

    ~Logger() { wth.stop(); }

private:
    Worker wth;
};



int main()
{
    Logger log;
    log.info("Hello World!");

    auto th1 = thread([&log]{
        for (auto i = 0; i < 150; ++i) {log.info("Th A");}
    });

    auto th2 = thread([&log]{
        for (auto i = 0; i < 150; ++i) {log.info("Th B");}
    });

    log.debug("stopping now!");

    th1.join();
    th2.join();
}
