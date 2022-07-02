#include "stdafx.h"
#include "Buffer.h"

void Buffer::SetBuffer(const char* _buff) {
	strncpy_s(m_buffer, _buff, strlen(_buff));	
}

void Buffer::Clear() {
	memset(m_buffer, 0, kMAX_BUFFER_SIZE);
}
