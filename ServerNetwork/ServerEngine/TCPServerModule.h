#pragma once
#include "stdafx.h"
#include "Session.h"
#include <concepts>
using asio::ip::tcp;
namespace this_coro = asio::this_coro;

class TCPServerModule 
{
public:
	TCPServerModule(asio::io_context& _ioContext, short _port);

	template<typename T_Session = Session>
		requires std::derived_from<T_Session, Session>
	void Run();

private:
	//Session을 상속받은 객체만 accept 가능
	template<typename T_Session = Session>
		requires std::derived_from<T_Session, Session>
	asio::awaitable<void> DoAccept();

private:
	asio::io_context& m_ioContext;
	tcp::acceptor m_acceptor;
	tcp::socket m_socket;
};

template<typename T_Session>
	requires std::derived_from<T_Session, Session>
void TCPServerModule::Run()
{
	asio::co_spawn(m_ioContext, DoAccept<T_Session>(), asio::detached);
}

template<typename T_Session>
	requires std::derived_from<T_Session, Session>
asio::awaitable<void> TCPServerModule::DoAccept()
{
	auto executor = co_await this_coro::executor;
	static unsigned long long connectionNumber = 0;
	while (true)
	{
		try
		{
			tcp::socket socket = co_await m_acceptor.async_accept(asio::use_awaitable);
			auto session = Session::Create<T_Session>(std::move(socket));
			asio::co_spawn(executor,
				[session]() -> asio::awaitable<void> {
					co_await session->Start();
					co_return;
				},
				asio::detached);
		}
		catch (const std::exception& e)
		{
			std::cerr << "Accept error: " << e.what() << '\n';
		}
	}
}
