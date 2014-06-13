#include <cppa/cppa.hpp>
#include <cppa/optional.hpp>

using namespace std;
using namespace cppa;
using namespace cppa::io;

optional<uint16_t> as_u16(const std::string& str) 
{
	return static_cast<uint16_t>(stoul(str));
}

void connection_worker(broker* self, connection_handle hdl) 
{
	partial_function await_http {
		[=](const new_data_msg& msg) {
			if (msg.buf.size() < 1024) {
				std::string s = "HTTP/1.1 200 OK\n\
Content-Type: text/plain\n\
Date: Fri, 06 Jun 2014 17:02:24 GMT\n\
Connection: keep-alive\n\
Transfer-Encoding: chunked\n\
\n\
d\n\
Hello ++It!!\n\
\n\
0\n\
\n\
\n";
				self->write(msg.handle, s.size(), s.c_str());
				self->quit();
			}
		}
	};

	self->receive_policy(hdl, broker::at_most, 1024);
	self->become(await_http);
}

behavior server(broker *self)
{
	aout(self) << "server is running" << endl;
	return {
		[=](const new_connection_msg &msg)
		{
			// aout(self) << "server accepted new connection" << endl;
			auto io_actor = self->fork(connection_worker, msg.handle);
		},
		others() >> [ = ] {
			cout << "unexpected: " << to_string(self->last_dequeued()) << endl;
		}
	};
}

int main(int argc, char const *argv[])
{

	match(std::vector<string> {argv + 1, argv + argc}) (
		on("-p", as_u16) >> [&](uint16_t port)
	{
		cout << "run in server mode listen on: " << port << endl;
		auto sever_actor = spawn_io_server(server, port);
	},
	others() >> []
	{
		cerr << "use with '-p PORT' as server on port" << endl;
	}
	);
	await_all_actors_done();
	shutdown();

	return 0;
}
