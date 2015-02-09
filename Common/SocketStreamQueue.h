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

	BOOL AddClient(PAsyncSocketStream &pAsyncSocketStream);
	BOOL DelClient(PAsyncSocketStream &pAsyncSocketStream);
	BOOL Wait(INT nMaxEventCount, INT &nEventCount, SPAsyncSocketEventArray spEventArray);

	VOID BroadcastMsg(SPDynamicBuffer &spBuffer);
	VOID CloseAll();
	size_t GetCurStreamVectorLen() CONST;

private:
	size_t	m_nLastWaitToProcessPos;
	VecPAsyncSocketStream m_vecSocketStream;
};
typedef AsyncSocketStreamQueue *PAsyncSocketStreamQueue;
typedef SharedPtr<AsyncSocketStreamQueue> SPAsyncSocketStreamQueue;

#endif // __NET_SOCKET_STREAM_QUEUE_H__

