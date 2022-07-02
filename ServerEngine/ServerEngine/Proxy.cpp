#include "stdafx.h"
#include "Proxy.h"
#include <iostream>

Proxy::~Proxy() {
	delete m_ioContext;
	m_ioContext = nullptr;
}

void Proxy::Start(tcp::resolver::results_type _endpoints) {
	m_endpoints = _endpoints;
	this->StartConnect(m_endpoints.begin());
}

void Proxy::Stop() {
	m_stopped = true;
	std::error_code ignoredError;
	m_socket.close(ignoredError);
}

void Proxy::Send(Buffer& _buffer) { //나중에 패킷 풀로 변경
	if (false == m_isConnect)
		return;

	asio::post(*m_ioContext,
		[this, &_buffer]()->void {
			bool isEmptyQ = m_sendBufferQueue.empty();
			m_sendBufferQueue.push(_buffer);
			if (true == isEmptyQ) {
				this->StartWrite();
			}
		}
	);
}

Proxy::Proxy(asio::io_context* _ioContext) :
	m_socket(*_ioContext),
	m_ioContext(_ioContext)
{ }

void Proxy::StartConnect(tcp::resolver::results_type::iterator _iterEndpoints) {
	if (_iterEndpoints != m_endpoints.end()) {
		//Log
		std::cout << "Trying " << _iterEndpoints->endpoint() << "...\n";

		// Start the asynchronous connect operation.
		m_socket.async_connect(_iterEndpoints->endpoint(),
			std::bind(&Proxy::HandleConnect,
				this, _1, _iterEndpoints));
	}
	else {
		// There are no more endpoints to try. Shut down the client.
		this->Stop();
	}
}

void Proxy::HandleConnect(const std::error_code& _error, tcp::resolver::results_type::iterator _iterEndpoints) {
	if (true == m_stopped)
		return;

	// The async_connect() function automatically opens the socket at the start
	// of the asynchronous operation. If the socket is closed at this time then
	// the timeout handler must have run first.
	if (false == m_socket.is_open()) {
		std::cout << "Connect timed out\n";

		// Try the next available endpoint.
		this->StartConnect(++_iterEndpoints);
	}

	// Check if the connect operation failed before the deadline expired.
	else if (_error) {
		std::cout << "Connect error: " << _error.message() << "\n";

		// We need to close the socket used in the previous connection attempt
		// before starting a new one.
		m_socket.close();

		// Try the next available endpoint.
		this->StartConnect(++_iterEndpoints);
	}

	// Otherwise we have successfully established a connection.
	else {
		std::cout << "Connected to " << _iterEndpoints->endpoint() << "\n";
		m_isConnect = true;
		// Start the input actor.
		this->StartRead();
	}
}

void Proxy::StartRead() {
	// Start an asynchronous operation to read a newline-delimited message.
	asio::async_read_until(m_socket,
		asio::dynamic_buffer(m_inputBuffer), '\n',
		std::bind(&Proxy::HandleRead, this, _1, _2));
}

void Proxy::HandleRead(const std::error_code& _error, std::size_t _size) {
	if (true == m_stopped)
		return;
	std::cout << "Read"<<m_inputBuffer << std::endl;
	if (!_error) {
		// Extract the newline-delimited message from the buffer.
		std::string line(m_inputBuffer.substr(0, _size - 1));
		m_inputBuffer.erase(0, _size);

		// Empty messages are heartbeats and so ignored.
		if (!line.empty()) {
			std::cout << "Received: " << line << "\n";
		}

		this->StartRead();
	}
	else {
		std::cout << "Error on receive: " << _error.message() << "\n";

		this->Stop();
	}
}

void Proxy::StartWrite() {
	if (true == m_stopped)
		return;

	// Start an asynchronous operation to send a message
	auto& buff = m_sendBufferQueue.front();
	std::cout << buff.GetBuffer() << "\tLength: "<< buff.GetLength() << std::endl; //Test
	asio::async_write(m_socket, asio::buffer(buff.GetBuffer(), buff.GetLength()),
		std::bind(&Proxy::HandleWrite, this, _1));
	
}

void Proxy::HandleWrite(const std::error_code& _error) {
	if (m_stopped)
		return;

	if (!_error) {
		m_sendBufferQueue.pop();
		if (false == m_sendBufferQueue.empty())
			this->StartWrite();
	}
	else {
		this->Stop();
	}
}