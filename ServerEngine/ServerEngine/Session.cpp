#include "stdafx.h"
#include "Session.h"

void Session::Start() {
	this->DoRead();
}

Session::Session(tcp::socket _socket) :
	m_socket(std::move(_socket)) { }

void Session::DoRead() {
	auto self(shared_from_this());
	m_socket.async_read_some(asio::buffer(m_data, kMAX_BUFFER_SIZE),
		[this, self](std::error_code _error, std::size_t _length) ->void {
			if (!_error) {
				std::cout << m_data << " Length: "<< _length << std::endl;
				DoWrite(_length);
//				memset(m_data, 0, kMAX_BUFFER_SIZE);
			}		
		}
	);
}

void Session::DoWrite(std::size_t _length) {
	auto self(shared_from_this());
	asio::async_write(m_socket, asio::buffer(m_data, _length),
		[this, self](std::error_code _error, std::size_t)->void {
			if (!_error) {
				DoRead();
			}
		}
	);
}
