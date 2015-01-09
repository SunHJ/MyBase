#ifndef __NET_SOCKET_EVENT_H__
#define __NET_SOCKET_EVENT_H__

#include "SocketStream.h"
#include "SharedArrayPtr.h"

#define MAX_WAIT_ACCEPT_EVENT 32
#define MAX_SOCKET_EVENT (1024 + MAX_WAIT_ACCEPT_EVENT)

typedef enum _AsyncSocketEventType
{
	ASYNC_SOCKET_EVENT_INVALID	= 0,        // invalid event
	ASYNC_SOCKET_EVENT_ACCEPT	= 1,        // socket accept
	ASYNC_SOCKET_EVENT_CLOSE	= 2,		// socket ¹Ø±Õ
	ASYNC_SOCKET_EVENT_IN		= 3,        // socket ¿É¶Á
	ASYNC_SOCKET_EVENT_OUT		= 4,        // socket ¿ÉÐ´

}AsyncSocketEventType;

struct AsyncSocketEvent
{
public:
	AsyncSocketEventType m_nEventType;
	SPAsyncSocketStream  m_spAsyncSocketStream;

public:
	AsyncSocketEvent();
	~AsyncSocketEvent();

public:
	VOID Reset();
};

typedef AsyncSocketEvent *PAsyncSocketEvent;
typedef SharedArrayPtr<AsyncSocketEvent> SPAsyncSocketEventArray;

class AsyncSocketStreamQueue
{
private:
	typedef std::vector<SPAsyncSocketStream> SP_ASYNC_STREAM_VECTOR;

private:
	SP_ASYNC_STREAM_VECTOR m_vecAsyncSocketStream;
	size_t m_nLastWaitToProcessPos;                    // the pos of stream vector processed next time

public:
	AsyncSocketStreamQueue();
	~AsyncSocketStreamQueue();

public:
	BOOL Wait(
		INT nMaxEventCount,
		INT &nEventCount,
		SPAsyncSocketEventArray spEventArray
		);

	BOOL SelectWait(
		INT nMaxEventCount,
		INT &nEventCount,
		SPAsyncSocketEventArray spEventArray
		);

	BOOL PushBack(SPAsyncSocketStream &spAsyncSocketStream);
	BOOL Remove(SPAsyncSocketStream &spAsyncSocketStream);

	VOID CloseAll();
	size_t GetCurStreamVectorLen() CONST;

private:
	BOOL WaitProcessRecvOrClose(
		INT nMaxEventCount,
		INT &nEventCount,
		SPAsyncSocketEventArray spEventArray
		);
};
typedef AsyncSocketStreamQueue *PAsyncSocketStreamQueue;
typedef SharedPtr<AsyncSocketStreamQueue> SPAsyncSocketStreamQueue;

#endif	//__NET_SOCKET_EVENT_H__