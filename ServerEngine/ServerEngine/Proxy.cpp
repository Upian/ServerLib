#include "stdafx.h"
#include "Proxy.h"
#include <iostream>

void Proxy::Start(tcp::resolver::results_type _endpoints) {
	m_endpoints = _endpoints;
	this->StartConnect(m_endpoints.begin());
	m_deadline.async_wait(std::bind(&Proxy::CheckDeadline, this));
}

void Proxy::Stop() {
	m_stopped = true;
	std::error_code ignoredError;
	m_socket.close(ignoredError);
	m_deadline.cancel();
	m_heartbeatTimer.cancel();
}

Proxy::Proxy(asio::io_context& _ioContext) :
	m_socket(_ioContext),
	m_deadline(_ioContext),
	m_heartbeatTimer(_ioContext)
{ }

void Proxy::StartConnect(tcp::resolver::results_type::iterator _iterEndpoints) {
	if (_iterEndpoints != m_endpoints.end()) {
		//Log
		std::cout << "Trying " << _iterEndpoints->endpoint() << "...\n";

		// Set a deadline for the connect operation.
		m_deadline.expires_after(std::chrono::seconds(60));

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

		// Start the input actor.
		this->StartRead();
		
		// Start the heartbeat actor.
		this->StartWrite();
	}
}

void Proxy::StartRead() {
	// Set a deadline for the read operation.
	m_deadline.expires_after(std::chrono::seconds(30));

	// Start an asynchronous operation to read a newline-delimited message.
	asio::async_read_until(m_socket,
		asio::dynamic_buffer(m_inputBuffer), '\n',
		std::bind(&Proxy::HandleRead, this, _1, _2));
}

void Proxy::HandleRead(const std::error_code& _error, std::size_t _size) {
	if (true == m_stopped)
		return;

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

	// Start an asynchronous operation to send a heartbeat message.
	asio::async_write(m_socket, asio::buffer("\n", 1),
		std::bind(&Proxy::HandleWrite, this, _1));
}

void Proxy::HandleWrite(const std::error_code& _error) {
	if (m_stopped)
		return;

	if (!_error) {
		// Wait 10 seconds before sending the next heartbeat.
		m_heartbeatTimer.expires_after(std::chrono::seconds(10));
		m_heartbeatTimer.async_wait(std::bind(&Proxy::StartWrite, this));
	}
	else {
		std::cout << "Error on heartbeat: " << _error.message() << "\n";

		this->Stop();
	}
}

void Proxy::CheckDeadline() {
	if (m_stopped)
		return;

	// Check whether the deadline has passed. We compare the deadline against
	// the current time since a new asynchronous operation may have moved the
	// deadline before this actor had a chance to run.
	if (m_deadline.expiry() <= steady_timer::clock_type::now()) {
		// The deadline has passed. The socket is closed so that any outstanding
		// asynchronous operations are cancelled.
		m_socket.close();

		// There is no longer an active deadline. The expiry is set to the
		// maximum time point so that the actor takes no action until a new
		// deadline is set.
		m_deadline.expires_at(steady_timer::time_point::max());
	}

	// Put the actor back to sleep.
	m_deadline.async_wait(std::bind(&Proxy::CheckDeadline, this));
}
