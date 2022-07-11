#include "stdafx.h"
#include "TCPClientModule.h"
#include "Proxy.h"

TCPClientModule::~TCPClientModule() {
	for (auto& iter : m_serverMap) {
		iter.second->Stop();
	}

	for (auto& iter : m_proxyThreadMap) {
		iter.second.join();
	}
}

void TCPClientModule::ConnectServer(std::string& _ip, uint16_t _port, uint16_t _serverSN) {
	asio::io_context* ioContext = new asio::io_context;
	tcp::resolver r(*ioContext);
	auto proxy = Proxy::Create(ioContext);

	proxy->Start(r.resolve(_ip, std::to_string(_port)));
	m_serverMap.emplace(_serverSN, proxy->GetPtr());

	m_proxyThreadMap.emplace(
		_serverSN,
		std::thread(
			[&ioContext]()->void {
				ioContext->run();
			})
	);
}

bool TCPClientModule::Send(uint16_t _serverSN, Buffer& _buffer) {
	const auto& findIter = m_serverMap.find(_serverSN);
	if (m_serverMap.end() == findIter)
		return false;

	findIter->second->Send(_buffer);
	return true;
}
