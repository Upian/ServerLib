#pragma once
#include "Common.h"

class Buffer {
public:
	const char* GetBuffer() const { return m_buffer; }
	size_t GetLength() const { return m_length; }

private:
	char m_buffer[kMAX_BUFFER_SIZE];
	size_t m_length = 0;
};