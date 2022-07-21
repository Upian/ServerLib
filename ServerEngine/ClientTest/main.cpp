#include <memory>
#include "TCPServerModule.h"
#include "TCPClientModule.h"
#include "Proxy.h"

int main(void) {
	TCPClientModule client;
	std::string ip = "127.0.0.1";
	client.ConnectServer(ip, 25000, 1);
	
	char line[kMAX_BUFFER_SIZE + 1];
	while (std::cin.getline(line, kMAX_BUFFER_SIZE + 1)) {
		Buffer msg;
//		msg.Clear();
		msg.SetBufferSize(std::strlen(line));
		std::memcpy(msg.GetBody(), line, msg.GetBodySize());
		msg.EncodeHeader();
		client.Send(1, msg);
	}
//	asio::io_context* io_context = new asio::io_context;
//	tcp::resolver r(*io_context);
//	auto proxy = Proxy::Create(io_context);
//	
//	proxy->Start(r.resolve("127.0.0.1", "25000"));
//
//	io_context->run();
	getchar();
}
