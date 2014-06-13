#include <cppa/cppa.hpp>

using namespace std;
using namespace cppa;

void worker(event_based_actor *self)
{
	aout(self) << "Worker started id:" << self->id() << endl;

	self->become(
		on(atom("EXIT")) >> [=]() {
			aout(self) << "WORKER EXIT NORMAL" << endl;
			self->quit();
		},
		on(atom("CRASH")) >> [=]() {
			aout(self) << "WORKER EXIT EXCEPTION" << endl;
			throw std::runtime_error("Managed CRASH!!!!");
			// self->quit(exit_reason::unhandled_exception);
		},
		others() >> [=]() {
			cout << "unexpected message: " << to_string(self->last_dequeued()) << endl;
		}
	);
}

void server(event_based_actor *self)
{
	aout(self) << "Server started!!!" << endl;

	auto wrk = self->spawn(worker);
	// auto wrk = self->spawn<linked>(worker);
	// auto wrk = self->spawn<monitored>(worker);
	// self->trap_exit(true);

	// self->delayed_send(wrk, chrono::seconds(5), atom("EXIT"));
	self->delayed_send(wrk, chrono::seconds(5), atom("CRASH"));
	
	self->send(self, atom("watchdog"));
	self->become(
		on_arg_match >> [=](const down_msg &dm) {
			cout << "Server receive DOWN from: " << dm.source.id() << " for reason: " << exit_reason::as_string(dm.reason) << endl;
		},
		on_arg_match >> [=](const exit_msg& em) {
			cout << "Server receive EXIT from: " << em.source.id() << " for reason: " << exit_reason::as_string(em.reason) << endl;
		},
		on(atom("watchdog")) >> [=](){
			aout(self) << "*** SERVER alive ***" << endl;
			self->delayed_send(self, chrono::seconds(7), atom("watchdog"));
		},
		others() >> [=]() {
			cout << "unexpected message: " << to_string(self->last_dequeued()) << endl;
		}
	);
}



int main(int argc, char const *argv[])
{
	auto srv = spawn(server);



	cout << "Wait all actors done" << endl;
	await_all_actors_done();
	cout << "Starting shoutdown" << endl;
	shutdown();
	cout << "Exit" << endl;
	return 0;
}
