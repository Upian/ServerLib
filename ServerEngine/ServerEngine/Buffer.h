#pragma once
#include "Common.h"
#include "ObjectPool.h"

class Buffer {
public:
	const char* GetBuffer() const { return m_buffer; }
	char* GetBuffer() { return m_buffer; }
	size_t GetBufferSize() const { return kHEADER_SIZE + m_bodySize; }
	void SetBufferSize(size_t _size);

	const char* GetBody() const { return m_buffer + kHEADER_SIZE; }
	char* GetBody() { return m_buffer + kHEADER_SIZE; }
	size_t GetBodySize() const { return m_bodySize; }
//	void SetBuffer(const char* _buff);

	void EncodeHeader();
	bool DecodeHeader();

	void Clear();
private:
	char m_buffer[kHEADER_SIZE + kMAX_BUFFER_SIZE];
	size_t m_bodySize = 0;
};

class BufferPool : public ObjectPool<Buffer>{
public:

private:

};
