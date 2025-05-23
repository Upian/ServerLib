#include "stdafx.h"
#include "Session.h"

Session::~Session()
{
	std::cout << "Destroy session: " << m_connectionNum << std::endl;
}

asio::awaitable<void> Session::Start()
{
//	co_spawn(co_await asio::this_coro::executor, this->DoRead(), asio::detached);
	co_await this->DoRead();
	co_return;
}

Session::Session(tcp::socket&& _socket) :
	m_socket(std::move(_socket)) 
{
	static unsigned long long connectionNum = 0;
	m_connectionNum = ++connectionNum;
	std::cout << "Create session: " << m_connectionNum << std::endl;
}

asio::awaitable<void> Session::DoRead()
{
	try
	{
		while (true)
		{
			co_await ReadHeader();
			co_await ReadBody();

			std::cout << m_connectionNum << " MSG: " << m_readBuffer.GetBody() << ": " << m_readBuffer.GetBodySize() << std::endl;
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

	if (false == m_readBuffer.DecodeHeader())
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
