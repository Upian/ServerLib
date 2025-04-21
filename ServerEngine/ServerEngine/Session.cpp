#include "stdafx.h"
#include "Session.h"

asio::awaitable<void> Session::Start()
{
	co_await this->DoRead();
	co_return;
}

Session::Session(tcp::socket&& _socket) :
	m_socket(std::move(_socket)) { }

asio::awaitable<void> Session::DoRead()
{
	try
	{
		while (true)
		{
			co_await ReadHeader();
			co_await ReadBody();

			std::cout << "MSG: " << m_readBuffer.GetBody() << ": " << m_readBuffer.GetBodySize() << std::endl;
			m_readBuffer.Clear();
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << "DoRead Fail: " << e.what() << "\n";
	}

	co_return;
}

asio::awaitable<void> Session::ReadHeader()
{
	std::size_t n = co_await asio::async_read(
		m_socket,
		asio::buffer(m_readBuffer.GetBuffer(), kHEADER_SIZE),
		asio::use_awaitable
	);

	if (!m_readBuffer.DecodeHeader())
	{
		throw std::runtime_error("Header decode failed");
	}

	co_return;
}

asio::awaitable<void> Session::ReadBody()
{
	std::size_t n = co_await asio::async_read(
		m_socket,
		asio::buffer(m_readBuffer.GetBody(), m_readBuffer.GetBodySize()),
		asio::use_awaitable
	);

	co_return;
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
