#include <memory>
#include "TCPServerModule.h"
#include "TCPClientModule.h"
#include "Proxy.h"

int main(void) {
	TCPClientModule client;
	std::string ip = "127.0.0.1";
	client.ConnectServer(ip, 25000, 1);
	Sleep(1000);
	Buffer buff;
	client.Send(1, buff);
	Sleep(500);
	client.Send(1, buff);
	Sleep(5000);
	client.Send(1, buff);
//	asio::io_context* io_context = new asio::io_context;
//	tcp::resolver r(*io_context);
//	auto proxy = Proxy::Create(io_context);
//	
//	proxy->Start(r.resolve("127.0.0.1", "25000"));
//
//	io_context->run();
	getchar();
}
