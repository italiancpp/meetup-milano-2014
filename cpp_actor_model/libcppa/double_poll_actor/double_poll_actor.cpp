#include <iostream>
#include <chrono>
#include <functional>
#include <vector>
#include <mutex>
#include <thread>
#include <utility>
#include <string>
#include <iostream>
#include <cppa/cppa.hpp>

using namespace std;
using namespace std::chrono;
using namespace cppa;

class Accumulator : public event_based_actor
{
	vector<pair<int, function<void()>>> _thresholds;
	int _counter;

	void checkThresholds()
	{
		for(auto &p : _thresholds) 
		{
			if (_counter > p.first)
				p.second();
		}
	}

protected:
	behavior make_behavior() override 
	{
		return (
			on(atom("inc")) >> [=] () 
			{
				aout(this) << "Increment counter from: " << _counter << " to " << _counter + 1 << endl; 
				++_counter;
				checkThresholds();
			},
			on(atom("at"), arg_match) >> [&] (int threshold, actor dest, atom_value msg)
			{
				aout(this) << "Register threshold at: " << threshold << " for actor " << dest << " msg: "<< to_string(msg) << endl; 
				_thresholds.push_back(make_pair(threshold, [=](){
					send(dest, msg);
				}));
			}
		);
	}

public:
	Accumulator()
		:_counter(0)
	{

	}

	~Accumulator()
	{

	}

	/* data */
};

class Poller : public event_based_actor
{
	string _name;
public:
	Poller(const string &name)
	:_name(name)
	{

	}
	~Poller()
	{

	}
protected:
	behavior make_behavior() override 
	{
		return (
			on(atom("fire"), arg_match) >> [=] (int delay, actor dest, atom_value msg)
			{
				// aout(this) << "send message " << to_string(msg) << " to: " << dest << endl;
				send(dest, msg);
				delayed_send(this, milliseconds(delay), atom("fire"), delay, dest, msg);
			},
			on(atom("poll"), arg_match) >> [=] (int delay, actor dest, atom_value msg) 
			{
				aout(this) << "actor " << dest << " want poll every " << delay << " msec" << endl;
				delayed_send(this, milliseconds(delay), atom("fire"), delay, dest, msg);
			}
		);
	}
};

behavior mainActor(event_based_actor *self)
{
	auto pollerLeft = spawn<Poller>("left");
	auto pollerRight = spawn<Poller>("right");
	auto accumulator = spawn<Accumulator>();

	for (int i = 0; i < 100; ++i)
	{
		// pollerRight.every(milliseconds(rand() % 500), [&] () {
		// 	if (rand() % 2){
		// 		cout << "Increase accumulator from pollerRight " << endl;
		// 		acc.increase();
		// 	}
		// });
		self->send(pollerLeft, atom("poll"), (int)milliseconds(rand() % 500).count(), self, atom("incLeft"));
		

		// pollerLeft.every(milliseconds(rand() % 600), [&] () {
		// 	if (rand() % 3){
		// 		cout << "Increase accumulator from pollerLeft " << endl;
		// 		acc.increase();
		// 	}
		// });
		self->send(pollerRight, atom("poll"), (int)milliseconds(rand() % 600).count(), self, atom("incRight"));
	}

	// acc.at(666, [&] {
	// 	cout << "Threshold reach!!!!" << endl;
		
	// 	cout << "add action to pollerLeft" << endl;
	// 	if (rand() % 2)
	// 	{
	// 		pollerLeft.every(milliseconds(666), [&](){
	// 			cout << "666 the number of the beast!!!" << endl;
	// 		});

	// 	}
	// 	else
	// 	{
	// 		pollerRight.every(milliseconds(666), [&](){
	// 			cout << "666 the number of the beast!!!" << endl;
	// 		});

	// 	}
	// });
	self->send(accumulator, atom("at"), 666, self, atom("threshold"));

	return (
		on(atom("incLeft")) >> [=] () 
		{
			if (rand() % 2){
				// aout(self) << "Increase accumulator from pollerRight " << endl;
				self->send(accumulator, atom("inc"));
			}
		},
		on(atom("incRight")) >> [=] () 
		{
			if (rand() % 3){
				// aout(self) << "Increase accumulator from pollerLeft " << endl;
				self->send(accumulator, atom("inc"));
			}
		},
		on(atom("threshold")) >> [=] () 
		{
			aout(self) << "add action to pollerLeft" << endl;
			if (rand() % 2)
			{
				// pollerLeft.every(milliseconds(666), [&](){
				// 	cout << "666 the number of the beast!!!" << endl;
				// });
				self->send(pollerLeft, atom("poll"), (int)milliseconds(666).count(), self, atom("beast"));
			}
			else
			{
				// pollerRight.every(milliseconds(666), [&](){
				// 	cout << "666 the number of the beast!!!" << endl;
				// });
				self->send(pollerLeft, atom("poll"), (int)milliseconds(666).count(), self, atom("beast"));
			}
		},
		on(atom("beast")) >> [=] ()
		{
			aout(self) << "666 the number of the beast!!!" << endl;
		}
	);
}

int main(int argc, char const *argv[])
{
	// // announce<milliseconds>();
	// scoped_actor self;

	auto main = spawn(mainActor);
	await_all_actors_done();

	return 0;
}
