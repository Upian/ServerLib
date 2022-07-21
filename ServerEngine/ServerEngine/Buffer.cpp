#include "stdafx.h"
#include "Buffer.h"

void Buffer::SetBufferSize(size_t _size) {
	m_bodySize = _size;
	if (m_bodySize > kMAX_BUFFER_SIZE)
		m_bodySize = kMAX_BUFFER_SIZE;
}

void Buffer::EncodeHeader() {
	char header[kHEADER_SIZE + 1] = "";
	std::sprintf(header, "%4d", static_cast<int>(m_bodySize));
	std::memcpy(m_buffer, header, kHEADER_SIZE);
}

bool Buffer::DecodeHeader() {
	char header[kHEADER_SIZE + 1] = "";
	std::strncat(header, m_buffer, kHEADER_SIZE);
	m_bodySize = std::atoi(header);
	if (m_bodySize > kMAX_BUFFER_SIZE) {
		m_bodySize = 0;
		return false;
	}
	return true;
}

//void Buffer::SetBuffer(const char* _buff) {
//	strncpy(m_buffer, _buff, strlen(_buff));	
//}

void Buffer::Clear() {
	memset(m_buffer, 0, kMAX_BUFFER_SIZE);
	m_bodySize = 0;
}
