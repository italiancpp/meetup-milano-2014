 
#include <iostream>
#include <vector>
#include <stack>
#include <tuple>
#include <algorithm>
#include <chrono>
#include <thread>
#include <future>
#include <mutex>
#include "helper.hpp"

using namespace std;


int main()
{
    // using lambdas inside a ternary expression

    auto t = ternary( 1 > 2, [](){ return 0; }, [](){ return 1; } );
    cout << t << endl;

    ternary( [](){ return false; }, [](){ cout << "true" << endl; }, [](){ cout << "false" << endl; } );
}
