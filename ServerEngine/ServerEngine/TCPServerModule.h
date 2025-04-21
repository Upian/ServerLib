#pragma once
#include "stdafx.h"
#include "Session.h"
#include <concepts>
using asio::ip::tcp;

class TCPServerModule 
{
public:
	TCPServerModule(asio::io_context& _ioContext, short _port);

	//Session을 상속받은 객체만 accept 가능
	template<typename T_Session>
	requires std::derived_from<T_Session, Session>
	void DoAccept();
private:

	tcp::acceptor m_acceptor;
	tcp::socket m_socket;
};

template<typename T_Session>
requires std::derived_from<T_Session, Session>
void TCPServerModule::DoAccept() 
{
	m_acceptor.async_accept(m_socket,
		[this](std::error_code _error) -> void 
		{
			if (!_error) {
				T_Session::Create<T_Session>(std::move(m_socket))->Start();
//				std::make_shared<Session>(std::move(m_socket))->Start();
				//Log
			}
			this->DoAccept<T_Session>();
		}
	);
}
