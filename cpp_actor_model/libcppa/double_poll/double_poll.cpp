#include <iostream>
#include <chrono>
#include <functional>
#include <vector>
#include <mutex>
#include <thread>
#include <utility>

using namespace std;
using namespace std::chrono;

class Job
{
	milliseconds _duration;
	time_point<system_clock> _lastExecution;
	function<void()> _action;

public:
	Job(milliseconds duration, const function<void()> &action)
	:_duration(duration), _action(action)
	{
		cout << "Create job for every: " << _duration .count() << " ms." << endl;
	}

	~Job()
	{
	}

	void execute()
	{
		if (!elapsed())
			return;

		_action();
		_lastExecution = system_clock::now();
	}

private:
	bool elapsed()
	{
		return (system_clock::now() - _lastExecution) > _duration;
	}
};

class Poller
{
	std::vector<Job*> _jobs;
	recursive_mutex _mutex;
	thread _thread; 
	std::string _name;

public:
	Poller(const string &name)
	:_name(name)
	{
		cout << "Poller " << _name << endl;

	}
	~Poller()
	{
		for(auto job : _jobs)
		{
			delete job;
		}
	}

	void start()
	{
		_thread = std::thread(&Poller::run, this);
	}

	void every(milliseconds duration, const function<void()> &action)
	{
		lock_guard<recursive_mutex> muard { _mutex };

		_jobs.push_back(new Job(duration, action));
	}

private:
	void run()
	{
		while(true)
		{
			this_thread::sleep_for(milliseconds(1));
			poll();			
		}
	}

	void poll()
	{
		lock_guard<recursive_mutex> muard { _mutex };
		vector<Job*> jobs = _jobs;
		for(auto job : jobs)
		{
			job->execute();
		}
	}

};

class Accumulator
{
	vector<pair<int, function<void()>>> _thresholds;
	int _counter;
	recursive_mutex _mutex;

public:
	Accumulator()
	:_counter(0)
	{
		cout << "Accumulator" << endl;
	}
	~Accumulator()
	{
		cout << "DTOR Accumulator" << endl;
	}

	void increase()
	{
		lock_guard<recursive_mutex> muard{_mutex};

		++_counter;
		checkThresholds();
	}

	void at(int threshold, const function<void()> &action)
	{
		lock_guard<recursive_mutex> muard{_mutex};

		_thresholds.push_back(make_pair(threshold, action));
	}

private:

	void checkThresholds()
	{
		for(auto &p : _thresholds) 
		{
			if (_counter > p.first)
				p.second();
		}
	}
};


int main(int argc, char const *argv[])
{
	Accumulator acc;
	Poller pollerLeft("left");
	Poller pollerRight("right");

	for (int i = 0; i < 100; ++i)
	{
		pollerRight.every(milliseconds(rand() % 500), [&] () {
			if (rand() % 2){
				cout << "Increase accumulator from pollerRight " << endl;
				acc.increase();
			}
		});

		pollerLeft.every(milliseconds(rand() % 600), [&] () {
			if (rand() % 3){
				cout << "Increase accumulator from pollerLeft " << endl;
				acc.increase();
			}
		});

	}

	acc.at(666, [&] {
		cout << "Threshold reach!!!!" << endl;
		
		cout << "add action to poller" << endl;
		if (rand() % 2)
		{
			pollerLeft.every(milliseconds(666), [&](){
				cout << "LEFT - 666 the number of the beast!!!" << endl;
			});

		}
		else
		{
			pollerRight.every(milliseconds(666), [&](){
				cout << "RIGHT - 666 the number of the beast!!!" << endl;
			});

		}
	});

	pollerRight.start();
	pollerLeft.start();

	string command;
	while(getline(cin, command))
	{
		cout << "execute command " << command << endl;
	}

	return 0;
}
