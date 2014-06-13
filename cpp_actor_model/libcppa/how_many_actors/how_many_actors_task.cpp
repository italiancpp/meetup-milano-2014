#include <string>
#include <iostream>
#include <thread>
#include <chrono>
#include <future>
#include <vector>

using namespace std;

void worker(int i) 
{
	cout << "Starting thread: " << i << " - id: " << this_thread::get_id() << endl;

	while(true)
	{
		this_thread::sleep_for(chrono::milliseconds(5000));
		cout << "Working id: " << this_thread::get_id() << endl;
	}
}

int main() 
{
	// create a new actor that calls 'mirror()'
	vector<future<void>> threads;
	for (int i = 0; i < 10000; ++i)
	{
		auto th = async(launch::async, worker, i);
		threads.push_back(move(th));
	}
	cout << "while" << endl;
	while(true)
	{
		string s;
		getline(cin, s);
		// for_each(actors, (actor){ actor.send(job)});
	}
	// run cleanup code before exiting main
	return 0;
}
