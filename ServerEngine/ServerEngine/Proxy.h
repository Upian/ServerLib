#pragma once
//서버로의 연결 객채
#include "stdafx.h"
#include "Buffer.h"
using asio::steady_timer;
using asio::ip::tcp;
using std::placeholders::_1;
using std::placeholders::_2;

class Proxy : public std::enable_shared_from_this<Proxy> {
public:
	virtual ~Proxy();
	std::shared_ptr<Proxy> GetPtr() {
		return shared_from_this();
	}

	[[nodiscard]] static std::shared_ptr<Proxy> Create(asio::io_context* _ioContext) {
		//		return std::make_shared<Session>(std::move(_socket));
		return std::shared_ptr<Proxy>(new Proxy(_ioContext)); //나중에 ObjectPool에서 가져올 필요 있다.
	}

	void Start(tcp::resolver::results_type _endpoints);
	void Stop();
	void Send(Buffer& _buffer);
private:
	Proxy(asio::io_context* _ioContext);
	
	void StartConnect(tcp::resolver::results_type::iterator _iterEndpoints);
	void HandleConnect(const std::error_code& _error, tcp::resolver::results_type::iterator _iterEndpoints);
	
	void StartRead();
	void HandleRead(const std::error_code& _error, std::size_t _size);

	void StartWrite();
	void HandleWrite(const std::error_code& _error);

	bool m_stopped = false;
	bool m_isConnect = false;

	tcp::resolver::results_type m_endpoints;
	tcp::socket m_socket;
	asio::io_context* m_ioContext = nullptr;

	std::string m_inputBuffer;
	std::queue<Buffer> m_sendBufferQueue;
};