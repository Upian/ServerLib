#pragma once
#include "stdafx.h"

using asio::ip::tcp;
class Session : public std::enable_shared_from_this<Session> {
public:
	std::shared_ptr<Session> GetPtr() {
		return shared_from_this();
	}

	[[nodiscard]] static std::shared_ptr<Session> Create(tcp::socket _socket) {
//		return std::make_shared<Session>(std::move(_socket));
		return std::shared_ptr<Session>(new Session(std::move(_socket))); //나중에 ObjectPool에서 가져올 필요 있다.
	}

	void Start();
	
	virtual void HandlePacket() {};
private:
	Session(tcp::socket _socket);

	void DoRead();
	void DoWrite(std::size_t _length);
	
	tcp::socket m_socket;
	char m_data[kMAX_BUFFER_SIZE];
};