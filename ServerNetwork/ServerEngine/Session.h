#pragma once
#include "stdafx.h"
#include "Buffer.h"
#include "ObjectPool.h"
using asio::ip::tcp;
using std::placeholders::_1;
using std::placeholders::_2;

//������ ��ü
class Session : public std::enable_shared_from_this<Session> 
{
	template<typename T_Type>
	friend class ObjectPool;
public:
	std::shared_ptr<Session> GetPtr() 
	{
		return shared_from_this();
	}

	virtual ~Session();

	template<typename T_Session>
		requires std::derived_from<T_Session, Session>
	static std::shared_ptr<T_Session> Create(tcp::socket&& _socket);

	asio::awaitable<void> Start();
	
	virtual void HandlePacket() {};

private:
	Session(tcp::socket&& _socket);
	template<typename T_Session>
		requires std::derived_from<T_Session, Session>
	static void DestroySession(T_Session* _session);

	asio::awaitable<void> DoRead();
	asio::awaitable<void> ReadHeader();
	asio::awaitable<void> ReadBody();

	void DoWrite();
	
	tcp::socket m_socket;
	Buffer m_readBuffer;
	std::queue<Buffer> m_writeBufferQueue;
	unsigned long long m_connectionNum = 0;
};

//ObjectPool
template<typename T_Session>
	requires std::derived_from<T_Session, Session>
ObjectPool<T_Session> m_sessionPool;

template<typename T_Session>
	requires std::derived_from<T_Session, Session>
static std::shared_ptr<T_Session> Session::Create(tcp::socket&& _socket)
{
//	return std::make_shared<Session>(std::move(_socket)); //�̵� �����ڰ� private��..
	T_Session* session = m_sessionPool<T_Session>.Alloc(std::move(_socket));
	if (nullptr == session)
		session = new T_Session(std::move(_socket));
	return std::shared_ptr<T_Session>(session, Session::DestroySession<T_Session>); //���߿� ObjectPool���� ������ �ʿ� �ִ�.
}

template<typename T_Session>
	requires std::derived_from<T_Session, Session>
static void Session::DestroySession(T_Session* _session)
{
	//��ȯ
	m_sessionPool<T_Session>.Release(_session);
}