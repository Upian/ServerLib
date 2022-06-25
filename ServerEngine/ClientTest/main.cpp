#include <memory>
#include "TCPServerModule.h"

int main(void) {
	asio::io_context io_context;
	TCPServerModule server(io_context, 25000);
	server.DoAccept<Session>();
	io_context.run();

}
