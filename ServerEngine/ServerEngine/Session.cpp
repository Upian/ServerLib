#include "stdafx.h"
#include "Session.h"

void Session::Start() 
{
	this->DoRead();
	
}

Session::Session(tcp::socket _socket) :
	m_socket(std::move(_socket)) { }

void Session::ReadHeader() 
{
	auto self(shared_from_this());
	asio::async_read(m_socket,
		asio::buffer(m_readBuffer.GetBuffer(), kHEADER_SIZE),
		[this, self](std::error_code _error, std::size_t /*length*/) -> void {
			if (!_error && m_readBuffer.DecodeHeader()) 
			{
				this->ReadBody();
			}
			else 
			{
				//log
			}
		}
	);
}

void Session::ReadBody()
{
	auto self(shared_from_this());
	asio::async_read(m_socket,
		asio::buffer(m_readBuffer.GetBody(), m_readBuffer.GetBodySize()),
		[this, self](std::error_code _error, std::size_t /*length*/) -> void {
			if (!_error)
			{
				std::cout << "MSG: " << m_readBuffer.GetBody() << ": " << m_readBuffer.GetBodySize() << std::endl;
				m_readBuffer.Clear();
				this->ReadHeader();

			}
			else
			{
				//log
			}
		}
	);
}

void Session::DoRead()
{
	this->ReadHeader();
	//	auto self(shared_from_this());
	//	m_socket.async_read_some(asio::buffer(m_data, kMAX_BUFFER_SIZE),
	//		[this, self](std::error_code _error, std::size_t _length) ->void {
	//			if (!_error) {
	//				this->ReadHeader();
	////				DoWrite(_length);
	//			}		
	//		}
	//	);
}



void Session::DoWrite()
{
	auto self(shared_from_this());
	asio::async_write(m_socket, asio::buffer(m_writeBufferQueue.front().GetBuffer(), m_writeBufferQueue.front().GetBufferSize()),
		[this, self](std::error_code _error, std::size_t)->void {
			if (!_error)
			{
				m_writeBufferQueue.pop();
				if (false == m_writeBufferQueue.empty())
					this->DoWrite();
			}
			else
			{
				//Log
			}
		}
	);
}
