#include <cppa/cppa.hpp>

using namespace std;
using namespace cppa;
using namespace cppa::io;

void print_on_exit(const actor& ptr, const std::string& name) 
{
	ptr->attach_functor([=](std::uint32_t reason) {
		aout(ptr) << name << " exited with reason " << exit_reason::as_string(reason) << endl;
	});
}

optional<uint16_t> as_u16(const std::string& str) 
{
	return static_cast<uint16_t>(stoul(str));
}

void client_worker(broker *self, connection_handle hdl, const actor& parent)
{
	aout(self) << "Run client worker for " << parent->id() << endl;

	partial_function default_bhvr = {
		[=](const connection_closed_msg&) {
			aout(self) << "Client connection closed" << endl;
			self->quit(exit_reason::remote_link_unreachable);
		},
		[=](const down_msg& dm) {
			aout(self) << "Client down message" << endl;
			self->quit(exit_reason::remote_link_unreachable);
		},
		others() >> [=] {
			cout << "unexpected: " << to_string(self->last_dequeued()) << endl;
		}
	};

	partial_function await_http {
		[=](const new_data_msg& msg) {
			self->send(parent, atom("proxy"), msg.buf);
		},
		on(atom("proxy"), arg_match) >> [=](const util::buffer& buf) {
			std::string s((const char *)buf.data(), buf.size());
			aout(self) << s << endl;
			self->write(hdl, buf);
		},
		default_bhvr
	};

	// initial setup
	self->receive_policy(hdl, broker::at_most, 512);
	self->become(await_http);
}

void connection_worker(broker* self, connection_handle hdl, const string& dst_host, uint16_t dst_port) 
{
	aout(self) << "Run worker on " << dst_host << ":" << dst_port << endl;

	auto client = spawn_io_client(client_worker, dst_host, dst_port, self);
	self->link_to(client);

	partial_function default_bhvr = {
		[=](const connection_closed_msg&) {
			// aout(self) << "connection closed" << endl;
			self->quit(exit_reason::remote_link_unreachable);
		},
		[=](const down_msg& dm) {
			// aout(self) << "down_msg" << endl;
			self->quit(exit_reason::remote_link_unreachable);
		},
		others() >> [=] {
			cout << "unexpected: " << to_string(self->last_dequeued()) << endl;
		}
	};

	partial_function await_http {
		[=](const new_data_msg& msg) {
			self->send(client, atom("proxy"), msg.buf);
		},
		on(atom("proxy"), arg_match) >> [=](const util::buffer& buf) {
			//client->send(atom("proxy"), msg);
			//auto_ptrchar buff[buf.size() + 1];
			std::string s((const char *)buf.data(), buf.size());
			aout(self) << s << endl;
			self->write(hdl, buf);
		},
		default_bhvr
	};

	// initial setup
	self->receive_policy(hdl, broker::at_most, 512);
	self->become(await_http);
}

behavior server(broker *self, const string& dst_host, uint16_t dst_port)
{
	aout(self) << "server is running" << endl;
	return {
		[=](const new_connection_msg &msg)
		{
			aout(self) << "server accepted new connection" << endl;
			auto actor_worker = self->fork(connection_worker, msg.handle, dst_host, dst_port);
			print_on_exit(actor_worker, "connection_worker");

		},
		others() >> [=] {
			cout << "unexpected: " << to_string(self->last_dequeued()) << endl;
		}
	};
}

int main(int argc, char const *argv[])
{
	match(std::vector<string> {argv + 1, argv + argc}) (
		on("-p", as_u16, val<string>, as_u16) >> [&](uint16_t port, const string& dst_host, uint16_t dst_port)
		{
			cout << "run in server mode listen on: " << port << " redirect to: " << dst_host << ":" << dst_port << endl;
			auto sever_actor = spawn_io_server(server, port, dst_host, dst_port);
			print_on_exit(sever_actor, "server");
			// print_on_exit(pong_actor, "pong");
		},
		others() >> []
		{
			cerr << "use with eihter '-p listen-port host-destination port-destination' as server on port" << endl;
		});
	
	await_all_actors_done();
	shutdown();

	return 0;
}
