#ifndef __NET_SOCKET_EVENT_H__
#define __NET_SOCKET_EVENT_H__

#include "SocketStream.h"
#include "SharedArrayPtr.h"

#define MAX_WAIT_ACCEPT_EVENT 32
#define MAX_SOCKET_EVENT (1024 + MAX_WAIT_ACCEPT_EVENT)

typedef enum _SocketEventType
{
	SOCKET_EVENT_INVALID	= 0,        // invalid event
	SOCKET_EVENT_ACCEPT		= 1,        // socket accept
	SOCKET_EVENT_CLOSE		= 2,		// socket ¹Ø±Õ
	SOCKET_EVENT_IN			= 3,        // socket ¿É¶Á
	SOCKET_EVENT_OUT		= 4,        // socket ¿ÉÐ´

}SocketEventType;

struct SocketEvent
{
public:
	SocketEventType m_nEventType;
	SPSocketStream  m_spAsyncSocketStream;

public:
	SocketEvent();
	~SocketEvent();

public:
	VOID Reset();
};

typedef SocketEvent *PSocketEvent;
typedef SharedArrayPtr<SocketEvent> SPSocketEventArray;


struct AsyncSocketEvent
{
public:
	SocketEventType		m_nEventType;
	PAsyncSocketStream	m_pAsyncSocketStream;

public:
	AsyncSocketEvent();
	~AsyncSocketEvent();

public:
	VOID Reset();
};

typedef AsyncSocketEvent *PAsyncSocketEvent;
typedef SharedArrayPtr<AsyncSocketEvent> SPAsyncSocketEventArray;

#endif	//__NET_SOCKET_EVENT_H__