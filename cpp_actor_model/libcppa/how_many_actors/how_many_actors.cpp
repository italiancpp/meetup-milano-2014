#include <string>
#include <iostream>
#include "cppa/cppa.hpp"
#include "cppa/opt.hpp"

using namespace std;
using namespace cppa;
using namespace cppa::placeholders;

void worker(event_based_actor* self)
{
	aout(self) << "Starting actor: " << self->id() << endl;
	self->become (
		on_arg_match.when(_x1 == "exit") >> [=](const string& ) 
		{
			aout(self) << ".";
			self->quit();
		},
		on_arg_match.when(_x1 == "wait") >> [=](const string& what) 
		{
			std::chrono::milliseconds wait{std::rand() % 5000};
			aout(self) << "I'm Actor: " << self->id() << " wait for: " 
				<< wait.count() << " msec - This is my work: '" << what << "'" << endl;
			this_thread::sleep_for(wait);
			aout(self) << "Actor: " << self->id() << " END" << endl;
		},
		
		on_arg_match >> [=](const string& what) 
		{
			aout(self) << "I'm Actor: " << self->id() << " - This is my work: '" << what << "'" << endl;
		}
	);
}

string get_rand_word_from(const vector<string> &words)
{
	return words[rand() % words.size()];
}

template< class Rep, class Period >
void run_forever(const vector<actor> &actors, const std::chrono::duration<Rep,Period>& sleep_duration)
{
	vector<string> words {"job1", "gino", "pino", "jdjdjdjjj", "ggggggggggggggg"};
	while(true)
	{
		scoped_actor self;
		string job = get_rand_word_from(words);
		cout << "Sending " << job << " to all actors" << endl;
		for_each(begin(actors), end(actors), [&](actor act){ self->send(act, job); });
		this_thread::sleep_for(sleep_duration);
	}

}

void run_manual(const vector<actor> &actors)
{
	string job;
	while (getline(cin, job))
	{
		scoped_actor self;
		cout << "Sending " << job << " to all actors" << endl;
		for_each(begin(actors), end(actors), [&](actor act){ self->send(act, job); });
		if (job == "exit")
			break;
	}
}


int main(int argc, char* argv[])
{

	auto bool_rd_arg = [](bool &value) -> function<bool()> {
	  return [&value]() -> bool  {
		  value = true;
		  return true;

	  };
	};
	int actor_number = 10;
	int sleep_duration = 1000;
	bool command = false;
	options_description desc;
	bool args_valid = match_stream<string>(argv + 1, argv + argc) (
		on_opt1('n', "actor-number", &desc, "number of actor to spaw") >> rd_arg(actor_number),
		on_opt1('s', "sleep", &desc, "milliseconds to sleep") >> rd_arg(sleep_duration),
		on_opt0('c', "command", &desc, "run in command line mode") >> bool_rd_arg(command),
		on_opt0('h', "help", &desc, "print this help") >> print_desc_and_exit(&desc)
	);

	if (!args_valid)
		print_desc_and_exit(&desc)();

	// create a new actor that calls 'mirror()'
	vector<actor> actors;
	for (int i = 0; i < actor_number; ++i)
	{
		actors.push_back(spawn(worker));
	}

	if (command)
		run_manual(actors);
	else
		run_forever(actors, chrono::milliseconds(sleep_duration));

	cout << "Exit from loop" << endl;
	// run cleanup code before exiting main
	//await_all_others_done();
	cout << "All actors are dead!!" << endl;
	shutdown();
	cout << "exit" << endl;
}


