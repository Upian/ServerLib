#include <memory>
#include "TCPServerModule.h"
#include "TCPClientModule.h"
#include "Proxy.h"

int main(void) {
	TCPClientModule client;
	std::string ip = "127.0.0.1";
	client.ConnectServer(ip, 25000, 1);
	Sleep(1000);
	Buffer buff1;
	buff1.SetBuffer("testMessage");
	client.Send(1, buff1);

	Sleep(500);
	Buffer buff2;
	buff2.SetBuffer("testMessage1");
	client.Send(1, buff2);

	Sleep(5000);
	Buffer buff3;
	buff3.SetBuffer("testMessage2");
	client.Send(1, buff3);
//	asio::io_context* io_context = new asio::io_context;
//	tcp::resolver r(*io_context);
//	auto proxy = Proxy::Create(io_context);
//	
//	proxy->Start(r.resolve("127.0.0.1", "25000"));
//
//	io_context->run();
	getchar();
}
