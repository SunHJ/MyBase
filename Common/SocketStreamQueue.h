#ifndef __NET_SOCKET_STREAM_QUEUE_H__
#define __NET_SOCKET_STREAM_QUEUE_H__

#include "SocketEvent.h"
#include "SocketStream.h"

class SocketStreamQueue
{
public:
	SocketStreamQueue();
	~SocketStreamQueue();
	BOOL SelectWait( 
		INT nMaxEventCount,
		INT &nEventCount,
		SPSocketEventArray spEventArray
		);

	BOOL PushBack(SPSocketStream &spSocketStream);
	BOOL Remove(SPSocketStream &spSocketStream);

	VOID CloseAll();
	size_t GetCurStreamVectorLen() CONST;
private:
	size_t	m_nLastWaitToProcessPos;
	SPSocketStreamVector m_vecSocketStream;

};
typedef SocketStreamQueue *PSocketStreamQueue;
typedef SharedPtr<SocketStreamQueue> SPSocketStreamQueue;

class AsyncSocketStreamQueue
{
public:
	AsyncSocketStreamQueue();
	~AsyncSocketStreamQueue();

	BOOL AddClient(SPAsyncSocketStream &spAsyncSocketStream);
	BOOL DelClient(SPAsyncSocketStream &spAsyncSocketStream);
	BOOL Wait(INT nMaxEventCount, INT &nEventCount, SPAsyncSocketEventArray spEventArray, INT nEpollHandle = -1);

	VOID CloseAll();
	size_t GetCurStreamVectorLen() CONST;
private:
	size_t	m_nLastWaitToProcessPos;
	VecSPAsyncSocketStream m_vecSocketStream;

#ifdef PLATFORM_OS_WINDOWS
	BOOL _WaitProcessRecvOrClose(INT nMaxEventCount, INT &nEventCount, SPAsyncSocketEventArray spEventArray);
#else
	BOOL _WaitProcessRecv(INT nMaxEventCount, INT &nEventCount, SPAsyncSocketEventArray spEventArray, INT nEpollHandle = -1);
#endif // PLATFORM_OS_WINDOWS	
};
typedef AsyncSocketStreamQueue *PAsyncSocketStreamQueue;
typedef SharedPtr<AsyncSocketStreamQueue> SPAsyncSocketStreamQueue;

#endif
