#pragma once
#include "stdafx.h"
#include "Session.h"
using asio::ip::tcp;

class TCPServerModule {
public:
	TCPServerModule(asio::io_context& _ioContext, short _port);

	//Session을 상속받은 객체만 accept 가능
	template<typename T_Session,
		typename std::enable_if_t<std::is_base_of_v<Session, T_Session> >* = nullptr>
	void DoAccept();
private:

	tcp::acceptor m_acceptor;
	tcp::socket m_socket;
};

template<typename T_Session,
	typename std::enable_if_t<std::is_base_of_v<Session, T_Session> >*>
void TCPServerModule::DoAccept() {
	m_acceptor.async_accept(m_socket,
		[this](std::error_code _error) ->void {
			if (!_error) {
				Session::Create(std::move(m_socket))->Start();
//				std::make_shared<Session>(std::move(m_socket))->Start();
			}
			this->DoAccept<T_Session>();
		}
	);
}
