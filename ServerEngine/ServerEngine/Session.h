#pragma once
#include "stdafx.h"
#include "Buffer.h"

using asio::ip::tcp;
using std::placeholders::_1;
using std::placeholders::_2;

class Session : public std::enable_shared_from_this<Session> 
{
public:
	std::shared_ptr<Session> GetPtr() 
	{
		return shared_from_this();
	}

	template<typename T_Session>
	requires std::derived_from<T_Session, Session>
	static std::shared_ptr<T_Session> Create(tcp::socket&& _socket);

	asio::awaitable<void> Start();
	
	virtual void HandlePacket() {};
private:
	Session(tcp::socket&& _socket);
	
	asio::awaitable<void> DoRead();
	asio::awaitable<void> ReadHeader();
	asio::awaitable<void> ReadBody();

	void DoWrite();
	
	tcp::socket m_socket;
	Buffer m_readBuffer;
	std::queue<Buffer> m_writeBufferQueue;
};

template<typename T_Session>
requires std::derived_from<T_Session, Session>
static std::shared_ptr<T_Session> Session::Create(tcp::socket&& _socket)
{
	return std::make_shared<Session>(std::move(_socket));
//	return std::shared_ptr<T_Session>(new T_Session(std::move(_socket))); //나중에 ObjectPool에서 가져올 필요 있다.
}