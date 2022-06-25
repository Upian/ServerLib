#include "stdafx.h"
#include "TCPServerModule.h"
#include "Session.h"

TCPServerModule::TCPServerModule(asio::io_context& _ioContext, short _port) :
	m_acceptor(_ioContext, tcp::endpoint(tcp::v4(), _port)),
	m_socket(_ioContext) { }
