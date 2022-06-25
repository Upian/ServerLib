#include <memory>
#include "TCPServerModule.h"
#include "Proxy.h"

int main(void) {
	asio::io_context io_context;
	tcp::resolver r(io_context);
	auto proxy = Proxy::Create(io_context);

	proxy->Start(r.resolve("127.0.0.1", "25000"));

	io_context.run();
}
