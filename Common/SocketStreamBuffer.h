#ifndef __NET_SOCKET_STREAM_BUFFER_H__
#define __NET_SOCKET_STREAM_BUFFER_H__

#include "Type.h"
#include "Buffer.h"

#define DEFAULT_MAX_SOCKET_STREAM_BUFFER    (1024 * 1024)

class SocketStreamBuffer : private UnCopyable
{
private:
	PBYTE  m_pBuffer;                   // the real buffer ptr managed by BSocketStreamBuffer.
	size_t m_nTotalSize;                // the total size of m_pBuffer.
	// if m_nDataHeadPos == m_nDataTailPos, it indicates there is no un-read data.
	size_t m_nDataHeadPos;              // the begin pos of un-read data.					 
	// if m_nDataTailPos == m_nTotalSize, it indicates the left room of m_pBuffer is used out.
	size_t m_nDataTailPos;              // the next pos of un-read data's end pos.

public:
	SocketStreamBuffer();
	SocketStreamBuffer(CONST size_t nTotalSize);
	~SocketStreamBuffer();	
	// if return "TRUE", it indicates one complete package is received.
	// if return "FALSE", it indicates no complete package is received.
	BOOL GetPackage(SPDynamicBuffer &spBuffer);     // get one complete package every time.
	BOOL AddUsedSize(CONST size_t nDataBytes);		// add the used size by outer function manually.

public:
	PBYTE  GetLeftPtr()   CONST;
	size_t GetUsedSize()  CONST;        // return the used size of buffer.
	size_t GetLeftSize()  CONST;        // return the left size of buffer.
	size_t GetTotalSize() CONST;        // return the total size of buffer.
	size_t GetUnReadDataSize() CONST;   // return the size of data that is not read by outer function.

};					   
typedef SocketStreamBuffer *			PSocketStreamBuffer;
typedef SharedPtr<SocketStreamBuffer>	SPSocketStreamBuffer;
#endif //__NET_SOCKET_STREAM_BUFFER_H__