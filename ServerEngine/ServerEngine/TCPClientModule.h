#pragma once
#include "stdafx.h"

class Buffer;
class Proxy;
class TCPClientModule {
public:
	~TCPClientModule();
	void ConnectServer(std::string& _ip, uint16_t _port, uint16_t _serverSN);
	bool Send(uint16_t _serverSN, Buffer& _buffer);
private:
	std::map<uint16_t/*server sn*/, std::shared_ptr<Proxy> > m_serverMap;
	std::map<uint16_t/*server sn*/, std::thread> m_proxyThreadMap;
};