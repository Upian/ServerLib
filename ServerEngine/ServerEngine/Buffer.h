#pragma once
#include "Common.h"
#include "ObjectPool.h"

class Buffer {
public:
	const char* GetBuffer() const { return m_buffer; }
	char* GetBuffer() { return m_buffer; }
	size_t GetLength() const { return strlen(m_buffer); }
	void SetBuffer(const char* _buff);

	void Clear();
private:
	char m_buffer[kMAX_BUFFER_SIZE];
};

class BufferPool : public ObjectPool<Buffer>{
public:

private:

};
